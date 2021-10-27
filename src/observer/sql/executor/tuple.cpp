/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "common/log/log.h"
#include <cfloat>

Tuple::Tuple(const Tuple &other) {
  LOG_PANIC("Copy constructor of tuple is not supported");
  exit(1);
}

Tuple::Tuple(Tuple &&other) noexcept : values_(std::move(other.values_)) {
}

Tuple & Tuple::operator=(Tuple &&other) noexcept {
  if (&other == this) {
    return *this;
  }

  values_.clear();
  values_.swap(other.values_);
  return *this;
}

Tuple::~Tuple() {
}

// add (Value && value)
void Tuple::add(TupleValue *value) {
  values_.emplace_back(value);
}
void Tuple::add(const std::shared_ptr<TupleValue> &other) {
  values_.emplace_back(other);
}
void Tuple::add(int value, AttrType type)
{
    if(type==DATES)
        add(new DateValue(value));
    else if (type == INTS)
        add(new IntValue(value));
}

void Tuple::add(float value) {
  add(new FloatValue(value));
}

void Tuple::add(const char *s, int len) {
  add(new StringValue(s, len));
}

// 新增Date类型处理
// void Tuple::add(int unix_time,AttrType type)
// {
//     add(new DateValue(unix_time));
// }

////////////////////////////////////////////////////////////////////////////////

std::string TupleField::to_string() const {
  return std::string(table_name_) + "." + field_name_ + std::to_string(type_);
}

std::string AggrField::to_string() const {
  if(aggr_type_ == AggrType::AGGR_UNDEFINED) {
    return "AggrTypeError";
  }
  if (aggr_type_ == AggrType::COUNT) {
    return "COUNT(" + std::string(table_name_) + "." + field_name_ + std::to_string(type_) + ")";
  } else if (aggr_type_ == AggrType::AVG) {
    return "AVG(" + std::string(table_name_) + "." + field_name_ + std::to_string(type_) + ")";
  } else if (aggr_type_ == AggrType::MAX) {
    return "MAX(" + std::string(table_name_) + "." + field_name_ + std::to_string(type_) + ")";
  } else {
    return "MIN(" + std::string(table_name_) + "." + field_name_ + std::to_string(type_) + ")";
  }
}

////////////////////////////////////////////////////////////////////////////////
void TupleSchema::from_table(const Table *table, TupleSchema &schema) {
  const char *table_name = table->name();
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = 0; i < field_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i);
    if (field_meta->visible()) {
      schema.add(field_meta->type(), table_name, field_meta->name());
    }
  }
}

void TupleSchema::add(AttrType type, const char *table_name, const char *field_name) {
  fields_.emplace_back(type, table_name, field_name);
}

void TupleSchema::add_if_not_exists(AttrType type, const char *table_name, const char *field_name) {
  for (const auto &field: fields_) {
    if (0 == strcmp(field.table_name(), table_name) &&
        0 == strcmp(field.field_name(), field_name)) {
      return;
    }
  }

  add(type, table_name, field_name);
}

void TupleSchema::add_aggr(AggrType aggr_type, AttrType type, const char *table_name, const char *field_name, const Value* constant_value) {
  aggr_fields_.emplace_back(aggr_type, type, table_name, field_name, constant_value);
}

void TupleSchema::add_aggr_if_not_exists(AggrType aggr_type, AttrType type, const char *table_name, const char *field_name) {
  for (const auto &aggr_field: aggr_fields_) {
    if (aggr_type == aggr_field.aggr_type() &&
        0 == strcmp(aggr_field.table_name(), table_name) &&
        0 == strcmp(aggr_field.field_name(), field_name)) {
      return;
    }
  }

  add(type, table_name, field_name);
}

void TupleSchema::append(const TupleSchema &other) {
  fields_.reserve(fields_.size() + other.fields_.size());
  for (const auto &field: other.fields_) {
    fields_.emplace_back(field);
  }
}

