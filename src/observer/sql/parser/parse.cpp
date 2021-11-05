/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its
affiliates. All rights reserved. miniob is licensed under Mulan PSL v2. You can
use this software according to the terms and conditions of the Mulan PSL v2. You
may obtain a copy of Mulan PSL v2 at: http://license.coscl.org.cn/MulanPSL2 THIS
SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Longda on 2021/4/13.
//

#include "sql/parser/parse.h"

#include <mutex>

#include "common/log/log.h"
#include "rc.h"

RC parse(char *st, Query *sqln);

#ifdef __cplusplus
extern "C" {
#endif  // __cplusplus


void expression_node_init(ExpressionNode *node, int isExpression, ExpressionNode *left_expression, OpType op, ExpressionNode *right_expression, int isValue, RelAttr *relation_attr, Value* constant_value) {
    node->isExpression =isExpression;
    node->left_expression = left_expression;
    node->right_expression = right_expression;
    node->op = op;
    
    node->isValue = isValue;
    node->constant_value = constant_value;
    node->relation_attr = relation_attr;
}

void expression_node_destory(ExpressionNode *node){
    if(node != nullptr ) {
        
        if(node->left_expression!=nullptr) {
            expression_node_destory(node->left_expression);
        }
        if(node->right_expression!=nullptr) {
            expression_node_destory(node->right_expression);
        }

        if(node->relation_attr!=nullptr) {
            relation_attr_destroy(node->relation_attr);
        }

        if(node->constant_value!=nullptr) {
            value_destroy(node->constant_value);
        }
    }
}

void aggr_attr_init(AggrAttr *aggr_attr, AggrType aggr_op,
                    const char *relation_name, const char *attribute_name) {
  aggr_attr->aggr_type = aggr_op;
  if (relation_name != nullptr) {
    aggr_attr->relation_name = strdup(relation_name);
  } else {
    aggr_attr->relation_name = nullptr;
  }
  if (attribute_name != nullptr) {
    aggr_attr->attribute_name = strdup(attribute_name);
  } else {
    aggr_attr->attribute_name = nullptr;
  }
}

void aggr_attr_destory(AggrAttr *aggr_attr) {
  free(aggr_attr->relation_name);
  free(aggr_attr->attribute_name);
  if (aggr_attr->is_constant) {
    value_destroy(&aggr_attr->constant_value);
  }
  aggr_attr->relation_name = nullptr;
  aggr_attr->attribute_name = nullptr;
}

void group_attr_init(GroupAttr *group_attr, const char *relation_name,const char* attribute_name){
    if(relation_name != nullptr) {
        group_attr->relation_name = strdup(relation_name);
    }else {
        group_attr->relation_name = nullptr;
    }
    
    if(attribute_name!=nullptr) {
        group_attr->attribute_name = strdup(attribute_name);
    }else {
        group_attr->attribute_name = nullptr;
    }
}

void order_attr_init(OrderAttr *order_attr,OrderType order_type,const char *relation_name,const char* attribute_name){
    order_attr->order_type = order_type;
    if(relation_name != nullptr) {
        order_attr->relation_name = strdup(relation_name);
    }else {
        order_attr->relation_name = nullptr;
    }
    
    if(attribute_name!=nullptr) {
        order_attr->attribute_name = strdup(attribute_name);
    }else {
        order_attr->attribute_name = nullptr;
    }
}

void group_attr_destory(GroupAttr *group_attr){
    if (group_attr->relation_name != nullptr) {
      free(group_attr->relation_name);
    }
    if (group_attr->attribute_name != nullptr) {
      free(group_attr->attribute_name);
    }
    group_attr->relation_name = nullptr;
    group_attr->attribute_name = nullptr;
}

void order_attr_destory(OrderAttr *order_attr){
    free(order_attr->relation_name);
    free(order_attr->attribute_name);
    order_attr->relation_name = nullptr;
    order_attr->attribute_name = nullptr;
}

void relation_attr_init(RelAttr *relation_attr, const char *relation_name,
                        const char *attribute_name, ExpressionNode *node) {
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  relation_attr->attribute_name = strdup(attribute_name);
  relation_attr->node = node;
}

void relation_attr_destroy(RelAttr *relation_attr) {
  if(relation_attr->attribute_name) {
    free(relation_attr->attribute_name);
  }

  if(relation_attr->relation_name) {    
    free(relation_attr->relation_name);
  }
  relation_attr->relation_name = nullptr;
  relation_attr->attribute_name = nullptr;

  if(relation_attr->node) {
      expression_node_destory(relation_attr->node);
  }
}
// insert into tt values('2038-1-19');
// SELECT *FROM tt WHERE u_date > '2020-1-20';
// SELECT *FROM tt WHERE u_date < '2019-12-31';
// SELECT *FROM tt WHERE u_date = '2020-1-1';

// 新增 record 初始化
void record_init(Insert_Record *record, Value *value, int value_length) {
  LOG_DEBUG("初始化一个记录");
  record->value_num = value_length;
  for (int i = 0; i < value_length; i++) {
    record->values[i].type = value[i].type;
    int len = 0;
    switch (value[i].type) {
      case CHARS:
        len = strlen((char *)value[i].data);
        record->values[i].data = malloc(len + 1);
        LOG_DEBUG("存储的CHARS: %s length:%d ", (char *)value[i].data, len);
        memcpy(record->values[i].data, value[i].data, len + 1);
        break;
      case INTS:
      case FLOATS:
      case DATES:
        // 当前默认都是四字节
        record->values[i].data = malloc(sizeof(int));
        memcpy(record->values[i].data, value[i].data, sizeof(int));
        break;
      case NULLFIELD:
        record->values[i].data = nullptr;
      default:
        break;
    }
    if (record->values[i].type == NULLFIELD) {
      LOG_DEBUG("value-type: nullfield data:nodata");
    } else {
      LOG_DEBUG("value-type: %d data: %d", value[i].type,
                *(int *)(value[i].data));
    }
  }
}

void value_init_integer(Value *value, int v) {
  value->type = INTS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_init_float(Value *value, float v) {
  value->type = FLOATS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
}
void value_init_string(Value *value, const char *v) {
  value->type = CHARS;
  value->data = strdup(v);
}
#include <assert.h>
#include <stdio.h>
#include <string.h>

bool DateVerify(int year, int month, int day) {
  LOG_DEBUG("VERIFY YEAR: %d, MONTH: %d, DAY:%d ", year, month, day);
  if (year < 1970 || year > 2038 || month < 1 || month > 12 || day < 1 ||
      day > 31) {
    return false;
  }

  switch (month) {
    case 4:
    case 6:
    case 9:
    case 11:
      if (day > 30) {  // 4.6.9.11月天数不能大于30
        return false;
      }
      break;
    case 2: {
      bool bLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
      if ((bLeapYear && day > 29) || (!bLeapYear && day > 28)) {
        // 闰年2月不能大于29天;平年2月不能大于28天
        return false;
      }
    } break;
    default:
      break;
  }

  return true;
}

// 增加时间转化函数
int str_to_time(const char *time_str) {
  struct tm tm_time;
  memset(&tm_time, 0, sizeof(struct tm));
  char *temp_time = strdup(time_str);

  // unix time <0 表示 1970年以前 表示时间戳生成错误 ==> 时间字符串错误
  // 先校验是合法字符串再调UNIX时间戳生成

  int str_len = strlen(temp_time);
  int count = 0;
  int time_numbers[3];
  char *head = temp_time;
  for (int i = 0; i <= str_len; i++) {
    if (temp_time[i] == '-' || temp_time[i] == '\0') {
      temp_time[i] = '\0';
      LOG_DEBUG("STRING: %s", head);
      time_numbers[count] = atoi(head);
      LOG_DEBUG("NUMBER: %d", atoi(head));
      head = &(temp_time[i + 1]);
      count++;
    }
  }

  free(temp_time);

  bool result = DateVerify(time_numbers[0], time_numbers[1], time_numbers[2]);
  if (result) {
    strptime(time_str, "%Y-%m-%d", &tm_time);
    int unixtime = mktime(&tm_time);
    return unixtime;
  } else
    return INT32_MIN;
}

// 将字符串转化为int存储
void value_init_date(Value *value, const char *v) {
  value->type = DATES;
  int time = str_to_time(v);
  value->data = malloc(sizeof(int));

  // printf("Store time: %ud\n",time);
  memcpy(value->data, &time, sizeof(int));
}

void value_init_null(Value *value) {
  value->type = NULLFIELD;
  value->data = nullptr;
}

void value_init_text(Value *value, const char *v) {
  value->type = TEXTS;
  value->data = value->data = strdup(v);
}

void value_destroy(Value *value) {
  value->type = UNDEFINED;
  if (value->type != AttrType::NULLFIELD) {
    free(value->data);
  }
  value->data = nullptr;
}

void condition_init(Condition *condition, CompOp comp, int left_is_attr,
                    RelAttr *left_attr, Value *left_value, int right_is_attr,
                    RelAttr *right_attr, Value *right_value,ExpressionNode *left,ExpressionNode *right) {
  condition->comp = comp;
  condition->left_is_attr = left_is_attr;
  if (left_is_attr) {
    condition->left_attr = *left_attr;
  } else {
    condition->left_value = *left_value;
  }

  condition->right_is_attr = right_is_attr;
  if (right_is_attr) {
    condition->right_attr = *right_attr;
  } else {
    condition->right_value = *right_value;
  }

  condition->left = left;
  condition->right = right;
}

void condition_destroy(Condition *condition) {
  if (condition->left_is_attr) {
    relation_attr_destroy(&condition->left_attr);
  } else {
    value_destroy(&condition->left_value);
  }
  if (condition->right_is_attr) {
    relation_attr_destroy(&condition->right_attr);
  } else {
    value_destroy(&condition->right_value);
  }

//   zt释放表达式树
  if(condition->left) {
      expression_node_destory(condition->left);
  }

  if(condition->right) {
       expression_node_destory(condition->right);
  }
}

// zt 长度 +1
void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type,
                    size_t length, int nullable = 0) {
  attr_info->name = strdup(name);
  attr_info->type = type;
  attr_info->length = length + 1;
  attr_info->nullable = nullable;
}
void attr_info_destroy(AttrInfo *attr_info) {
  free(attr_info->name);
  attr_info->name = nullptr;
}

void selects_init(Selects *selects, ...);
void selects_append_attribute(Selects *selects, RelAttr *rel_attr) {
  selects->attributes[selects->attr_num++] = *rel_attr;
}

void selects_append_aggr_attribute(Selects *selects, AggrAttr *aggr_attr) {
  selects->aggr_attr[selects->aggr_num++] = *aggr_attr;
}

void selects_append_order_attribute(Selects *selects, OrderAttr *order_attr) {
  selects->order_attr[selects->order_num++] = *order_attr;
}

void selects_append_group_attribute(Selects *selects, GroupAttr *group_attr) {
  selects->group_attr[selects->group_num++] = *group_attr;
}


void selects_append_relation(Selects *selects, const char *relation_name) {
  selects->relations[selects->relation_num++] = strdup(relation_name);
}

void selects_append_conditions(Selects *selects, Condition conditions[],
                               size_t condition_num) {
  assert(condition_num <=
         sizeof(selects->conditions) / sizeof(selects->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->conditions[i] = conditions[i];
  }
  selects->condition_num = condition_num;
}

void selects_destroy(Selects *selects) {
  for (size_t i = 0; i < selects->attr_num; i++) {
    relation_attr_destroy(&selects->attributes[i]);
  }
  selects->attr_num = 0;

  for (size_t i = 0; i < selects->relation_num; i++) {
    free(selects->relations[i]);
    selects->relations[i] = NULL;
  }
  selects->relation_num = 0;

  for (size_t i = 0; i < selects->aggr_num; i++) {
    aggr_attr_destory(&selects->aggr_attr[i]);
  }

  selects->aggr_num = 0;

  for (size_t i = 0; i < selects->condition_num; i++) {
    condition_destroy(&selects->conditions[i]);
  }
  selects->condition_num = 0;
  
  for (size_t i = 0; i < selects->order_num; i++)
  {
      order_attr_destory(&selects->order_attr[i]);
  }
  selects->order_num = 0;

  for (size_t i = 0; i < selects->group_num; i++)
  {
      group_attr_destory(&selects->group_attr[i]);
  }
  selects->order_num = 0;
}

// 修改record存储
void inserts_init(Inserts *inserts, const char *relation_name,
                  Insert_Record records[], size_t record_num) {
  // 数据的深拷贝！
  assert(record_num <= sizeof(inserts->records) / sizeof(inserts->records[0]));
  inserts->relation_name = strdup(relation_name);
  for (size_t i = 0; i < record_num; i++) {
    inserts->records[i].value_num = records[i].value_num;
    for (size_t j = 0; j < records[i].value_num; j++) {
      inserts->records[i].values[j].type = records[i].values[j].type;
      if (inserts->records[i].values[j].type == NULLFIELD) {
        inserts->records[i].values[j].data = nullptr;
      } else {
        
        // zt 如果是CHARS 
        if(inserts->records[i].values[j].type == CHARS) {
            inserts->records[i].values[j].data = malloc(strlen((char* )records[i].values[j].data) + 1);
            memcpy(inserts->records[i].values[j].data, records[i].values[j].data,
               strlen((char* )records[i].values[j].data) + 1);
        }else {
            inserts->records[i].values[j].data = malloc(sizeof(int));
            memcpy(inserts->records[i].values[j].data, records[i].values[j].data,
               sizeof(int));
        }
      }
    }
  }
  inserts->record_num = record_num;
}

// zt 增加recoed的销毁
void inserts_destroy(Inserts *inserts) {
  free(inserts->relation_name);
  inserts->relation_name = nullptr;

  for (size_t i = 0; i < inserts->record_num; i++) {
    for (size_t j = 0; j < inserts->records[i].value_num; j++) {
      value_destroy(&inserts->records[i].values[j]);
    }
    inserts->records[i].value_num = 0;
  }
  inserts->record_num = 0;
}

void deletes_init_relation(Deletes *deletes, const char *relation_name) {
  deletes->relation_name = strdup(relation_name);
}

void deletes_set_conditions(Deletes *deletes, Condition conditions[],
                            size_t condition_num) {
  assert(condition_num <=
         sizeof(deletes->conditions) / sizeof(deletes->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    deletes->conditions[i] = conditions[i];
  }
  deletes->condition_num = condition_num;
}
void deletes_destroy(Deletes *deletes) {
  for (size_t i = 0; i < deletes->condition_num; i++) {
    condition_destroy(&deletes->conditions[i]);
  }
  deletes->condition_num = 0;
  free(deletes->relation_name);
  deletes->relation_name = nullptr;
}

void updates_init(Updates *updates, const char *relation_name,
                  const char *attribute_name, Value *value,
                  Condition conditions[], size_t condition_num) {
  updates->relation_name = strdup(relation_name);
  updates->attribute_name = strdup(attribute_name);
  updates->value = *value;

  assert(condition_num <=
         sizeof(updates->conditions) / sizeof(updates->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    updates->conditions[i] = conditions[i];
  }
  updates->condition_num = condition_num;
}

void updates_destroy(Updates *updates) {
  free(updates->relation_name);
  free(updates->attribute_name);
  updates->relation_name = nullptr;
  updates->attribute_name = nullptr;

  value_destroy(&updates->value);

  for (size_t i = 0; i < updates->condition_num; i++) {
    condition_destroy(&updates->conditions[i]);
  }
  updates->condition_num = 0;
}

void create_table_append_attribute(CreateTable *create_table,
                                   AttrInfo *attr_info) {
  create_table->attributes[create_table->attribute_count++] = *attr_info;
}
void create_table_init_name(CreateTable *create_table,
                            const char *relation_name) {
  create_table->relation_name = strdup(relation_name);
}
void create_table_destroy(CreateTable *create_table) {
  for (size_t i = 0; i < create_table->attribute_count; i++) {
    attr_info_destroy(&create_table->attributes[i]);
  }
  create_table->attribute_count = 0;
  free(create_table->relation_name);
  create_table->relation_name = nullptr;
}

void drop_table_init(DropTable *drop_table, const char *relation_name) {
  drop_table->relation_name = strdup(relation_name);
}
void drop_table_destroy(DropTable *drop_table) {
  free(drop_table->relation_name);
  drop_table->relation_name = nullptr;
}

void create_index_init(CreateIndex *create_index, const char *index_name,
                       const char *relation_name) {
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
}

// zt 新增索引列名存储 支持多列索引
void create_index_attr_init(CreateIndex *create_index, const char *attr) {
  LOG_DEBUG("记录 列名");
  *(create_index->attribute_name + create_index->attribute_length) =
      strdup(attr);
  create_index->attribute_length++;
}

// 新增unique标志位实现
void create_index_unique_init(CreateIndex *create_index) {
  LOG_DEBUG("记录 UNIQUE");
  create_index->isUnique = 1;
}
// zt 修改索引结构体的内存释放
void create_index_destroy(CreateIndex *create_index) {
  free(create_index->index_name);
  free(create_index->relation_name);
  create_index->index_name = nullptr;
  create_index->relation_name = nullptr;

  for (int i = 0; i < create_index->attribute_length; i++) {
    free(create_index->attribute_name + i);
    *(create_index->attribute_name + i) = nullptr;
  }
  create_index->isUnique = 0;
  create_index->attribute_length = 0;
}

void drop_index_init(DropIndex *drop_index, const char *index_name) {
  drop_index->index_name = strdup(index_name);
}
void drop_index_destroy(DropIndex *drop_index) {
  free((char *)drop_index->index_name);
  drop_index->index_name = nullptr;
}

void desc_table_init(DescTable *desc_table, const char *relation_name) {
  desc_table->relation_name = strdup(relation_name);
}

void desc_table_destroy(DescTable *desc_table) {
  free((char *)desc_table->relation_name);
  desc_table->relation_name = nullptr;
}

void load_data_init(LoadData *load_data, const char *relation_name,
                    const char *file_name) {
  load_data->relation_name = strdup(relation_name);

  if (file_name[0] == '\'' || file_name[0] == '\"') {
    file_name++;
  }
  char *dup_file_name = strdup(file_name);
  int len = strlen(dup_file_name);
  if (dup_file_name[len - 1] == '\'' || dup_file_name[len - 1] == '\"') {
    dup_file_name[len - 1] = 0;
  }
  load_data->file_name = dup_file_name;
}

void load_data_destroy(LoadData *load_data) {
  free((char *)load_data->relation_name);
  free((char *)load_data->file_name);
  load_data->relation_name = nullptr;
  load_data->file_name = nullptr;
}

void query_init(Query *query) {
  query->flag = SCF_ERROR;
  memset(&query->sstr, 0, sizeof(query->sstr));
}

Query *query_create() {
  Query *query = (Query *)malloc(sizeof(Query));
  if (nullptr == query) {
    LOG_ERROR("Failed to alloc memroy for query. size=%ld", sizeof(Query));
    return nullptr;
  }

  query_init(query);
  return query;
}

void query_reset(Query *query) {
  switch (query->flag) {
    case SCF_SELECT: {
      selects_destroy(&query->sstr.selection);
    } break;
    case SCF_INSERT: {
      inserts_destroy(&query->sstr.insertion);
    } break;
    case SCF_DELETE: {
      deletes_destroy(&query->sstr.deletion);
    } break;
    case SCF_UPDATE: {
      updates_destroy(&query->sstr.update);
    } break;
    case SCF_CREATE_TABLE: {
      create_table_destroy(&query->sstr.create_table);
    } break;
    case SCF_DROP_TABLE: {
      drop_table_destroy(&query->sstr.drop_table);
    } break;
    case SCF_CREATE_INDEX: {
      create_index_destroy(&query->sstr.create_index);
    } break;
    case SCF_DROP_INDEX: {
      drop_index_destroy(&query->sstr.drop_index);
    } break;
    case SCF_SYNC: {
    } break;
    case SCF_SHOW_TABLES:
      break;

    case SCF_DESC_TABLE: {
      desc_table_destroy(&query->sstr.desc_table);
    } break;

    case SCF_LOAD_DATA: {
      load_data_destroy(&query->sstr.load_data);
    } break;
    case SCF_BEGIN:
    case SCF_COMMIT:
    case SCF_ROLLBACK:
    case SCF_HELP:
    case SCF_EXIT:
    case SCF_ERROR:
      break;
  }
}

void query_destroy(Query *query) {
  query_reset(query);
  free(query);
}
#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

////////////////////////////////////////////////////////////////////////////////

extern "C" int sql_parse(const char *st, Query *sqls);

RC parse(const char *st, Query *sqln) {
  sql_parse(st, sqln);

  if (sqln->flag == SCF_ERROR)
    return SQL_SYNTAX;
  else
    return SUCCESS;
}