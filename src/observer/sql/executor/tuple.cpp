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

void TupleSchema::add_aggr(AggrType aggr_type, AttrType type, const char *table_name, const char *field_name) {
  aggr_fields_.emplace_back(aggr_type, type, table_name, field_name);
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

void TupleSchema::print(std::ostream &os) const {
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
      if (table_names.size() > 1) {
        os << iter->table_name() << ".";
      }
      std::string aggr_all_field_name;
      switch(iter->aggr_type()) {
        case AggrType::COUNT: {
          aggr_all_field_name = "COUNT(" + std::string(iter->field_name()) + ")";
          break;
        }
        case AggrType::AVG: {
          aggr_all_field_name = "AVG(" + std::string(iter->field_name()) + ")";
          break;
        }
        case AggrType::MIN: {
          aggr_all_field_name = "MIN(" + std::string(iter->field_name()) + ")";
          break;
        }
        case AggrType::MAX: {
          aggr_all_field_name = "MAX(" + std::string(iter->field_name()) + ")";
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
    switch(aggr_fields_.back().aggr_type()) {
      case AggrType::COUNT: {
        aggr_all_field_name = "COUNT(" + std::string(aggr_fields_.back().field_name()) + ")";
        break;
      }
      case AggrType::AVG: {
        aggr_all_field_name = "AVG(" + std::string(aggr_fields_.back().field_name()) + ")";
        break;
      }
      case AggrType::MIN: {
        aggr_all_field_name = "MIN(" + std::string(aggr_fields_.back().field_name()) + ")";
        break;
      }
      case AggrType::MAX: {
        aggr_all_field_name = "MAX(" + std::string(aggr_fields_.back().field_name()) + ")";
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
      if (table_names.size() > 1) {
        os << iter->table_name() << ".";
      }
      os << iter->field_name() << " | ";
    }
    if (table_names.size() > 1) {
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

void TupleSet::add_aggr_tuple(Tuple &&tuple) {
  aggr_tuples_.emplace_back(std::move(tuple));
}

void TupleSet::replace_aggr_tuple(int i, Tuple &&tuple) {
  aggr_tuples_.emplace(aggr_tuples_.begin() + i, tuple);
}

void TupleSet::clear() {
  tuples_.clear();
  schema_.clear();
}

void TupleSet::print(std::ostream &os) const {
  if (schema_.fields().empty() && schema_.aggr_fields().empty()) {
    LOG_WARN("Got empty schema");
    return;
  }

  schema_.print(os);

  if (aggr_tuples_.size() > 0) {
    for (const Tuple &item : aggr_tuples_) {
      const std::vector<std::shared_ptr<TupleValue>> &values = item.values();
      for (std::vector<std::shared_ptr<TupleValue>>::const_iterator iter = values.begin(), end = --values.end();
            iter != end; ++iter) {
        (*iter)->to_string(os);
        os << " | ";
      }
      values.back()->to_string(os);
      os << std::endl;
    }
  } else {
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
}

void TupleSet::set_schema(const TupleSchema &schema) {
  schema_ = schema;
}

const TupleSchema &TupleSet::get_schema() const {
  return schema_;
}

bool TupleSet::is_empty() const {
  return tuples_.empty() && aggr_tuples_.empty();
}

int TupleSet::size() const {
  if (aggr_tuples_.size() > 0) {
    return aggr_tuples_.size();
  } else {
    return tuples_.size();
  }
}

const Tuple &TupleSet::get(int index) const {
  return tuples_[index];
}

const Tuple &TupleSet::get_aggr(int index) const {
  return aggr_tuples_[index];
}

const std::vector<Tuple> &TupleSet::tuples() const {
  return tuples_;
}

const std::vector<Tuple> &TupleSet::aggr_tuples() const {
  return aggr_tuples_;
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
        int value = *(int*)(record + field_meta->offset());
        tuple.add(value,INTS);
      }
      break;
      case FLOATS: {
        float value = *(float *)(record + field_meta->offset());
        tuple.add(value);
      }
        break;
      case CHARS: {
        const char *s = record + field_meta->offset();  // 现在当做Cstring来处理
        tuple.add(s, strlen(s));
      }
      break;
      case DATES:
      {
        //   LOG_ERROR("DATES type test-1!");
          int value = *(int *)(record + field_meta->offset()); // 现在当做Cstring来处理
        //   LOG_ERROR("DATES type test-2!");
          tuple.add(value,DATES);
        //   LOG_ERROR("DATES type test-3!");
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
        if (aggr_field.aggr_type() == AggrType::COUNT) {
          aggr_results_[i] = 0;
        } else if (aggr_field.aggr_type() == AggrType::AVG) {
          aggr_results_[i] = 0;
        } else if (aggr_field.aggr_type() == AggrType::MIN) {
          aggr_results_[i] = DBL_MAX_EXP;
        } else if (aggr_field.aggr_type() == AggrType::MAX) {
          aggr_results_[i] = DBL_MIN_EXP;
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
    if (aggr_field.aggr_type() == AggrType::COUNT){
      if (0 == strcmp("*", aggr_field.field_name())) {
        // *的情况必为COUNT， 其他类型已经在上层做过滤
        aggr_results_[i] += 1;
        line_counts_[i] += 1;
        continue;
      } else {
        // 后期才会出现某一列值为NULL的情况，现暂不处理COUNT中的这种情况
        aggr_results_[i] += 1;
        line_counts_[i] += 1;
        continue;
      }
    }

    const FieldMeta *field_meta = table_meta.field(aggr_field.field_name());
    assert(field_meta != nullptr);
    switch (field_meta->type()) {
      case INTS: {
        int value = *(int*)(record + field_meta->offset());
        switch (aggr_field.aggr_type())
        {
        case AggrType::AVG:
          aggr_results_[i] += (double)value;
          break;
        case AggrType::MAX:
          aggr_results_[i] = value > aggr_results_[i] ? (double)value : aggr_results_[i];
          break;
        case AggrType::MIN:
          aggr_results_[i] = value < aggr_results_[i] ? (double)value : aggr_results_[i];
        default:
          LOG_DEBUG("Not Support AGGR_UNDEFINED aggregation. ");
          break;
        }
        line_counts_[i] += 1;
      }
      break;
      case FLOATS: {
        float value = *(float *)(record + field_meta->offset());
        switch (aggr_field.aggr_type())
        {
        case AggrType::AVG:
          aggr_results_[i] += (double)value;
          break;
        case AggrType::MAX:
          aggr_results_[i] = value > aggr_results_[i] ? (double)value : aggr_results_[i];
          break;
        case AggrType::MIN:
          aggr_results_[i] = value < aggr_results_[i] ? (double)value : aggr_results_[i];
        default:
          LOG_DEBUG("Not Support AGGR_UNDEFINED aggregation. ");
          break;
        }
        line_counts_[i] += 1;
      }
      break;
      case CHARS: {
        LOG_DEBUG("AggregationRecordConverter::add_record Aggregation Not Support CHARS.");
        return;
      }
      break;
      default: {
        LOG_PANIC("Unsupported field type. type=%d", field_meta->type());
      }
    }
  }
}

RC AggregationRecordConverter::final_add_record() {
  RC rc = RC::SUCCESS;
  const TupleSchema &schema = tuple_set_.schema();
  const std::vector<AggrField> & aggr_fields = schema.aggr_fields();
  Tuple tuple;
  for (int i=0; i<aggr_fields.size(); i++) {
    const AggrField &aggr_field = aggr_fields[i];
    switch(aggr_field.aggr_type()) {
      case AggrType::COUNT:
        tuple.add(int(aggr_results_[i]));
        break;
      case AggrType::AVG:
        tuple.add(float(aggr_results_[i] / line_counts_[i]));
        break;
      case AggrType::MAX:
      case AggrType::MIN:
        tuple.add(float(aggr_results_[i]));
        break;
      default:
        LOG_DEBUG("Not Support AGGR_UNDEFINED aggregation. ");
        rc = RC::GENERIC_ERROR;
        return rc;
    }
  }
  tuple_set_.add(std::move(tuple));
  return rc;
}