int TupleSchema::index_of_field(const char *table_name, const char *field_name) const {
  const int size = fields_.size();
  for (int i = 0; i < size; i++) {
    const TupleField &field = fields_[i];
    if (0 == strcmp(field.table_name(), table_name) && 0 == strcmp(field.field_name(), field_name)) {
      return i;
    }
  }
  return -1;
}

void TupleSchema::print(std::ostream &os , bool is_single_table) const {
  if (fields_.empty() && aggr_fields_.empty()) {
    os << "No schema";
    return;
  }

  if (aggr_fields_.size() > 0) {
    // 判断有多张表还是只有一张表
    std::set<std::string> table_names;
    for (const auto &field: aggr_fields_) {
      table_names.insert(field.table_name());
    }
    for (std::vector<AggrField>::const_iterator iter = aggr_fields_.begin(), end = --aggr_fields_.end();
      iter != end; ++iter) {
      
      std::string aggr_all_field_name;
      std::string aggr_name;
      if(iter->constant_value() != nullptr) {
        switch(iter->constant_value()->type) {
          case AttrType::INTS: {
            int value = *(int *)iter->constant_value()->data;
            aggr_name = std::to_string(value);
            break;
          }
          case AttrType::FLOATS: {
            float value = *(float *)iter->constant_value()->data;
            char buf[128];
            sprintf(buf, "%.2f", value);
          //   std::string s(buf);
            for (size_t i = 0; i < 128; i++) {
              if (buf[i] == '.') {
                // 最后一位
                if (i + 2 < 128 && buf[i + 2] == '0') {
                  buf[i + 2] = '\0';

                  if (i + 1 < 128 && buf[i + 1] == '0') {
                    buf[i] = '\0';
                  }
                }
                break;
              }
            }
            aggr_name = std::string(buf, strlen(buf));
            break;
          }
          case AttrType::CHARS: {
            char* value = (char*)iter->constant_value()->data;
            aggr_name = std::string(value, strlen(value));
            break;
          }
          default:
            break;
        }
      } else {
        if (table_names.size() > 1) {
          aggr_name = iter->table_name() + std::string(".");
        }
        aggr_name += std::string(iter->field_name());
      }
      switch(iter->aggr_type()) {
        case AggrType::COUNT: {
          aggr_all_field_name = std::string("COUNT(") + aggr_name + std::string(")");
          break;
        }
        case AggrType::AVG: {
          aggr_all_field_name = std::string("AVG(") + aggr_name + std::string(")");
          break;
        }
        case AggrType::MIN: {
          aggr_all_field_name = std::string("MIN(") + aggr_name + std::string(")");
          break;
        }
        case AggrType::MAX: {
          aggr_all_field_name = std::string("MAX(") + aggr_name + std::string(")");
          break;
        }
        default:
          LOG_DEBUG("Aggregation AGGR_UNDEFINED.");
      }
      os << aggr_all_field_name << " | ";
    }
    if (table_names.size() > 1) {
      os << fields_.back().table_name() << ".";
    }
    std::string aggr_all_field_name;
    std::string aggr_name;
    if(aggr_fields_.back().constant_value() != nullptr) {
      switch(aggr_fields_.back().constant_value()->type) {
        case AttrType::INTS: {
          int value = *(int *)aggr_fields_.back().constant_value()->data;
          aggr_name = std::to_string(value);
          break;
        }
        case AttrType::FLOATS: {
          float value = *(float *)aggr_fields_.back().constant_value()->data;
          char buf[128];
          sprintf(buf, "%.2f", value);
          for (size_t i = 0; i < 128; i++) {
            if (buf[i] == '.') {
              // 最后一位
              if (i + 2 < 128 && buf[i + 2] == '0') {
                buf[i + 2] = '\0';

                if (i + 1 < 128 && buf[i + 1] == '0') {
                  buf[i] = '\0';
                }
              }
              break;
            }
          }
          aggr_name = std::string(buf, strlen(buf));
          break;
        }
        case AttrType::CHARS: {
          char* value = (char*)aggr_fields_.back().constant_value()->data;
          aggr_name = std::string(value, strlen(value));
          break;
        }
        default:
          break;
      }
    } else {
      if (table_names.size() > 1) {
        aggr_name = aggr_fields_.back().table_name() + std::string(".");
      }
      aggr_name += std::string(aggr_fields_.back().field_name(), strlen(aggr_fields_.back().field_name()));
    }
    switch(aggr_fields_.back().aggr_type()) {
      case AggrType::COUNT: {
        aggr_all_field_name = std::string("COUNT(") + aggr_name + std::string(")");
        break;
      }
      case AggrType::AVG: {
        aggr_all_field_name = std::string("AVG(") + aggr_name + std::string(")");
        break;
      }
      case AggrType::MIN: {
        aggr_all_field_name = std::string("MIN(") + aggr_name + std::string(")");
        break;
      }
      case AggrType::MAX: {
        aggr_all_field_name = std::string("MAX(") + aggr_name + std::string(")");
        break;
      }
      default:
        LOG_DEBUG("Aggregation AGGR_UNDEFINED.");
    }
    os << aggr_all_field_name << std::endl;
  } else {
    // 判断有多张表还是只有一张表
    std::set<std::string> table_names;
    for (const auto &field: fields_) {
      table_names.insert(field.table_name());
    }
    for (std::vector<TupleField>::const_iterator iter = fields_.begin(), end = --fields_.end();
      iter != end; ++iter) {
      if (!is_single_table) {
        os << iter->table_name() << ".";
      }
      os << iter->field_name() << " | ";
    }
    if (!is_single_table) {
      os << fields_.back().table_name() << ".";
    }
    os << fields_.back().field_name() << std::endl;
  }

}

