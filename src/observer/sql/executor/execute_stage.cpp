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

#include "execute_stage.h"

#include <sstream>
#include <string>

#include "common/io/io.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "event/execution_plan_event.h"
#include "event/session_event.h"
#include "event/sql_event.h"
#include "event/storage_event.h"
#include "group_exec.h"
#include "session/session.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/condition_filter.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/trx/trx.h"

using namespace common;

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db,
                             const char *table_name,
                             SelectExeNode &select_node);
RC create_selection_aggregation_executor(Trx *trx, const Selects &selects,
                                         const char *db, const char *table_name,
                                         SelectAggregationExeNode &select_node);
RC aggr_execution(const Selects &selects, const char *db, TupleSet &tupleset_in,
                  TupleSet &tupleset_out);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

// add by cmk(2021/10/26)
void find_conditions(const Selects &selects, std::vector<Condition> &conditions,
                     std::vector<Condition> &expression_conditions);

bool is_matched_sub_select(const Condition &condition,
                    const TupleSchema &joined_tuple_set_schema,
                    const Tuple &tuple);
// add by cmk : ???????????????????????????????????????????????????joined
// table??????????????????????????????
void select_columns(const TupleSet &joined_table, const Selects &selects,
                    TupleSet &joined_table_selected, bool is_single_table);

// add by cmk : ????????????????????????????????????schema
RC select_column_names(const TupleSchema &old_schema, const Selects &selects,
                       TupleSchema &new_schema, bool is_single_table);

// add by cmk : ??????join
void join_multiple_table(std::vector<TupleSet> &tuple_sets,
                         const std::vector<Condition> &conditions,
                         TupleSet &joined_table);

std::shared_ptr<TupleValue> calculate_result(
    ExpressionNode *node, const TupleSchema &joined_table_schema,
    const Tuple &tuple);

