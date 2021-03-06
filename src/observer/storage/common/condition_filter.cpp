/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its
affiliates. All rights reserved. miniob is licensed under Mulan PSL v2. You can
use this software according to the terms and conditions of the Mulan PSL v2. You
may obtain a copy of Mulan PSL v2 at: http://license.coscl.org.cn/MulanPSL2 THIS
SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/7.
//

#include "condition_filter.h"

#include <stddef.h>

#include "common/log/log.h"
#include "record_manager.h"
#include "storage/common/table.h"

using namespace common;

ConditionFilter::~ConditionFilter() {}

DefaultConditionFilter::DefaultConditionFilter() {
  left_.is_attr = false;
  left_.attr_length = 0;
  left_.attr_offset = 0;
  left_.value = nullptr;

  right_.is_attr = false;
  right_.attr_length = 0;
  right_.attr_offset = 0;
  right_.value = nullptr;
}
DefaultConditionFilter::~DefaultConditionFilter() {}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right,
                                AttrType left_attr_type,
                                AttrType right_attr_type, CompOp comp_op,
                                ExpressionNode *left_expression,
                                ExpressionNode *right_expression) {
//  如果左边不是一个表达式则校验
  if (!left_expression->isExpression) {
    if (left_attr_type < CHARS || left_attr_type > TEXTS) {
      LOG_ERROR("Invalid condition with unsupported left attribute type: %d",
                left_attr_type);
      return RC::INVALID_ARGUMENT;
    }
  }
// 如果右边不是一个表达式则校验
  if (!right_expression->isExpression) {
    if (right_attr_type < CHARS || right_attr_type > TEXTS) {
      LOG_ERROR("Invalid condition with unsupported right attribute type: %d",
                right_attr_type);
      return RC::INVALID_ARGUMENT;
    }
  }

  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d",
              comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  left_attr_type_ = left_attr_type;
  right_attr_type_ = right_attr_type;
  comp_op_ = comp_op;
  left_expression_ = left_expression;
  right_expression_ = right_expression;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, const Condition &condition) {
  const TableMeta &table_meta = table.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  if (!condition.left_expression->isExpression) {
    if (1 == condition.left_is_attr) {
      left.is_attr = true;
      const FieldMeta *field_left =
          table_meta.field(condition.left_attr.attribute_name);
      if (nullptr == field_left) {
        LOG_WARN("No such field in condition. %s.%s", table.name(),
                 condition.left_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      left.attr_length = field_left->len();
      left.attr_offset = field_left->offset();

      left.value = nullptr;

      type_left = field_left->type();
    } else {
      left.is_attr = false;
      left.value = condition.left_value.data;  // 校验type 或者转换类型
      type_left = condition.left_value.type;

      // zt 新增 DATES 校验
      if (type_left == DATES && *(int *)(left.value) == INT32_MIN) {
        LOG_DEBUG("WHERE LEFT DATE TYPE INVAILD!");
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }

      left.attr_length = 0;
      left.attr_offset = 0;
    }
  }
  if (!condition.right_expression->isExpression) {
    if (1 == condition.right_is_attr) {
      right.is_attr = true;
      const FieldMeta *field_right =
          table_meta.field(condition.right_attr.attribute_name);
      if (nullptr == field_right) {
        LOG_WARN("No such field in condition. %s.%s", table.name(),
                 condition.right_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }

      right.attr_length = field_right->len();
      right.attr_offset = field_right->offset();
      type_right = field_right->type();
      right.value = nullptr;
    } else {
      // zt 新增 DATES 校验

      right.is_attr = false;
      right.value = condition.right_value.data;
      type_right = condition.right_value.type;

      // zt 新增 DATES 校验
      if (type_right == DATES && *(int *)(right.value) == INT32_MIN) {
        LOG_DEBUG("WHERE RIGHT DATE TYPE INVAILD!");
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }

      right.attr_length = 0;
      right.attr_offset = 0;
    }
  }
  // 校验和转换
  //  if (!field_type_compare_compatible_table[type_left][type_right]) {
  //    // 不能比较的两个字段， 要把信息传给客户端
  //    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  //  }
  // NOTE：这里没有实现不同类型的数据比较，比如整数跟浮点数之间的对比
  // 但是选手们还是要实现。这个功能在预选赛中会出现
  if (!condition.left_expression->isExpression &&
      !condition.right_expression->isExpression) {
    if (type_left != type_right) {
      if (type_left != NULLFIELD && type_right != NULLFIELD) {
        if (!((type_left == AttrType::INTS && type_right == AttrType::FLOATS) ||
              (type_left == AttrType::FLOATS &&
               type_right == AttrType::INTS))) {
          LOG_DEBUG("Type left not the same as type right");
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
    }
  }

  return init(left, right, type_left, type_right, condition.comp,
              condition.left_expression, condition.right_expression);
}

bool DefaultConditionFilter::filter(const Record &rec) const {
  char *left_value = nullptr;
  char *right_value = nullptr;
    if (left_.is_attr) {  // value
      int8_t is_null =
          *(int8_t *)(rec.data + left_.attr_offset + left_.attr_length - 1);
      if (is_null) {
        left_value = nullptr;
      } else {
        left_value = (char *)(rec.data + left_.attr_offset);
      }
    } else {
      left_value = (char *)left_.value;
    }
  

    if (right_.is_attr) {
      int8_t is_null =
          *(int8_t *)(rec.data + right_.attr_offset + right_.attr_length - 1);
      if (is_null) {
        right_value = nullptr;
      } else {
        right_value = (char *)(rec.data + right_.attr_offset);
      }
    } else {
      right_value = (char *)right_.value;
    }
  

  double cmp_result = 0;
  switch (left_attr_type_) {
    case CHARS: {  // 字符串都是定长的，直接比较
      // 按照C字符串风格来定
      if (nullptr == left_value || nullptr == right_value ) {
        break;
      }
      cmp_result = strcmp(left_value, right_value);
    } break;
    // DATES 底层于INTS采取相同存储方式
    case DATES:
    case INTS: {
      // 没有考虑大小端问题
      // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
      if (nullptr == left_value || nullptr == right_value ) {
        break;
      }
      double left = *(int *)left_value;
      double right;
      if (right_attr_type_ == AttrType::INTS || right_attr_type_ == AttrType::DATES) {
        right = *(int *)right_value;
      } else {
        right = *(float *)right_value;
      }
      cmp_result = left - right;
    } break;
    case FLOATS: {
      if (nullptr == left_value || nullptr == right_value ) {
        break;
      }
      double left = *(float *)left_value;
      double right;
      if (right_attr_type_ == AttrType::FLOATS) {
        right = *(float *)right_value;
      } else {
        right = *(int *)right_value;
      }
      cmp_result = left - right;
    } break;
      break;
    default: {
    }
  }

  switch (comp_op_) {
    case EQUAL_TO:
      if (nullptr == left_value || nullptr == right_value ) {
        return false;
      }
      return 0 == cmp_result;
    case LESS_EQUAL:
      if (nullptr == left_value || nullptr == right_value ) {
        return false;
      }
      return cmp_result <= 0;
    case NOT_EQUAL:
      if (nullptr == left_value || nullptr == right_value ) {
        return false;
      }
      return cmp_result != 0;
    case LESS_THAN:
      if (nullptr == left_value || nullptr == right_value ) {
        return false;
      }
      return cmp_result < 0;
    case GREAT_EQUAL:
      if (nullptr == left_value || nullptr == right_value ) {
        return false;
      }
      return cmp_result >= 0;
    case GREAT_THAN:
      if (nullptr == left_value || nullptr == right_value ) {
        return false;
      }
      return cmp_result > 0;
    case IS:
      if (nullptr == left_value && nullptr == right_value ) {
        return true;
      } else {
        return false;
      }
      
    case ISNOT:
      if ((nullptr != left_value && nullptr == right_value) ||  (nullptr == left_value && nullptr != right_value)) {
        return true;
      } else {
        return false;
      }
    default:
      break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result;  // should not go here
}

CompositeConditionFilter::~CompositeConditionFilter()
{
  if (memory_owner_) {
    delete[] filters_;
    filters_ = nullptr;
  }
}

RC CompositeConditionFilter::init(const ConditionFilter *filters[],
                                  int filter_num, bool own_memory) {
  filters_ = filters;
  filter_num_ = filter_num;
  memory_owner_ = own_memory;
  return RC::SUCCESS;
}
RC CompositeConditionFilter::init(const ConditionFilter *filters[],
                                  int filter_num) {
  return init(filters, filter_num, false);
}

RC CompositeConditionFilter::init(Table &table, const Condition *conditions,
                                  int condition_num) {
  if (condition_num == 0) {
    return RC::SUCCESS;
  }

  if (conditions == nullptr) {
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;
  ConditionFilter **condition_filters = new ConditionFilter *[condition_num];
  for (int i = 0; i < condition_num; i++) {
    DefaultConditionFilter *default_condition_filter =
        new DefaultConditionFilter();
    rc = default_condition_filter->init(table, conditions[i]);
    if (rc != RC::SUCCESS) {
      delete default_condition_filter;
      for (int j = i - 1; j >= 0; j--) {
        delete condition_filters[j];
        condition_filters[j] = nullptr;
      }
      delete[] condition_filters;
      condition_filters = nullptr;
      return rc;
    }
    condition_filters[i] = default_condition_filter;
  }
  return init((const ConditionFilter **)condition_filters, condition_num, true);
}

bool CompositeConditionFilter::filter(const Record &rec) const {
  for (int i = 0; i < filter_num_; i++) {
    if (!filters_[i]->filter(rec)) {
      return false;
    }
  }
  return true;
}