/////////////////////////////////////////////////////////////////////////////
TupleSet::TupleSet(TupleSet &&other) : tuples_(std::move(other.tuples_)), schema_(other.schema_){
  other.schema_.clear();
}

TupleSet &TupleSet::operator=(TupleSet &&other) {
  if (this == &other) {
    return *this;
  }

  schema_.clear();
  schema_.append(other.schema_);
  other.schema_.clear();

  tuples_.clear();
  tuples_.swap(other.tuples_);
  return *this;
}

void TupleSet::add(Tuple &&tuple) {
  tuples_.emplace_back(std::move(tuple));
}

void TupleSet::clear() {
  tuples_.clear();
  schema_.clear();
}

void TupleSet::print(std::ostream &os, bool is_single_table) const {
  if (schema_.fields().empty() && schema_.aggr_fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  schema_.print(os, is_single_table);

  for (const Tuple &item : tuples_) {
      const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
      for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
            iter != end; ++iter) {
        (*iter)->to_string(os);
        os << " | ";
      }
      values.back()->to_string(os);
      os << std::endl;
    }
}

void TupleSet::set_schema(const TupleSchema &schema) {
  schema_ = schema;
}

const TupleSchema &TupleSet::get_schema() const {
  return schema_;
}

bool TupleSet::is_empty() const {
  return tuples_.empty();
}

int TupleSet::size() const {
  return tuples_.size();
}

const Tuple &TupleSet::get(int index) const {
  return tuples_[index];
}

const std::vector<Tuple> &TupleSet::tuples() const {
  return tuples_;
}

/////////////////////////////////////////////////////////////////////////////
TupleRecordConverter::TupleRecordConverter(Table *table, TupleSet &tuple_set) :
      table_(table), tuple_set_(tuple_set){
}