bool is_match_tuple(const std::vector<Condition> &expression_conditions,
                    const TupleSchema &joined_tuple_set_schema,
                    const Tuple &tuple);

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag) {
  ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
  if (stage == nullptr) {
    LOG_ERROR("new ExecuteStage failed");
    return nullptr;
  }
  stage->set_properties();
  return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties() {
  //  std::string stageNameStr(stageName);
  //  std::map<std::string, std::string> section = theGlobalProperties()->get(
  //    stageNameStr);
  //
  //  std::map<std::string, std::string>::iterator it;
  //
  //  std::string key;

  return true;
}

//! Initialize stage params and validate outputs
bool ExecuteStage::initialize() {
  LOG_TRACE("Enter");

  std::list<Stage *>::iterator stgp = next_stage_list_.begin();
  default_storage_stage_ = *(stgp++);
  mem_storage_stage_ = *(stgp++);

  LOG_TRACE("Exit");
  return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup() {
  LOG_TRACE("Enter");

  LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event) {
  LOG_TRACE("Enter\n");

  handle_request(event);

  LOG_TRACE("Exit\n");
  return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context) {
  LOG_TRACE("Enter\n");

  // here finish read all data from disk or network, but do nothing here.
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SQLStageEvent *sql_event = exe_event->sql_event();
  sql_event->done_immediate();

  LOG_TRACE("Exit\n");
  return;
}

void print_selects(const Selects& selects)
{
  LOG_DEBUG("show selects infomation");
  LOG_DEBUG("selects attr num is [%d]",selects.attr_num);
  LOG_DEBUG("selects relation num is [%d]",selects.relation_num);
  LOG_DEBUG("selects condition num is [%d]",selects.condition_num);
  LOG_DEBUG("selects aggr num is [%d]",selects.aggr_num);
  LOG_DEBUG("selects order num is [%d]",selects.order_num);
  LOG_DEBUG("selects group num is [%d]",selects.group_num);

}
RC ExecuteStage::select(const char *db, Query *sql,
                           SessionEvent *session_event) {
  RC rc = RC::SUCCESS;
  //????????? 
  TupleSet* sub_select_result = nullptr;
  if(sql->sub_sstr != NULL){

    const Selects &sub_selects = sql->sub_sstr->selection;
    LOG_DEBUG("sub select information");
    print_selects(sub_selects);
    rc = do_select(db, sub_selects, session_event, true, &sub_select_result);
  } 
  if(rc != RC::SUCCESS ) {
    if(sub_select_result != nullptr){
      delete sub_select_result;
    }
    return rc;
  }
  //?????????
  TupleSet* main_select_result = nullptr;
  const Selects &main_selects = sql->sstr.selection;
  for(int i = 0; i < main_selects.condition_num; i++){
    if(main_selects.conditions[i].sub_select != nullptr){
      main_selects.conditions[i].sub_select->sub_select_result = static_cast<void*>(sub_select_result);
    }
  }
  LOG_DEBUG("main select information");
  print_selects(main_selects);
  rc =  do_select(db, main_selects, session_event, false, &main_select_result);
  delete sub_select_result;
  delete main_select_result;
  return rc;
}

void ExecuteStage::handle_request(common::StageEvent *event) {
  ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
  SessionEvent *session_event = exe_event->sql_event()->session_event();
  Query *sql = exe_event->sqls();
  const char *current_db =
      session_event->get_client()->session->get_current_db().c_str();

  CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
  if (cb == nullptr) {
    LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
    exe_event->done_immediate();
    return;
  }
  exe_event->push_callback(cb);

  switch (sql->flag) {
    case SCF_SELECT: {  // select
      RC rc =
          select(current_db, sql, exe_event->sql_event()->session_event());
      if (rc != RC::SUCCESS) {
        session_event->set_response("FAILURE\n");
      }
      exe_event->done_immediate();
    } break;

    case SCF_INSERT:
    case SCF_UPDATE:
    case SCF_DELETE:
    case SCF_CREATE_TABLE:
    case SCF_SHOW_TABLES:
    case SCF_DESC_TABLE:
    case SCF_DROP_TABLE:
    case SCF_CREATE_INDEX:
    case SCF_DROP_INDEX:
    case SCF_LOAD_DATA: {
      StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
      if (storage_event == nullptr) {
        LOG_ERROR("Failed to new StorageEvent");
        event->done_immediate();
        return;
      }

      default_storage_stage_->handle_event(storage_event);
    } break;
    case SCF_SYNC: {
      RC rc = DefaultHandler::get_default().sync();
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    } break;
    case SCF_BEGIN: {
      session_event->get_client()->session->set_trx_multi_operation_mode(true);
      session_event->set_response(strrc(RC::SUCCESS));
      exe_event->done_immediate();
    } break;
    case SCF_COMMIT: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->commit();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    } break;
    case SCF_ROLLBACK: {
      Trx *trx = session_event->get_client()->session->current_trx();
      RC rc = trx->rollback();
      session_event->get_client()->session->set_trx_multi_operation_mode(false);
      session_event->set_response(strrc(rc));
      exe_event->done_immediate();
    } break;
    case SCF_HELP: {
      const char *response =
          "show tables;\n"
          "desc `table name`;\n"
          "create table `table name` (`column name` `column type`, ...);\n"
          "create index `index name` on `table` (`column`);\n"
          "insert into `table` values(`value1`,`value2`);\n"
          "update `table` set column=value [where `column`=`value`];\n"
          "delete from `table` [where `column`=`value`];\n"
          "select [ * | `columns` ] from `table`;\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    } break;
    case SCF_EXIT: {
      // do nothing
      const char *response = "Unsupported\n";
      session_event->set_response(response);
      exe_event->done_immediate();
    } break;
    default: {
      exe_event->done_immediate();
      LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
  }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right) {
  if (!session->is_trx_multi_operation_mode()) {
    if (all_right) {
      trx->commit();
    } else {
      trx->rollback();
    }
  }
}

bool check_expression_valid(const Selects &selects, const char *db,
                            ExpressionNode *node, bool is_single_table) {
  if (node) {
    if (node->isExpression) {
      return check_expression_valid(selects, db, node->left_expression,
                                    is_single_table) &&
             check_expression_valid(selects, db, node->right_expression,
                                    is_single_table);
    } else if (node->isValue) {
    //   return node->constant_value->type == INTS ||
    //          node->constant_value->type == FLOATS;
        return true;
    } else {
      char *relation_name = node->relation_attr->relation_name;
      char *attribute_name = node->relation_attr->attribute_name;
      if (relation_name != nullptr) {
        Table *table =
            DefaultHandler::get_default().find_table(db, relation_name);
        if (nullptr == table) {
          LOG_WARN("No such table [%s] in db [%s]", relation_name, db);
          return false;
        }

        // ???????????????????????????????????????
        bool isExistInSelectTable = false;
        for (int i = 0; i < selects.relation_num; i++) {
          if (strcmp(selects.relations[i], relation_name) == 0) {
            isExistInSelectTable = true;
            break;
          }
        }

        if (!isExistInSelectTable) {
          return false;
        }

        // ??????????????????????????????????????????attribute
        const FieldMeta *field = table->table_meta().field(attribute_name);
        if (field == nullptr) {
          return false;
        }
      } else {
        // ???????????????????????? ??????????????? ???????????????
        if (selects.relation_num != 1) {
          return false;
        }
        // ??????????????? ????????????????????????????????????
        relation_name = selects.relations[0];
        Table *table =
            DefaultHandler::get_default().find_table(db, relation_name);
        if (nullptr == table) {
          LOG_WARN("No such table [%s] in db [%s]", relation_name, db);
          return false;
        }

        // ??????????????????????????????????????????attribute
        const FieldMeta *field = table->table_meta().field(attribute_name);
        if (field == nullptr) {
          return false;
        }
      }

      return true;
    }
  }
  return false;
}

// ?????????????????????????????????????????????????????????????????????????????????where?????????????????????????????????????????????????????????
// ???????????????????????????.
// ???????????????????????????resolve????????????execution????????????????????????
RC ExecuteStage::do_select(const char *db, const Selects& selects,
                           SessionEvent *session_event, bool is_sub_select, TupleSet** select_result) {
  RC rc = RC::SUCCESS;
  Session *session = session_event->get_client()->session;
  Trx *trx = session->current_trx();

  // Expression ???????????????
  for (int i = 0; i < selects.attr_num; i++) {
    RelAttr attribute = selects.attributes[i];
    ExpressionNode *node = selects.attributes[i].node;
    if (node != nullptr) {
      bool is_single_table;
      if (selects.relation_num > 1)
        is_single_table = false;
      else if (selects.relation_num == 1)
        is_single_table = true;
      else
        return RC::GENERIC_ERROR;
      bool is_valid =
          check_expression_valid(selects, db, node, is_single_table);
      if (!is_valid) {
        return GENERIC_ERROR;
      }
    }
  }

  // condition expression ???????????????
  for (int i = 0; i < selects.condition_num; i++) {
    Condition condition = selects.conditions[i];
    ExpressionNode *node = condition.left_expression;
    if (node != nullptr) {
      bool is_single_table;
      if (selects.relation_num > 1)
        is_single_table = false;
      else if (selects.relation_num == 1)
        is_single_table = true;
      else
        return RC::GENERIC_ERROR;
      bool is_valid =
          check_expression_valid(selects, db, node, is_single_table);
      if (!is_valid) {
        return GENERIC_ERROR;
      }
    }

    node = condition.right_expression;
    if (node != nullptr) {
      bool is_single_table;
      if (selects.relation_num > 1)
        is_single_table = false;
      else if (selects.relation_num == 1)
        is_single_table = true;
      else
        return RC::GENERIC_ERROR;
      bool is_valid =
          check_expression_valid(selects, db, node, is_single_table);
      if (!is_valid) {
        return GENERIC_ERROR;
      }
    }
  }

  // ???order?????????????????????
  for (int i = 0; i < selects.order_num; i++) {
    char *relation_name = selects.order_attr[i].relation_name;
    char *attribute_name = selects.order_attr[i].attribute_name;
    if (relation_name != nullptr) {
      Table *table =
          DefaultHandler::get_default().find_table(db, relation_name);
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      // ???????????????????????????????????????
      bool isExistInSelectTable = false;
      for (int i = 0; i < selects.relation_num; i++) {
        if (strcmp(selects.relations[i], relation_name) == 0) {
          isExistInSelectTable = true;
          break;
        }
      }
      // ?????????????????????????????? ????????????select???relation????????? return false
      if (!isExistInSelectTable) {
        return RC::GENERIC_ERROR;
      }

      // ??????????????????????????????????????????attribute
      const FieldMeta *field = table->table_meta().field(attribute_name);
      if (field == nullptr) {
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    } else {
      // ???????????????????????? ??????????????? ???????????????
      if (selects.relation_num != 1) {
        return RC::GENERIC_ERROR;
      }
      // ??????????????? ????????????????????????????????????
      relation_name = selects.relations[0];
      Table *table =
          DefaultHandler::get_default().find_table(db, relation_name);
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }

      // ??????????????????????????????????????????attribute
      const FieldMeta *field = table->table_meta().field(attribute_name);
      if (field == nullptr) {
        return RC::SCHEMA_FIELD_NOT_EXIST;
      }
    }
  }

  // ??????????????????????????????????????????condition?????????????????????????????????select ????????????
  std::vector<ExecutionNode *> select_nodes;
  for (size_t i = 0; i < selects.relation_num; i++) {
    const char *table_name = selects.relations[i];
    ExecutionNode *select_node;
    // if (false) {
    //     select_node = new SelectAggregationExeNode;
    //     SelectAggregationExeNode* select_node_in =
    //     dynamic_cast<SelectAggregationExeNode*>(select_node); rc =
    //     create_selection_aggregation_executor(trx, selects, db, table_name,
    //     *select_node_in);
    // } else {
    //     select_node = new SelectExeNode;
    //     SelectExeNode* select_node_in =
    //     dynamic_cast<SelectExeNode*>(select_node); rc =
    //     create_selection_executor(trx, selects, db, table_name,
    //     *select_node_in);
    // }
    select_node = new SelectExeNode;
    SelectExeNode *select_node_in = dynamic_cast<SelectExeNode *>(select_node);
    rc = create_selection_executor(trx, selects, db, table_name,
                                   *select_node_in);
    if (rc != RC::SUCCESS) {
      delete select_node;
      for (ExecutionNode *&tmp_node : select_nodes) {
        delete tmp_node;
      }
      end_trx_if_need(session, trx, false);
      return rc;
    }
    select_nodes.push_back(select_node);
  }

  if (select_nodes.empty()) {
    LOG_ERROR("No table given");
    end_trx_if_need(session, trx, false);
    return RC::SQL_SYNTAX;
  }

  // add by cmk (2021/10/26)
  std::vector<TupleSet> tuple_sets;

  for (ExecutionNode *&node : select_nodes) {
    TupleSet tuple_set;
    rc = node->execute(tuple_set);
    if (rc != RC::SUCCESS) {
      for (ExecutionNode *&tmp_node : select_nodes) {
        delete tmp_node;
      }
      end_trx_if_need(session, trx, false);
      return rc;
    } else {
      tuple_sets.push_back(std::move(tuple_set));
    }
  }

  std::stringstream ss;
  if (tuple_sets.size() > 1) {
    // ????????????????????????????????????join??????
    bool is_single_table = false;
    bool exist_empty_table = false;
    LOG_INFO("tuple sets size is %d", tuple_sets.size());
    TupleSchema all_tables_schemas;
    for (auto iter = tuple_sets.rbegin(); iter != tuple_sets.rend(); iter++) {
      all_tables_schemas.append((*iter).get_schema());
      if ((*iter).size() == 0) {
        exist_empty_table = true;
      }
    }
    //????????????????????????
    //????????????????????????????????????
    if (exist_empty_table) {
      LOG_INFO("exist empty table,print fields only");
      TupleSchema selected_schema;
      LOG_INFO("selects attr_num is [%d], attr[0] is [%s]", selects.attr_num,
               selects.attributes[0].attribute_name);
      if (RC::SUCCESS != select_column_names(all_tables_schemas, selects,
                                             selected_schema,
                                             is_single_table)) {
        return RC::GENERIC_ERROR;
      }
      selected_schema.print(ss, is_single_table);
    } else {
      //??????join
      LOG_INFO("multiple table joining");
      TupleSet joined_table;
      std::vector<Condition> conditions;
      std::vector<Condition> expression_conditions;
      find_conditions(selects, conditions, expression_conditions);
      join_multiple_table(tuple_sets, conditions, joined_table);

      LOG_DEBUG("multiple table joined, joined table size is [%d]",
                joined_table.size());

      // ???????????? ?????????????????????????????????
      const TupleSchema joined_tuple_set_schema = joined_table.schema();

      TupleSet new_tuple_set;
      new_tuple_set.set_schema(joined_tuple_set_schema);

      for (int tuple_index = 0; tuple_index < joined_table.size();
           tuple_index++) {
        Tuple tuple = joined_table.get(tuple_index);

        bool isOk = is_match_tuple(expression_conditions,
                                   joined_tuple_set_schema, tuple);
        // ?????????????????????
        if (isOk) {
          new_tuple_set.add(std::move(tuple));
        }
      }

      // select????????????

      if (selects.group_num > 0) {
        // 1. ???????????????TupleSet
        std::vector<TupleSet> tuple_sets_out;
        rc = group_decompose(new_tuple_set, tuple_sets_out, selects, db);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        std::vector<TupleSet> edited_sets;
        if (selects.aggr_num > 0) {
          // 2. ??????????????????
          rc = group_aggr(tuple_sets_out, edited_sets, selects, db);
          if (rc != RC::SUCCESS) {
            return rc;
          }
        } else {
          // 3. ??????TupleSet????????????????????????
          // 4. ???????????????select?????????
          rc = group_de_duplication(tuple_sets_out, edited_sets, selects, db);
          if (rc != RC::SUCCESS) {
            return rc;
          }
        }
        // 5. ????????????TupleSet
        // 6. ??????TupleSet
        TupleSet* final_tupleset = new TupleSet();
        rc = group_compose(edited_sets, *final_tupleset);
        *select_result = final_tupleset;
        //????????????????????????
        if(!is_sub_select){
          (*final_tupleset).print(ss, is_single_table);
        }
      } else {
        TupleSet* joined_table_selected = new TupleSet();
        if (selects.aggr_num > 0) {
          aggr_execution(selects, db, new_tuple_set, *joined_table_selected);
          if(!is_sub_select){
            (*joined_table_selected).print(ss, is_single_table);
          }
        } else {
          select_columns(new_tuple_set, selects, *joined_table_selected,
                         is_single_table);
          if (selects.order_num > 0) {
            (*joined_table_selected).sort(is_single_table, selects.order_attr,
                                       selects.order_num);
          }
          if(!is_sub_select){
            (*joined_table_selected).print(ss, is_single_table);
          }
        }
        *select_result = joined_table_selected;
      }
    }
  } else {
    bool is_single_table = true;

    if (selects.group_num > 0) {
      // 1. ???????????????TupleSet
      std::vector<TupleSet> tuple_sets_out;
      rc = group_decompose(tuple_sets[0], tuple_sets_out, selects, db);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      std::vector<TupleSet> edited_sets;
      if (selects.aggr_num > 0) {
        // 2. ??????????????????
        rc = group_aggr(tuple_sets_out, edited_sets, selects, db);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      } else {
        // 3. ??????TupleSet????????????????????????
        // 4. ???????????????select?????????
        rc = group_de_duplication(tuple_sets_out, edited_sets, selects, db);
        if (rc != RC::SUCCESS) {
          return rc;
        }
      }
      // 5. ????????????TupleSet
      // 6. ??????TupleSet
      TupleSet* final_tupleset = new TupleSet();
      rc = group_compose(edited_sets, *final_tupleset);
      *select_result = final_tupleset;
      if(!is_sub_select){
        (*final_tupleset).print(ss, is_single_table);
      }
    } else {
      // ???????????????????????????????????????????????????
      TupleSet *table_selected = new TupleSet();
      if (selects.aggr_num > 0) {
        aggr_execution(selects, db, tuple_sets[0], *table_selected);
        *select_result = table_selected;
        if(!is_sub_select){
          (*table_selected).print(ss, is_single_table);
        }
      } else {
        std::vector<Condition> expression_conditions;
        std::vector<Condition> sub_select_conditions;
        TupleSet new_tuple_set;
        new_tuple_set.set_schema(tuple_sets[0].schema());
        for (int i = 0; i < selects.condition_num; i++) {
          if (selects.conditions[i].sub_select == nullptr &&
            (selects.conditions[i].left_expression->isExpression  || selects.conditions[i].right_expression->isExpression)) {
            expression_conditions.push_back(selects.conditions[i]);
          }
          if(selects.conditions[i].sub_select != nullptr){
            sub_select_conditions.push_back(selects.conditions[i]);
          }
        }

        //???comp??????IN/NOT IN, sub_select???????????????????????????????????????????????????????????????
        if(sub_select_conditions.size() != 0){
            Condition& sub_select_condition = sub_select_conditions[0];
            CompOp sb_op = sub_select_condition.comp;
            TupleSet* sub_select_result = static_cast<TupleSet*>(sub_select_condition.sub_select->sub_select_result); 
            if( sb_op != IS_IN && sb_op != NOT_IN){    
              if(!(sub_select_result->size() == 1 && sub_select_result->get(0).size() == 1)){
                return RC::GENERIC_ERROR;
              }
            }
        }
        for (int i = 0; i < tuple_sets[0].size(); i++) {
          Tuple tuple = tuple_sets[0].get(i);
          bool l_isOk = true;
          //??????????????????????????????????????????
          if(sub_select_conditions.size() != 0){
              l_isOk = is_matched_sub_select(sub_select_conditions[0], new_tuple_set.get_schema(), tuple);
          }
          bool r_isOk = is_match_tuple(expression_conditions,
                                     new_tuple_set.get_schema(), tuple);
          bool isOk = (l_isOk && r_isOk);
          // ?????????????????????
          if (isOk) {
            new_tuple_set.add(std::move(tuple));
          }
        }

        select_columns(new_tuple_set, selects, *table_selected, is_single_table);
        // ?????????
        if (selects.order_num > 0) {
          (*table_selected).sort(is_single_table, selects.order_attr,
                              selects.order_num);
        }
        *select_result = table_selected;
        if(!is_sub_select){
          (*table_selected).print(ss, is_single_table);
        }
      }
    }
  }

  for (ExecutionNode *&tmp_node : select_nodes) {
    delete tmp_node;
  }
  session_event->set_response(ss.str());
  end_trx_if_need(session, trx, true);
  return rc;
}

bool is_match(std::shared_ptr<TupleValue>& left_value, std::shared_ptr<TupleValue>& right_value, CompOp op)
{
    bool isOk = true;
    if (left_value != nullptr && right_value != nullptr) {
      float f1, f2;
      // ?????????????????????????????????
      if ((*left_value).type() == INT) {
        f1 = std::dynamic_pointer_cast<IntValue>(left_value)->value();
      } else if ((*left_value).type() == FLOAT) {
        f1 = std::dynamic_pointer_cast<FloatValue>(left_value)->value();
      } else {
        return false;
      }

      if ((*right_value).type() == INT) {
        f2 = std::dynamic_pointer_cast<IntValue>(right_value)->value();
      } else if ((*right_value).type() == FLOAT) {
        f2 = std::dynamic_pointer_cast<FloatValue>(right_value)->value();
      } else {
        return false;
      }

      float result = f1 - f2;

      // ?????????????????????????????????????????? IS ISNOT?????????
      if (op == EQUAL_TO && result != 0) {
        isOk = false;
      } else if (op == LESS_EQUAL && result > 0) {
        isOk = false;
      } else if (op == NOT_EQUAL && result == 0) {
        isOk = false;
      } else if (op == LESS_THAN && result >= 0) {
        isOk = false;
      } else if (op == GREAT_EQUAL && result < 0) {
        isOk = false;
      } else if (op == GREAT_THAN && result <= 0) {
        isOk = false;
      } 
    }
  return isOk;

}
bool is_match_tuple(const std::vector<Condition> &expression_conditions,
                    const TupleSchema &joined_tuple_set_schema,
                    const Tuple &tuple) {
  bool isOk = true;
  for (int i = 0; i < expression_conditions.size(); i++) {
    Condition condition = expression_conditions[i];
    CompOp op = condition.comp;
    ExpressionNode *left_node = expression_conditions[i].left_expression;
    ExpressionNode *right_node = expression_conditions[i].right_expression;

    std::shared_ptr<TupleValue> left_value =
        calculate_result(left_node, joined_tuple_set_schema, tuple);
    if(left_value == nullptr) return false;

    std::shared_ptr<TupleValue> right_value =
        calculate_result(right_node, joined_tuple_set_schema, tuple);
    if(right_value == nullptr) return false;

    isOk = is_match(left_value, right_value, op);
    //????????????condition??????????????????????????????
    if(isOk == false){
      break;
    }
  }
  return isOk;
}

bool is_in_subset(std::shared_ptr<TupleValue> left_value, TupleSet* right_tupleset, CompOp op)
{
  bool isOk = true;
  if(op == IS_IN){
    if(right_tupleset->size() == 0)
       return false;
    op = EQUAL_TO;
  }else if(op == NOT_IN){
    if(right_tupleset->size() == 0)
       return true;
    op = NOT_EQUAL;
  }
  //??????????????????
  assert(right_tupleset->get_schema().total_size() == 1);
  for(int i = 0; i < right_tupleset->size(); i++){
    std::shared_ptr<TupleValue> right_value = right_tupleset->get_edit(i).get_edit(0);
    isOk = is_match(left_value, right_value, op);
    if(isOk && op == EQUAL_TO){
      break;
    }
    if(!isOk && op == NOT_EQUAL){
      break;
    }
  }
  return isOk;
}

bool is_matched_sub_select(const Condition &condition,
                    const TupleSchema &joined_tuple_set_schema,
                    const Tuple &tuple) {
  bool isOk = true;
  int right_is_sub_select = condition.right_is_sub_select;
  CompOp op = condition.comp;

  TupleSet* sub_select_result = static_cast<TupleSet*>(condition.sub_select->sub_select_result); 
  std::shared_ptr<TupleValue> left_value;
  std::shared_ptr<TupleValue> right_value;
  //????????????condition?????????
  if(right_is_sub_select){
      ExpressionNode* left_node = condition.left_expression;
      left_value = calculate_result(left_node, joined_tuple_set_schema, tuple);
      if(left_value == nullptr) return false;

      if(op == IS_IN || op == NOT_IN){
        return is_in_subset(left_value, sub_select_result, op);
      }


      if(sub_select_result->size() == 1 && sub_select_result->get(0).size() == 1){
        right_value = sub_select_result->get(0).get_edit(0);
        if(right_value == nullptr) return false;
      }else{
        return false;
      }
  }else{ //????????????condition?????????
      ExpressionNode* right_node = condition.right_expression;
       right_value = calculate_result(right_node, joined_tuple_set_schema, tuple);
      if(right_value == nullptr) return false;

      //op???IN/NOT IN????????????????????????condition?????????
      if(op == IS_IN || op == NOT_IN){
        return false;
      }

      if(sub_select_result->size() == 1 && sub_select_result->get(0).size() == 1){
        left_value = sub_select_result->get(0).get_edit(0);
        if(left_value == nullptr) return false;
      }else{
        return false;
      }
  }

  isOk = is_match(left_value, right_value, op);
  return isOk;
}

bool match_table(const Selects &selects, const char *table_name_in_condition,
                 const char *table_name_to_match) {
  if (table_name_in_condition != nullptr) {
    return 0 == strcmp(table_name_in_condition, table_name_to_match);
  }

  return selects.relation_num == 1;
}

static RC schema_add_field(Table *table, const char *field_name,
                           TupleSchema &schema) {
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }

  schema.add_if_not_exists(field_meta->type(), table->name(),
                           field_meta->name());
  return RC::SUCCESS;
}

static RC schema_add_aggr_field(const AggrAttr *aggr, Table *table,
                                const char *field_name, TupleSchema &schema) {
  if (aggr->is_constant) {
    schema.add_aggr(aggr->aggr_type, AttrType::UNDEFINED, table->name(), table,
                    field_name, &aggr->constant_value);
    return RC::SUCCESS;
  }
  if (0 == strcmp("*", field_name)) {
    if (aggr->aggr_type == AggrType::COUNT) {
      schema.add_aggr(aggr->aggr_type, AttrType::INTS, table->name(), table,
                      field_name, nullptr);
      return RC::SUCCESS;
    } else {
      LOG_DEBUG("AVG MIN MAX not support * field");
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }
  const FieldMeta *field_meta = table->table_meta().field(field_name);
  if (nullptr == field_meta) {
    LOG_WARN("No such field in aggregation. %s.%s", table->name(), field_name);
    return RC::SCHEMA_FIELD_MISSING;
  }
  switch (aggr->aggr_type) {
    case AggrType::COUNT:
      schema.add_aggr(aggr->aggr_type, AttrType::INTS, table->name(), table,
                      field_meta->name(), nullptr);
      break;
    case AggrType::AVG:
      if (field_meta->type() != AttrType::INTS &&
          field_meta->type() != AttrType::FLOATS) {
        LOG_WARN(
            "Field type in aggregation not support aggregation op of avg. "
            "%s.%s",
            table->name(), field_meta->name());
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      schema.add_aggr(aggr->aggr_type, AttrType::FLOATS, table->name(), table,
                      field_meta->name(), nullptr);
      break;
    case AggrType::MAX:
    case AggrType::MIN:
      if (field_meta->type() == AttrType::UNDEFINED) {
        // Min Max ???????????????????????????UNDEFINED
        LOG_WARN(
            "Field type in aggregation not support aggregation op of max, min. "
            "%s.%s",
            table->name(), field_meta->name());
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
      schema.add_aggr(aggr->aggr_type, field_meta->type(), table->name(), table,
                      field_meta->name(), nullptr);
      break;
    default:
      LOG_DEBUG("Not Support AGGR_UNDEGINED.");
      return RC::GENERIC_ERROR;
      break;
  }
  return RC::SUCCESS;
}

RC aggr_execution(const Selects &selects, const char *db, TupleSet &tupleset_in,
                  TupleSet &tupleset_out) {
  RC rc = RC::SUCCESS;
  TupleSchema schema;
  std::vector<const AggrAttr *> attrs;
  std::vector<GroupAttr> groupattrs_compare;
  if (selects.group_num > 0) {
    for (int i = selects.group_num - 1; i >= 0; i--) {
      // ????????????????????????????????????
      GroupAttr group_attr(selects.group_attr[i]);
      Table *table;
      if (group_attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   group_attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          group_attr.relation_name = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        table = DefaultHandler::get_default().find_table(
            db, group_attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", group_attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      const FieldMeta *field_meta =
          table->table_meta().field(group_attr.attribute_name);
      if (nullptr == field_meta) {
        LOG_WARN("No such field in group. %s.%s", table->name(),
                 group_attr.attribute_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      groupattrs_compare.push_back(group_attr);
    }
  }
  if (selects.aggr_num > 0) {
    for (int i = 0; i < selects.aggr_num; i++) {
      const AggrAttr &attr = selects.aggr_attr[i];
      char *rel_comp;
      char *attr_comp;
      Table *table;
      if (attr.is_constant) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr only recieve constant in single table.");
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
          attrs.push_back(&attr);
          schema_add_aggr_field(&attr, table, attr.attribute_name, schema);
          continue;
        }
      }
      if (attr.relation_name == nullptr) {
        if (selects.relation_num > 1) {
          LOG_WARN("Aggr must have table name in multi table, field [%s]",
                   attr.attribute_name);
          return RC::SCHEMA_TABLE_NOT_EXIST;
        } else {
          rel_comp = selects.relations[0];
          table = DefaultHandler::get_default().find_table(
              db, selects.relations[0]);
        }
      } else {
        rel_comp = attr.relation_name;
        table =
            DefaultHandler::get_default().find_table(db, attr.relation_name);
      }
      if (nullptr == table) {
        LOG_WARN("No such table [%s] in db [%s]", attr.relation_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
      if (strcmp("*", attr.attribute_name) != 0) {
        const FieldMeta *field_meta =
            table->table_meta().field(attr.attribute_name);
        if (nullptr == field_meta) {
          LOG_WARN("No such field in aggr. %s.%s", table->name(),
                   attr.attribute_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        attr_comp = attr.attribute_name;
        attrs.push_back(&attr);
        schema_add_aggr_field(&attr, table, attr.attribute_name, schema);
      } else {
        attr_comp = attr.attribute_name;
        attrs.push_back(&attr);
        schema_add_aggr_field(&attr, table, attr.attribute_name, schema);
      }
    }
    tupleset_out.set_schema(schema);
  }
  AggregationRecordConverter converter(tupleset_out, attrs,
                                       &tupleset_in.schema());
  for (int i = 0; i < tupleset_in.size(); i++) {
    converter.read_tuple(tupleset_in.get(i));
  }
  rc = converter.final_add_record();
  return rc;
}

RC create_selection_aggregation_executor(
    Trx *trx, const Selects &selects, const char *db, const char *table_name,
    SelectAggregationExeNode &select_node) {
  // ???????????????????????????Attr
  TupleSchema schema;
  Table *table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }
  const TableMeta &table_meta = table->table_meta();

  if (selects.aggr_num > 0) {
    for (int i = selects.aggr_num - 1; i >= 0; i--) {
      // ????????????????????????????????????
      const AggrAttr &attr = selects.aggr_attr[i];
      if (nullptr == attr.relation_name ||
          0 == strcmp(table_name, attr.relation_name)) {
        select_node.add_aggr_attr(&attr);
        RC rc =
            schema_add_aggr_field(&attr, table, attr.attribute_name, schema);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        // schema.add_aggr(attr.aggr_type, field->type(), table_name,
        // attr.attribute_name);
      } else {
        return RC::SCHEMA_TABLE_NAME_ILLEGAL;
      }
    }
  }
  // ???????????????????????????????????????, ??????????????????????????????
  std::vector<DefaultConditionFilter *> condition_filters;
  for (size_t i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    if ((condition.left_is_attr == 0 &&
         condition.right_is_attr == 0) ||  // ???????????????
        (condition.left_is_attr == 1 && condition.right_is_attr == 0 &&
         match_table(selects, condition.left_attr.relation_name,
                     table_name)) ||  // ???????????????????????????
        (condition.left_is_attr == 0 && condition.right_is_attr == 1 &&
         match_table(selects, condition.right_attr.relation_name,
                     table_name)) ||  // ?????????????????????????????????
        (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
         match_table(selects, condition.left_attr.relation_name, table_name) &&
         match_table(selects, condition.right_attr.relation_name,
                     table_name))  // ?????????????????????????????????????????????
    ) {
      DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
      RC rc = condition_filter->init(*table, condition);
      if (rc != RC::SUCCESS) {
        delete condition_filter;
        for (DefaultConditionFilter *&filter : condition_filters) {
          delete filter;
        }
        return rc;
      }
      condition_filters.push_back(condition_filter);
    }
  }
  return select_node.init(trx, table, std::move(schema),
                          std::move(condition_filters));
}

// ??????????????????????????????????????????condition?????????????????????????????????select ????????????
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db,
                             const char *table_name,
                             SelectExeNode &select_node) {
  // ???????????????????????????Attr
  TupleSchema schema;
  Table *table = DefaultHandler::get_default().find_table(db, table_name);
  if (nullptr == table) {
    LOG_WARN("No such table [%s] in db [%s]", table_name, db);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // for (int i = selects.attr_num - 1; i >= 0; i--)
  // {
  //     const RelAttr &attr = selects.attributes[i];
  //     if (nullptr == attr.relation_name || 0 == strcmp(table_name,
  //     attr.relation_name))
  //     {
  //         if (0 == strcmp("*", attr.attribute_name))
  //         {
  //             // ???????????????????????????
  //             TupleSchema::from_table(table, schema);
  //             break; // ?????????????????????* ??????????????????????????????
  //         }
  //         else
  //         {
  //             // ???????????????????????????
  //             RC rc = schema_add_field(table, attr.attribute_name, schema);
  //             if (rc != RC::SUCCESS)
  //             {
  //                 return rc;
  //             }
  //         }
  //     }
  // }

  // ???????????????????????????????????????, ??????????????????????????????

  std::vector<DefaultConditionFilter *> condition_filters;
  for (size_t i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];

    if (condition.left_expression!= NULL && !condition.left_expression->isExpression &&
        condition.right_expression != NULL && !condition.right_expression->isExpression) {
      if ((condition.left_is_attr == 0 &&
           condition.right_is_attr == 0) ||  // ???????????????
          (condition.left_is_attr == 1 && condition.right_is_attr == 0 &&
           match_table(selects, condition.left_attr.relation_name,
                       table_name)) ||  // ???????????????????????????
          (condition.left_is_attr == 0 && condition.right_is_attr == 1 &&
           match_table(selects, condition.right_attr.relation_name,
                       table_name)) ||  // ?????????????????????????????????
          (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
           match_table(selects, condition.left_attr.relation_name,
                       table_name) &&
           match_table(selects, condition.right_attr.relation_name,
                       table_name))  // ?????????????????????????????????????????????

      ) {
        DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
        RC rc = condition_filter->init(*table, condition);
        if (rc != RC::SUCCESS) {
          delete condition_filter;
          for (DefaultConditionFilter *&filter : condition_filters) {
            delete filter;
          }
          return rc;
        }
        condition_filters.push_back(condition_filter);
      }
    }

    // add by cmk :
    // ????????????????????????????????????????????????????????????????????????????????????????????????????????????
    // if(condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
    //         (0 != strcmp(condition.left_attr.relation_name,
    //         condition.right_attr.relation_name)))
    // {
    //     // ???????????????????????????
    //     LOG_INFO("multi join conditions , still list all attr of one table");
    //     schema.clear();
    //     TupleSchema::from_table(table, schema);
    // }
  }

  TupleSchema::from_table(table, schema);
  return select_node.init(trx, table, std::move(schema),
                          std::move(condition_filters));
}

const std::shared_ptr<TupleValue> get_value_from_tuple(
    const TupleSchema &joined_table_schema, const char *table_name,
    const char *field_name, const Tuple &tuple) {
  int value_index = joined_table_schema.index_of_field(table_name, field_name);
  if (value_index != -1) {
    auto &value = tuple.get_pointer(value_index);
    return value;
  } else {
    return nullptr;
  }
}

std::shared_ptr<TupleValue> calculate_result(
    ExpressionNode *node, const TupleSchema &joined_table_schema,
    const Tuple &tuple) {
  // ?????????????????????????????????
  int sign;
  if(node->sign==SUB) {
      sign = -1;
  }else {
      sign = 1;
  }
  if (node->isExpression) {
    std::shared_ptr<TupleValue> left =
        calculate_result(node->left_expression, joined_table_schema, tuple);

    std::shared_ptr<TupleValue> right =
        calculate_result(node->right_expression, joined_table_schema, tuple);

    int i1, i2;
    float f1, f2;

    // ?????????????????????????????????
    if(left == nullptr) {
        return nullptr;
    }

    if ((*left).type() == INT) {
      f1 = std::dynamic_pointer_cast<IntValue>(left)->value();
    } else if ((*left).type() == FLOAT) {
      f1 = std::dynamic_pointer_cast<FloatValue>(left)->value();
    } else {
      return nullptr;
    }

    if(right == nullptr) {
        return nullptr;
    }

    if ((*right).type() == INT) {
      f2 = std::dynamic_pointer_cast<IntValue>(right)->value();
    } else if ((*right).type() == FLOAT) {
      f2 = std::dynamic_pointer_cast<FloatValue>(right)->value();
    } else {
      return nullptr;
    }

    float number = 0;
    switch (node->op) {
      case ADD:
        number = sign * (f1 + f2);
        break;
      case SUB:
        number = sign *(f1 - f2);
        break;
      case MUL:
        number = sign *(f1 * f2);
        break;
      case DIV:
        if (f2 == 0) {
          return nullptr;
        }else {
          number = sign *(f1 / f2);
        }
        break;
      default:
        break;
    }

    FloatValue *temp = new FloatValue(number);
    std::shared_ptr<TupleValue> result(temp);
    return result;
  }
  //   ???????????????????????????tuple????????????
  else if (node->isValue) {
    if (node->constant_value->type == INTS) {
      int number = *(int *)(node->constant_value->data);
      IntValue *temp = new IntValue(number);
      std::shared_ptr<TupleValue> result(temp);
      return result;
    } else if (node->constant_value->type == FLOATS) {
      float number = *(float *)(node->constant_value->data);
      FloatValue *temp = new FloatValue(number);
      std::shared_ptr<TupleValue> result(temp);
      return result;
    } else {
      // ?????????????????? ????????????????????????
      return nullptr;
    }

  }
  //   ???????????????????????? ?????????????????????
  else {
    const std::shared_ptr<TupleValue> result = get_value_from_tuple(
        joined_table_schema, node->relation_attr->relation_name,
        node->relation_attr->attribute_name, tuple);
    return result;
  }
  return nullptr;
}

std::string op_2_string(OpType type) {
  switch (type) {
    case ADD:
      return "+";
      break;
    case SUB:
      return "-";
      break;
    case MUL:
      return "*";
      break;
    case DIV:
      return "/";
      break;
    default:
      break;
  }
  return "";
}

std::string expression_2_string(ExpressionNode *node,bool is_single_table) {
  std::string result;
  if (node != nullptr) {

    if (node->isExpression){
      result = expression_2_string(node->left_expression,is_single_table) +
               op_2_string(node->op) +
               expression_2_string(node->right_expression,is_single_table);}
    else if (node->isValue){
      result = node->constant_value->type == INTS
                   ? std::to_string(*(int *)node->constant_value->data)
                   : std::to_string(*(float *)node->constant_value->data);}
    else {
        if(is_single_table) {
            result =  std::string(node->relation_attr->attribute_name);
        }else {
            result = node->relation_attr->relation_name != nullptr 
                   ? std::string(node->relation_attr->relation_name) + "." +
                         std::string(node->relation_attr->attribute_name)
                   : std::string(node->relation_attr->attribute_name);
        }
    }
    if (node->isBracket) {
      result = "(" + result + ")";
    }

    if(node->sign == SUB) {
        result = "-" + result;
    }
  }
  return result;
}

// add by cmk : ????????????????????????????????????schema
RC select_column_names(const TupleSchema &old_schema, const Selects &selects,
                       TupleSchema &new_schema, bool is_single_table) {
  const std::vector<TupleField> fields = old_schema.fields();
  if (fields.size() <= 0) {
    LOG_INFO("old_schema does not have fields");
    return RC::GENERIC_ERROR;
  }
  if (selects.attr_num <= 0) {
    LOG_INFO("selects dose not have attributes");
    return RC::GENERIC_ERROR;
  }

  if (selects.attributes[0].node != nullptr) {
    // ?????? * ?????????
    for (int j = 0; j <selects.attr_num; j++) {
      const RelAttr &attr = selects.attributes[j];
      std::string expression_string = expression_2_string(attr.node,is_single_table);
      new_schema.add_expression_field(expression_string);
    }
  } else {
    //????????????????????????
    for (int j = (selects.attr_num - 1); j >= 0; j--) {
      const RelAttr &attr = selects.attributes[j];
      LOG_INFO("attr table name is [%s], field name is [%s]",
               attr.relation_name, attr.attribute_name);
      //???????????????????????????condition??????????????????
      if (!is_single_table && (0 != strcmp(attr.attribute_name, "*")) &&
          attr.relation_name == nullptr) {
        return RC::GENERIC_ERROR;
      }

      if (attr.attribute_name == nullptr) continue;

      for (auto field : fields) {
        if (!is_single_table) {  //??????
          if ((0 == strcmp(attr.attribute_name, "*")) ||
              ((0 == strcmp(field.table_name(), attr.relation_name)) &&
               (0 == strcmp(field.field_name(), attr.attribute_name)))) {
            LOG_DEBUG(
                "select_columns_name() multi table: new_schema table name is "
                "[%s],field name is [%s]",
                field.table_name(), field.field_name());
            new_schema.add_if_not_exists(field.type(), field.table_name(),
                                         field.field_name());
          }
        } else {  //??????
          LOG_DEBUG("SINGLE");
          if (nullptr == attr.relation_name) {
            if (0 == strcmp(field.field_name(), attr.attribute_name) ||
                (0 == strcmp(attr.attribute_name, "*"))) {
              LOG_DEBUG(
                  "select_columns_name() single table: new_schema table name "
                  "is [%s],field name is [%s]",
                  field.table_name(), field.field_name());
              new_schema.add_if_not_exists(field.type(), field.table_name(),
                                           field.field_name());
            }
          } else {
            if (((0 == strcmp(field.table_name(), attr.relation_name)) &&
                 (0 == strcmp(field.field_name(), attr.attribute_name)))) {
              LOG_DEBUG(
                  "select_columns_name() single table: new_schema table name "
                  "is [%s],field name is [%s]",
                  field.table_name(), field.field_name());
              new_schema.add_if_not_exists(field.type(), field.table_name(),
                                           field.field_name());
            }
          }
        }
      }
    }
    if (new_schema.fields().size() <= 0) {
      LOG_INFO("select column names failed");
      return RC::GENERIC_ERROR;
    }
  }
  return RC::SUCCESS;
}

// add by cmk : ???????????????????????????????????????????????????joined
// table??????????????????????????????
void select_columns(const TupleSet &joined_table, const Selects &selects,
                    TupleSet &joined_table_selected, bool is_single_table) {
  if (joined_table.size() == 0) {
    LOG_WARN("src table size is 0");
  }
  const TupleSchema &joined_table_schema = joined_table.schema();
  const std::vector<TupleField> &joined_table_fields =
      joined_table_schema.fields();
  LOG_DEBUG("select columns() :joined table schema size is [%d]",
            joined_table_fields.size());
  TupleSchema schema;
  //????????????????????????
  if (RC::SUCCESS != select_column_names(joined_table_schema, selects, schema,
                                         is_single_table)) {
    LOG_INFO("select columns names failed");
  }
  LOG_DEBUG("select columns() :selected schema size is [%d]",
            schema.fields().size());
  const std::vector<TupleField> &selected_table_fields = schema.fields();

  if (selects.attributes[0].node != nullptr) {
    for (int i = 0; i < joined_table.size(); i++) {
      //?????????????????????????????????
      const Tuple &tuple = joined_table.get(i);
      Tuple select_tuple;
      for (int j = 0; j < selects.attr_num; j++) {
        ExpressionNode *node = selects.attributes[j].node;
        std::shared_ptr<TupleValue> result =
            calculate_result(node, joined_table_schema, tuple);
        if (result != nullptr) {
          select_tuple.add(calculate_result(node, joined_table_schema, tuple));
        }
      }
      joined_table_selected.add(std::move(select_tuple));
    }
  } else {
    for (int i = 0; i < joined_table.size(); i++) {
      //?????????????????????????????????
      const Tuple &tuple = joined_table.get(i);
      Tuple select_tuple;
      for (auto field : selected_table_fields) {
        int value_index = joined_table_schema.index_of_field(
            field.table_name(), field.field_name());
        auto &value = tuple.get_pointer(value_index);
        select_tuple.add(value);
      }
      joined_table_selected.add(std::move(select_tuple));
    }
  }
  joined_table_selected.set_schema(schema);
  LOG_DEBUG("joined_table_selected size is [%d]", joined_table_selected.size());
}

// add by cmk(2021/10/24) : ???????????????????????????????????????
void find_conditions(const Selects &selects, std::vector<Condition> &conditions,
                     std::vector<Condition> &expression_conditions) {
  for (size_t i = 0; i < selects.condition_num; i++) {
    const Condition &condition = selects.conditions[i];
    // ???????????????????????????????????? ??????????????????????????? ????????????????????????
    if (!condition.left_expression->isExpression &&
        !condition.right_expression->isExpression) {
      if (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
          (0 != strcmp(condition.left_attr.relation_name,
                       condition.right_attr.relation_name))) {
        conditions.push_back(condition);
      }
    } else {
      expression_conditions.push_back(condition);
    }
  }
}
// add by cmk : ????????????Tuple
void merge_tuple(const Tuple &left_tuple, const Tuple &right_tuple,
                 Tuple &dst_tuple) {
  for (int i = 0; i < left_tuple.size(); i++) {
    dst_tuple.add(left_tuple.get_pointer(i));
  }

  for (int j = 0; j < right_tuple.size(); j++) {
    dst_tuple.add(right_tuple.get_pointer(j));
  }
  return;
}

void strexchg(char **a, char **b) {
  char *c;
  c = *a;
  *a = *b;
  *b = c;
}
//????????????join??????TupleSchema????????????????????????
bool exist_tabl_name(const TupleSchema &schema, const char *table_name) {
  std::vector<TupleField> fields = schema.fields();
  for (int i = 0; i < fields.size(); i++) {
    if ((0 == strcmp(fields[i].table_name(), table_name))) {
      return true;
    }
  }
  return false;
}

// add by cmk : ???????????????????????????conditions
//(?????????3??????????????????????????????TupleSchema????????????????????????)
void find_two_table_condition(const TupleSchema &left_table_schema,
                              const TupleSchema &right_table_schema,
                              const std::vector<Condition> &conditions,
                              std::vector<Condition> &cond_with_two_table) {
  for (auto condition : conditions) {
    if (((true == exist_tabl_name(left_table_schema,
                                  condition.left_attr.relation_name)) &&
         (true == exist_tabl_name(right_table_schema,
                                  condition.right_attr.relation_name)))) {
      cond_with_two_table.push_back(condition);
    }
    //????????????????????????????????????????????????????????????
    if (((true == exist_tabl_name(left_table_schema,
                                  condition.right_attr.relation_name)) &&
         (true == exist_tabl_name(right_table_schema,
                                  condition.left_attr.relation_name)))) {
      //?????????????????????
      LOG_DEBUG(
          "find_two_table_condition : before ex "
          "condition.right_attr.relation_name is [%s]",
          condition.right_attr.relation_name);
      LOG_DEBUG(
          "find_two_table_condition : before ex "
          "condition.left_attr.relation_name is [%s]",
          condition.left_attr.relation_name);
      strexchg(&condition.left_attr.relation_name,
               &condition.right_attr.relation_name);
      strexchg(&condition.left_attr.attribute_name,
               &condition.right_attr.attribute_name);

      LOG_DEBUG(
          "find_two_table_condition : after ex "
          "condition.right_attr.relation_name is [%s]",
          condition.right_attr.relation_name);
      LOG_DEBUG(
          "find_two_table_condition : after ex "
          "condition.left_attr.relation_name is [%s]",
          condition.left_attr.relation_name);
      switch (condition.comp) {
        case LESS_EQUAL:
          condition.comp = GREAT_EQUAL;
          break;
        case LESS_THAN:
          condition.comp = GREAT_THAN;
          break;
        case GREAT_EQUAL:
          condition.comp = LESS_EQUAL;
          break;
        case GREAT_THAN:
          condition.comp = LESS_THAN;
          break;
        default:
          break;
      }

      cond_with_two_table.push_back(condition);
    }
  }
  return;
}

// add by cmk(2021/10/24): ????????????Tuple??????????????????conditions
bool tuple_matched_conditions(const Tuple &left_tuple, const Tuple &right_tuple,
                              const TupleSchema &left_table_schema,
                              const TupleSchema &right_table_schema,
                              const std::vector<Condition> &conditions) {
  bool matched_tuple = false;
  for (auto condition : conditions) {
    int tuple_value_left_index = left_table_schema.index_of_field(
        condition.left_attr.relation_name, condition.left_attr.attribute_name);
    int tuple_value_right_index =
        right_table_schema.index_of_field(condition.right_attr.relation_name,
                                          condition.right_attr.attribute_name);
    auto &left = left_tuple.get(tuple_value_left_index);
    auto &right = right_tuple.get(tuple_value_right_index);

    double cmp_result = 0;
    cmp_result = left.compare(right);
    LOG_INFO("CMP_RESULT is [%d]", cmp_result);
    ValueType left_attr_type = left.type();
    ValueType right_attr_type = right.type();

    if (left_attr_type != right_attr_type) {
      if (left_attr_type != ValueType::NULLTYPE &&
          right_attr_type != ValueType::NULLTYPE) {
        if (!((left_attr_type == ValueType::INT &&
               right_attr_type == ValueType::FLOAT) ||
              (left_attr_type == ValueType::FLOAT &&
               right_attr_type == ValueType::INT))) {
          LOG_DEBUG("Type left not the same as type right");
          return false;
        }
      }
    }
    switch (left_attr_type) {
      case ValueType::STRING: {  // ???????????????????????????????????????
        // ??????C?????????????????????
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          break;
        }
        cmp_result = left.compare(right);
      } break;
      // DATES ?????????INTS????????????????????????
      case ValueType::DATE:
      case ValueType::INT: {
        // ???????????????????????????
        // ???int???float??????????????????????????????,???????????????????????????????????????
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          break;
        }
        const IntValue &int_left = (const IntValue &)left;
        double left_in = int_left.value();
        double right_in;
        if (right_attr_type == ValueType::INT) {
          const IntValue &int_right = (const IntValue &)right;
          right_in = int_right.value();
        } else if (right_attr_type == ValueType::DATE) {
          const DateValue &date_right = (const DateValue &)right;
          right_in = date_right.value();
        } else {
          const FloatValue &date_right = (const FloatValue &)right;
          right_in = date_right.value();
        }
        cmp_result = left_in - right_in;
      } break;
      case FLOAT: {
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          break;
        }
        const FloatValue &float_left = (const FloatValue &)left;
        double left_in = float_left.value();
        double right_in;
        if (right_attr_type == ValueType::INT) {
          const IntValue &int_right = (const IntValue &)right;
          right_in = int_right.value();
        } else if (right_attr_type == ValueType::DATE) {
          const DateValue &date_right = (const DateValue &)right;
          right_in = date_right.value();
        } else {
          const FloatValue &date_right = (const FloatValue &)right;
          right_in = date_right.value();
        }
        cmp_result = left_in - right_in;
      } break;
      default: {
      }
    }
    switch (condition.comp) {
      case EQUAL_TO:
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          matched_tuple = false;
          break;
        }
        matched_tuple = (0 == cmp_result);
        break;
      case LESS_EQUAL:
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          matched_tuple = false;
          break;
        }
        matched_tuple = (cmp_result <= 0);
        break;
      case NOT_EQUAL:
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          matched_tuple = false;
          break;
        }
        matched_tuple = (cmp_result != 0);
        break;
      case LESS_THAN:
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          matched_tuple = false;
          break;
        }
        matched_tuple = (cmp_result < 0);
        break;
      case GREAT_EQUAL:
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          matched_tuple = false;
          break;
        }
        matched_tuple = (cmp_result >= 0);
        break;
      case GREAT_THAN:
        if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type) {
          matched_tuple = false;
          break;
        }
        matched_tuple = (cmp_result > 0);
        break;
      case IS:
        if (NULLTYPE == left_attr_type && NULLTYPE == right_attr_type) {
          matched_tuple = true;
        } else {
          matched_tuple = false;
        }
        break;
      case ISNOT:
        if ((NULLTYPE != left_attr_type && NULLTYPE == right_attr_type) ||
            (NULLTYPE == left_attr_type && NULLTYPE != right_attr_type)) {
          matched_tuple = true;
        } else {
          matched_tuple = false;
        }
        break;
      default:
        matched_tuple = false;
        break;
    }
    //????????????And???????????????(?????????????????????????????????????????????)
    if (matched_tuple == false) return false;
  }
  return matched_tuple;
}