void TupleRecordConverter::add_record(const char *record) {
  const TupleSchema &schema = tuple_set_.schema();
  Tuple tuple;
  const TableMeta &table_meta = table_->table_meta();
  for (const TupleField &field : schema.fields()) {
    const FieldMeta *field_meta = table_meta.field(field.field_name());
    assert(field_meta != nullptr);
    switch (field_meta->type()) {
      case INTS: {
        int8_t is_null;
        memcpy(&is_null, record+field_meta->offset()+field_meta->len()-1, 1);
        if(is_null == 0){
          int value = *(int*)(record + field_meta->offset());
          tuple.add(value,INTS);
        } else {
          tuple.add(new NullValue());
        }
      }
      break;
      case FLOATS: {
        int8_t is_null;
        memcpy(&is_null, record+field_meta->offset()+field_meta->len()-1, 1);
        if(is_null == 0) {
          float value = *(float *)(record + field_meta->offset());
          tuple.add(value);
        } else {
          tuple.add(new NullValue());
        }
      }
        break;
      case CHARS: {
        int8_t is_null;
        memcpy(&is_null, record+field_meta->offset()+field_meta->len()-1, 1);
        if(is_null == 0) {
          const char *s = record + field_meta->offset();  // 现在当做Cstring来处理
          tuple.add(s, strlen(s));
        } else {
          tuple.add(new NullValue());
        }
      }
      break;
      case DATES:
      { 
        int8_t is_null;
        memcpy(&is_null, record+field_meta->offset()+field_meta->len()-1, 1);
        if (is_null == 0) {
          //   LOG_ERROR("DATES type test-1!");
          int value = *(int *)(record + field_meta->offset()); // 现在当做Cstring来处理
          //   LOG_ERROR("DATES type test-2!");
            tuple.add(value,DATES);
          //   LOG_ERROR("DATES type test-3!");
        } else {
          tuple.add(new NullValue());
        }
        
      }
      break;
      default: {
        LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
      }
    }
  }

//   LOG_DEBUG("DATES type test-4!");
  tuple_set_.add(std::move(tuple));
//   LOG_DEBUG("DATES type test-5!");
}

AggregationRecordConverter::AggregationRecordConverter(Table *table, TupleSet &tuple_set, std::vector<const AggrAttr *> &aggr_attrs) :
      table_(table), tuple_set_(tuple_set), aggr_attrs_(aggr_attrs) {
      const std::vector<AggrField> & aggr_fields = tuple_set_.schema().aggr_fields();
      aggr_results_.resize(aggr_fields.size());
      line_counts_.resize(aggr_fields.size());
      Tuple tuple;
      for (int i=0; i<aggr_fields.size(); i++) {
        const AggrField &aggr_field = aggr_fields[i];
        const Value* constant_value = aggr_field.constant_value();
        if (constant_value != nullptr) {
          switch (aggr_field.aggr_type()) {
            case AggrType::COUNT:
              aggr_results_[i] = new IntValue(0);
              break;
            case AggrType::AVG:
              if(constant_value->type == AttrType::INTS) {
                int value = *(int *)constant_value->data;
                aggr_results_[i] = new FloatValue(float(value));
              } else {
                float value = *(float *)constant_value->data;
                aggr_results_[i] = new FloatValue(value);
              }
              break;
            case AggrType::MAX:
            case AggrType::MIN:
              if(constant_value->type == AttrType::INTS) {
                int value = *(int *)constant_value->data;
                aggr_results_[i] = new IntValue(value);
              } else if (constant_value->type == AttrType::FLOATS){
                float value = *(float *)constant_value->data;
                aggr_results_[i] = new FloatValue(value);
              } else {
                char* value = (char*)constant_value->data;
                aggr_results_[i] = new StringValue(value);
              }
              break;
          }
          continue;
        }
        if (aggr_field.aggr_type() == AggrType::COUNT) {
          aggr_results_[i] = new IntValue(0);
        } else if (aggr_field.aggr_type() == AggrType::AVG) {
          aggr_results_[i] = new FloatValue(0);
        } else if (aggr_field.aggr_type() == AggrType::MIN) {
          aggr_results_[i] = new NullValue();
        } else if (aggr_field.aggr_type() == AggrType::MAX) {
          aggr_results_[i] = new NullValue();
        }
      }
}

void AggregationRecordConverter::read_record(const char *record) {
  
  const TupleSchema &schema = tuple_set_.schema();
  const std::vector<AggrField> & aggr_fields = schema.aggr_fields();
  const TableMeta &table_meta = table_->table_meta();
  int tuple_loc = 0;
  for (int i=0; i<aggr_fields.size(); i++) {
    const AggrField &aggr_field = aggr_fields[i];
    const Value* contant_value = aggr_field.constant_value();
    if (contant_value != nullptr) {
      switch (aggr_field.aggr_type()) {
        case AggrType::COUNT: {
          line_counts_[i] += 1;
          IntValue* result = dynamic_cast<IntValue*>(aggr_results_[i]);
          result->add(1);
          break;
        }
        default:
          break;
      }
      continue;
    }
    if (aggr_field.aggr_type() == AggrType::COUNT){
      if (0 == strcmp("*", aggr_field.field_name())) {
        // *的情况必为COUNT， 其他类型已经在上层做过滤
        line_counts_[i] += 1;
        IntValue* result = dynamic_cast<IntValue*>(aggr_results_[i]);
        result->add(1);
        continue;
      }
    }
    const FieldMeta *field_meta = table_meta.field(aggr_field.field_name());
    assert(field_meta != nullptr);
    int8_t is_null = *(int8_t*)(record + field_meta->offset() + field_meta->len() - 1);
    if (is_null) continue;
    switch(aggr_field.aggr_type()) {
      case AggrType::COUNT: {
        IntValue* result = dynamic_cast<IntValue*>(aggr_results_[i]);
        line_counts_[i] += 1;
        result->add(1);
        break;
      }
      case AggrType::AVG: {
        FloatValue* result = dynamic_cast<FloatValue*>(aggr_results_[i]);
        switch (field_meta->type()) {
          case AttrType::INTS: {
            int value = *(int*)(record + field_meta->offset());
            result->add(float(value));
            line_counts_[i] += 1;
            break;
          }
          case AttrType::FLOATS: {
            float value = *(float*)(record + field_meta->offset());
            result->add(value);
            line_counts_[i] += 1;
            break;
          }
          default:
            LOG_PANIC("AggregationRecordConverter::read_record Unsupported field type. type=%d", field_meta->type());
        }
        break;
      }
      case AggrType::MAX: {
        switch (field_meta->type()) {
          case AttrType::INTS: {
            int value = *(int*)(record + field_meta->offset());
            IntValue* result = dynamic_cast<IntValue*>(aggr_results_[i]);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new IntValue(value);
            } else if (!result->bigger_than(value)) {
              result->replace(value);
            }
            break;
          }
          case AttrType::FLOATS: {
            float value = *(float*)(record + field_meta->offset());
            FloatValue* result = dynamic_cast<FloatValue*>(aggr_results_[i]);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new FloatValue(value);
            } else if (!result->bigger_than(value)) {
              result->replace(value);
            }
            break;
          }
          case AttrType::DATES: {
            int value = *(int*)(record + field_meta->offset());
            DateValue* result = dynamic_cast<DateValue*>(aggr_results_[i]);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new DateValue(value);
            } else if (!result->bigger_than(value)) {
              result->replace(value);
            }
            break;
          }
          case AttrType::CHARS: {
            char* value = (char*)(record + field_meta->offset());
            StringValue* result = dynamic_cast<StringValue*>(aggr_results_[i]);
            StringValue value_in(value);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new StringValue(value, strlen(value));
            } else if (result->compare(value_in) < 0) {
              result->replace(value_in.value());
            }
            break;
          }
          default:
            LOG_PANIC("AggregationRecordConverter::read_record Unsupported field type. type=%d", field_meta->type());
        }
        break;
      }
      case AggrType::MIN: {
        switch (field_meta->type()) {
          case AttrType::INTS: {
            int value = *(int*)(record + field_meta->offset());
            IntValue* result = dynamic_cast<IntValue*>(aggr_results_[i]);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new IntValue(value);
            } else if (result->bigger_than(value)) {
              result->replace(value);
            }
            break;
          }
          case AttrType::FLOATS: {
            float value = *(float*)(record + field_meta->offset());
            FloatValue* result = dynamic_cast<FloatValue*>(aggr_results_[i]);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new FloatValue(value);
            } else if (result->bigger_than(value)) {
              result->replace(value);
            }
            break;
          }
          case AttrType::DATES: {
            int value = *(int*)(record + field_meta->offset());
            DateValue* result = dynamic_cast<DateValue*>(aggr_results_[i]);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new DateValue(value);
            } else if (result->bigger_than(value)) {
              result->replace(value);
            }
            break;
          }
          case AttrType::CHARS: {
            char* value = (char*)(record + field_meta->offset());
            StringValue* result = dynamic_cast<StringValue*>(aggr_results_[i]);
            StringValue value_in(value);
            line_counts_[i] += 1;
            if (aggr_results_[i]->type() == ValueType::NULLTYPE) {
              delete aggr_results_[i];
              aggr_results_[i] = new StringValue(value, strlen(value));
            } else if (result->compare(value_in) > 0) {
              result->replace(value_in.value());
            }
            break;
          }
          default:
            LOG_PANIC("AggregationRecordConverter::read_record Unsupported field type. type=%d", field_meta->type());
        }
        break;
      }
      default:
        LOG_DEBUG("Not Support AGGR_UNDEFINED aggregation. ");
    }
  }
}