//??????nested loop join??????????????????????????????????????????
void join_two_table(const TupleSet &left_table, const TupleSet &right_table,
                    std::vector<Condition> &conditions,
                    TupleSet &joined_table) {
  LOG_DEBUG("left table has [%d] tuples", left_table.size());
  LOG_DEBUG("right table has [%d] tuples", right_table.size());

  const TupleSchema &left_tuple_schema = left_table.get_schema();
  const TupleSchema &right_tuple_schema = right_table.get_schema();
  TupleSchema merge_schema;
  merge_schema.append(left_tuple_schema);
  merge_schema.append(right_tuple_schema);

  LOG_INFO("conditions size is [%d]", conditions.size());
  for (int i = 0; i < left_table.size(); i++) {
    const Tuple &left_tuple = left_table.get(i);
    LOG_DEBUG("left tuple has [%d] tuple value", left_tuple.size());
    for (int j = 0; j < right_table.size(); j++) {
      const Tuple &right_tuple = right_table.get(j);
      LOG_DEBUG("right tuple has [%d] tuple value", right_tuple.size());
      //????????????conditions?????????join??????????????????tuple
      //?????????conditions???join???????????????????????????tuple
      if (conditions.size() == 0 ||
          tuple_matched_conditions(left_tuple, right_tuple, left_tuple_schema,
                                   right_tuple_schema, conditions)) {
        Tuple dst_tuple;
        merge_tuple(left_tuple, right_tuple, dst_tuple);
        LOG_DEBUG("merge tuple has [%d] tuple value", dst_tuple.size());
        joined_table.add(std::move(dst_tuple));
      }
    }
  }
  joined_table.set_schema(merge_schema);
  LOG_INFO("joined table size is [%d]", joined_table.size());
}