RC AggregationRecordConverter::final_add_record() {
  RC rc = RC::SUCCESS;
  const TupleSchema &schema = tuple_set_.schema();
  const std::vector<AggrField> & aggr_fields = schema.aggr_fields();
  Tuple tuple;
  for (int i=0; i<aggr_fields.size(); i++) {
    tuple.add(aggr_results_[i]);
    // const AggrField &aggr_field = aggr_fields[i];
    // const Value* constant_value = aggr_field.constant_value();
    // if (constant_value != nullptr) {
    //   switch(aggr_field.aggr_type()) {
    //     case AggrType::COUNT: {
    //       IntValue* value_tuple = dynamic_cast<IntValue*>(aggr_results_[i]);
    //       tuple.add(value_tuple->value());
    //       break;
    //     }
    //     case AggrType::AVG: {
    //       FloatValue* value_tuple = dynamic_cast<FloatValue*>(aggr_results_[i]);
    //       tuple.add(value_tuple->value());
    //       break;
    //     }
    //     case AggrType::MAX:
    //     case AggrType::MIN: {
    //       if (constant_value->type == AttrType::INTS) {
    //         IntValue* value_tuple = dynamic_cast<IntValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value());
    //         break;
    //       } else if(constant_value->type == AttrType::FLOATS) {
    //         FloatValue* value_tuple = dynamic_cast<FloatValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value());
    //         break;
    //       } else {
    //         StringValue* value_tuple = dynamic_cast<StringValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value().c_str(), value_tuple->value().size());
    //       }
    //     }
    //   }
    //   continue;
    // }
    // switch(aggr_field.aggr_type()) {
    //   case AggrType::COUNT: {
    //     IntValue* value_tuple = dynamic_cast<IntValue*>(aggr_results_[i]);
    //     tuple.add(value_tuple->value());
    //     break;
    //   }
    //   case AggrType::AVG: {
    //     FloatValue* value_tuple = dynamic_cast<FloatValue*>(aggr_results_[i]);
    //     tuple.add(value_tuple->value() / line_counts_[i]);
    //     break;
    //   }
    //   case AggrType::MAX:
    //   case AggrType::MIN: {
    //     switch (aggr_field.type()) {
    //       case AttrType::INTS: {
    //         IntValue* value_tuple = dynamic_cast<IntValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value(), AttrType::INTS);
    //         break;
    //       }
    //       case AttrType::FLOATS: {
    //         FloatValue* value_tuple = dynamic_cast<FloatValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value());
    //         break;
    //       }
    //       case AttrType::DATES: {
    //         DateValue* value_tuple = dynamic_cast<DateValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value(), AttrType::DATES);
    //         break;
    //       }
    //       case AttrType::CHARS: {
    //         StringValue* value_tuple = dynamic_cast<StringValue*>(aggr_results_[i]);
    //         tuple.add(value_tuple->value().c_str(), value_tuple->value().size());
    //         break;
    //       }
    //     }
    //     break;
    //   }
    //   default:
    //     LOG_DEBUG("Not Support AGGR_UNDEFINED aggregation. ");
    //     rc = RC::GENERIC_ERROR;
    //     return rc;
    // }
  }
  tuple_set_.add(std::move(tuple));
  return rc;
}