// add by cmk : ??????join
void join_multiple_table(std::vector<TupleSet> &tuple_sets,
                         const std::vector<Condition> &conditions,
                         TupleSet &joined_table) {
  if (tuple_sets.size() < 2) {
    LOG_INFO("no more than 2 tables, no need to join");
    return;
  }
  int end = tuple_sets.size() - 1;
  LOG_INFO("tuple sets number to be joined is [%d]", end + 1);
  //????????????join???????????????????????????????????????
  while (true) {
    std::vector<Condition> condition_with_two_table;
    TupleSchema left_table_schema = tuple_sets[end].get_schema();
    TupleSchema right_table_schema = tuple_sets[end - 1].get_schema();
    find_two_table_condition(left_table_schema, right_table_schema, conditions,
                             condition_with_two_table);

    if (tuple_sets.size() == 2) {
      //????????????join
      LOG_INFO("tuple sets number is 2 , just join two table");
      join_two_table(tuple_sets[end], tuple_sets[end - 1],
                     condition_with_two_table, joined_table);
      return;
    }
    TupleSet tmp_joined_table;
    join_two_table(tuple_sets[end], tuple_sets[end - 1],
                   condition_with_two_table, tmp_joined_table);

    tuple_sets.pop_back();
    tuple_sets.pop_back();
    // join???????????????????????????join
    tuple_sets.push_back(std::move(tmp_joined_table));
    end--;
  }
}