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
// Created by Longda on 2021/4/13.
//

#include <string>
#include <sstream>

#include "execute_stage.h"

#include "common/io/io.h"
#include "common/log/log.h"
#include "common/seda/timer_stage.h"
#include "common/lang/string.h"
#include "session/session.h"
#include "event/storage_event.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "event/execution_plan_event.h"
#include "sql/executor/execution_node.h"
#include "sql/executor/tuple.h"
#include "storage/common/table.h"
#include "storage/default/default_handler.h"
#include "storage/common/condition_filter.h"
#include "storage/trx/trx.h"

#include "group_exec.h"

using namespace common;

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);
RC create_selection_aggregation_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectAggregationExeNode &select_node);
RC aggr_execution(const Selects &selects, const char *db, TupleSet& tupleset_in, TupleSet& tupleset_out);

//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

//add by cmk(2021/10/26)
void find_conditions(const Selects& selects, std::vector<Condition>& conditions);

//add by cmk : 根据查询语句中指定的要查询的列，从joined table中选出指定列组成新表
void select_columns(const TupleSet& joined_table, const Selects& selects, TupleSet& joined_table_selected,bool is_single_table);

//add by cmk : 选出要查询的列名组成新的schema
RC select_column_names(const TupleSchema& old_schema, const Selects& selects, TupleSchema& new_schema,bool is_single_table);

//add by cmk : 多表join
void join_multiple_table(std::vector<TupleSet>& tuple_sets,const std::vector<Condition>& conditions,TupleSet& joined_table);

//! Parse properties, instantiate a stage object
Stage *ExecuteStage::make_stage(const std::string &tag)
{
    ExecuteStage *stage = new (std::nothrow) ExecuteStage(tag.c_str());
    if (stage == nullptr)
    {
        LOG_ERROR("new ExecuteStage failed");
        return nullptr;
    }
    stage->set_properties();
    return stage;
}

//! Set properties for this object set in stage specific properties
bool ExecuteStage::set_properties()
{
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
bool ExecuteStage::initialize()
{
    LOG_TRACE("Enter");

    std::list<Stage *>::iterator stgp = next_stage_list_.begin();
    default_storage_stage_ = *(stgp++);
    mem_storage_stage_ = *(stgp++);

    LOG_TRACE("Exit");
    return true;
}

//! Cleanup after disconnection
void ExecuteStage::cleanup()
{
    LOG_TRACE("Enter");

    LOG_TRACE("Exit");
}

void ExecuteStage::handle_event(StageEvent *event)
{
    LOG_TRACE("Enter\n");

    handle_request(event);

    LOG_TRACE("Exit\n");
    return;
}

void ExecuteStage::callback_event(StageEvent *event, CallbackContext *context)
{
    LOG_TRACE("Enter\n");

    // here finish read all data from disk or network, but do nothing here.
    ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
    SQLStageEvent *sql_event = exe_event->sql_event();
    sql_event->done_immediate();

    LOG_TRACE("Exit\n");
    return;
}

void ExecuteStage::handle_request(common::StageEvent *event)
{
    ExecutionPlanEvent *exe_event = static_cast<ExecutionPlanEvent *>(event);
    SessionEvent *session_event = exe_event->sql_event()->session_event();
    Query *sql = exe_event->sqls();
    const char *current_db = session_event->get_client()->session->get_current_db().c_str();

    CompletionCallback *cb = new (std::nothrow) CompletionCallback(this, nullptr);
    if (cb == nullptr)
    {
        LOG_ERROR("Failed to new callback for ExecutionPlanEvent");
        exe_event->done_immediate();
        return;
    }
    exe_event->push_callback(cb);

    switch (sql->flag)
    {
    case SCF_SELECT:
    { // select
        RC rc = do_select(current_db, sql, exe_event->sql_event()->session_event());
        if (rc != RC::SUCCESS)
        {
            session_event->set_response("FAILURE\n");
        }
        exe_event->done_immediate();
    }
    break;

    case SCF_INSERT:
    case SCF_UPDATE:
    case SCF_DELETE:
    case SCF_CREATE_TABLE:
    case SCF_SHOW_TABLES:
    case SCF_DESC_TABLE:
    case SCF_DROP_TABLE:
    case SCF_CREATE_INDEX:
    case SCF_DROP_INDEX:
    case SCF_LOAD_DATA:
    {
        StorageEvent *storage_event = new (std::nothrow) StorageEvent(exe_event);
        if (storage_event == nullptr)
        {
            LOG_ERROR("Failed to new StorageEvent");
            event->done_immediate();
            return;
        }

        default_storage_stage_->handle_event(storage_event);
    }
    break;
    case SCF_SYNC:
    {
        RC rc = DefaultHandler::get_default().sync();
        session_event->set_response(strrc(rc));
        exe_event->done_immediate();
    }
    break;
    case SCF_BEGIN:
    {
        session_event->get_client()->session->set_trx_multi_operation_mode(true);
        session_event->set_response(strrc(RC::SUCCESS));
        exe_event->done_immediate();
    }
    break;
    case SCF_COMMIT:
    {
        Trx *trx = session_event->get_client()->session->current_trx();
        RC rc = trx->commit();
        session_event->get_client()->session->set_trx_multi_operation_mode(false);
        session_event->set_response(strrc(rc));
        exe_event->done_immediate();
    }
    break;
    case SCF_ROLLBACK:
    {
        Trx *trx = session_event->get_client()->session->current_trx();
        RC rc = trx->rollback();
        session_event->get_client()->session->set_trx_multi_operation_mode(false);
        session_event->set_response(strrc(rc));
        exe_event->done_immediate();
    }
    break;
    case SCF_HELP:
    {
        const char *response = "show tables;\n"
                               "desc `table name`;\n"
                               "create table `table name` (`column name` `column type`, ...);\n"
                               "create index `index name` on `table` (`column`);\n"
                               "insert into `table` values(`value1`,`value2`);\n"
                               "update `table` set column=value [where `column`=`value`];\n"
                               "delete from `table` [where `column`=`value`];\n"
                               "select [ * | `columns` ] from `table`;\n";
        session_event->set_response(response);
        exe_event->done_immediate();
    }
    break;
    case SCF_EXIT:
    {
        // do nothing
        const char *response = "Unsupported\n";
        session_event->set_response(response);
        exe_event->done_immediate();
    }
    break;
    default:
    {
        exe_event->done_immediate();
        LOG_ERROR("Unsupported command=%d\n", sql->flag);
    }
    }
}

void end_trx_if_need(Session *session, Trx *trx, bool all_right)
{
    if (!session->is_trx_multi_operation_mode())
    {
        if (all_right)
        {
            trx->commit();
        }
        else
        {
            trx->rollback();
        }
    }
}

// 这里没有对输入的某些信息做合法性校验，比如查询的列名、where条件中的列名等，没有做必要的合法性校验
// 需要补充上这一部分. 校验部分也可以放在resolve，不过跟execution放一起也没有关系
RC ExecuteStage::do_select(const char *db, Query *sql, SessionEvent *session_event)
{

    RC rc = RC::SUCCESS;
    Session *session = session_event->get_client()->session;
    Trx *trx = session->current_trx();
    const Selects &selects = sql->sstr.selection;
    // 对order进行元信息校验
    for (int i = 0; i < selects.order_num; i++)
    {
        char *relation_name = selects.order_attr[i].relation_name;
        char *attribute_name = selects.order_attr[i].attribute_name;
        if(relation_name != nullptr){
            Table *table = DefaultHandler::get_default().find_table(db, relation_name);
            if (nullptr == table)
            {
                LOG_WARN("No such table [%s] in db [%s]", relation_name, db);
                return RC::SCHEMA_TABLE_NOT_EXIST;
            }
            // 判断该表名是否在搜索的表中
            bool isExistInSelectTable = false;
            for (int i = 0; i < selects.relation_num; i++)
            {
                if(strcmp(selects.relations[i],relation_name) ==0 ) {
                    isExistInSelectTable = true;
                    break;
                }
            }
            // 如果要排序的表的字段 该表不在select的relation中的话 return false
            if(!isExistInSelectTable) {
                return RC::GENERIC_ERROR;
            }

            // 如果存在则判断该表有没有这个attribute
            const FieldMeta *field  = table->table_meta().field(attribute_name);
            if(field == nullptr) {
                return RC::SCHEMA_FIELD_NOT_EXIST;
            }
        } else {
            // 如果没有指定表名 且不是单表 就返回出错
            if (selects.relation_num != 1) {
                return RC::GENERIC_ERROR;
            }
            // 如果是单表 则判断先检查该表是否存在
            relation_name =  selects.relations[0];
            Table *table = DefaultHandler::get_default().find_table(db, relation_name);
            if (nullptr == table)
            {
                LOG_WARN("No such table [%s] in db [%s]", relation_name, db);
                return RC::SCHEMA_TABLE_NOT_EXIST;
            }

            // 如果存在则判断该表有没有这个attribute
            const FieldMeta *field  = table->table_meta().field(attribute_name);
            if(field == nullptr) {
                return RC::SCHEMA_FIELD_NOT_EXIST;
            }
        }
    }
    

    // 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
    std::vector<ExecutionNode *> select_nodes;
    for (size_t i = 0; i < selects.relation_num; i++)
    {
        const char *table_name = selects.relations[i];
        ExecutionNode* select_node;
        // if (false) {
        //     select_node = new SelectAggregationExeNode;
        //     SelectAggregationExeNode* select_node_in = dynamic_cast<SelectAggregationExeNode*>(select_node);
        //     rc = create_selection_aggregation_executor(trx, selects, db, table_name, *select_node_in);
        // } else {
        //     select_node = new SelectExeNode;
        //     SelectExeNode* select_node_in = dynamic_cast<SelectExeNode*>(select_node);
        //     rc = create_selection_executor(trx, selects, db, table_name, *select_node_in);
        // }
        select_node = new SelectExeNode;
        SelectExeNode* select_node_in = dynamic_cast<SelectExeNode*>(select_node);
        rc = create_selection_executor(trx, selects, db, table_name, *select_node_in);
        if (rc != RC::SUCCESS)
        {
            delete select_node;
            for (ExecutionNode *&tmp_node : select_nodes)
            {
                delete tmp_node;
            }
            end_trx_if_need(session, trx, false);
            return rc;
        }
        select_nodes.push_back(select_node);
    }

    if (select_nodes.empty())
    {
        LOG_ERROR("No table given");
        end_trx_if_need(session, trx, false);
        return RC::SQL_SYNTAX;
    }

    //add by cmk (2021/10/26)
    std::vector<TupleSet> tuple_sets;

    for (ExecutionNode *&node : select_nodes)
    {
        TupleSet tuple_set;
        rc = node->execute(tuple_set);
        if (rc != RC::SUCCESS)
        {
            for (ExecutionNode *&tmp_node : select_nodes)
            {
                delete tmp_node;
            }
            end_trx_if_need(session, trx, false);
            return rc;
        }
        else
        {
            tuple_sets.push_back(std::move(tuple_set));
        }
    }

    std::stringstream ss;
    if (tuple_sets.size() > 1)
    {
        // 本次查询了多张表，需要做join操作
        bool is_single_table = false;
        bool exist_empty_table = false;
        LOG_INFO("tuple sets size is %d", tuple_sets.size());
        TupleSchema all_tables_schemas;
        for(auto iter = tuple_sets.rbegin() ; iter != tuple_sets.rend(); iter++){
            all_tables_schemas.append((*iter).get_schema());
            if((*iter).size() == 0){
                exist_empty_table = true;
            }
        }
        //选出要打印的列名
        //如果有空表，则只打印列名
        if(exist_empty_table){
            LOG_INFO("exist empty table,print fields only");
            TupleSchema selected_schema;
            LOG_INFO("selects attr_num is [%d], attr[0] is [%s]", selects.attr_num, selects.attributes[0].attribute_name);
            if(RC::SUCCESS != select_column_names(all_tables_schemas, selects, selected_schema,is_single_table)){
                return RC::GENERIC_ERROR;
            }
            selected_schema.print(ss,is_single_table);
        }else{
            //多表join
            LOG_INFO("multiple table joining");
            TupleSet joined_table;
            std::vector<Condition> conditions;
            find_conditions(selects, conditions);
            join_multiple_table(tuple_sets, conditions, joined_table);

            LOG_DEBUG("multiple table joined, joined table size is [%d]", joined_table.size());

            //select需要的列
            
            if (selects.group_num > 0) {
                // 1. 分解为多个TupleSet
                std::vector<TupleSet> tuple_sets_out;
                rc = group_decompose(joined_table, tuple_sets_out, selects, db);
                if (rc != RC::SUCCESS) {
                    return rc;
                }
                std::vector<TupleSet> edited_sets;
                if (selects.aggr_num > 0) {
                    // 2. 分别进行聚合
                    rc = group_aggr(tuple_sets_out, edited_sets, selects, db);
                    if (rc != RC::SUCCESS) {
                        return rc;
                    }
                } else {
                    // 3. 否则TupleSet内部去重只剩一行
                    // 4. 并且只选择select的元素
                    rc = group_de_duplication(tuple_sets_out, edited_sets, selects, db);
                    if (rc != RC::SUCCESS) {
                        return rc;
                    }
                }
                // 5. 合并所有TupleSet
                // 6. 打印TupleSet
                TupleSet final_tupleset;
                rc = group_compose(edited_sets, final_tupleset);
                final_tupleset.print(ss, is_single_table);
            } else {
                TupleSet joined_table_selected;
                if (selects.aggr_num > 0) {
                    aggr_execution(selects, db, joined_table, joined_table_selected);
                    joined_table_selected.print(ss, is_single_table);
                } else {
                    select_columns(joined_table, selects, joined_table_selected,is_single_table);
                    if(selects.order_num > 0) {
                        joined_table_selected.sort(is_single_table, selects.order_attr, selects.order_num);
                    }
                    joined_table_selected.print(ss, is_single_table);
                }
            }
        }
    }
    else
    {
        bool is_single_table = true;
        
        if (selects.group_num > 0) {
            // 1. 分解为多个TupleSet
            std::vector<TupleSet> tuple_sets_out;
            rc = group_decompose(tuple_sets[0], tuple_sets_out, selects, db);
            if (rc != RC::SUCCESS) {
                return rc;
            }
            std::vector<TupleSet> edited_sets;
            if (selects.aggr_num > 0) {
                // 2. 分别进行聚合
                rc = group_aggr(tuple_sets_out, edited_sets, selects, db);
                if (rc != RC::SUCCESS) {
                    return rc;
                }
            } else {
                // 3. 否则TupleSet内部去重只剩一行
                // 4. 并且只选择select的元素
                rc = group_de_duplication(tuple_sets_out, edited_sets, selects, db);
                if (rc != RC::SUCCESS) {
                    return rc;
                }
            }
            // 5. 合并所有TupleSet
            // 6. 打印TupleSet
            TupleSet final_tupleset;
            rc = group_compose(edited_sets, final_tupleset);
            final_tupleset.print(ss, is_single_table);
        } else {
            // 当前只查询一张表，直接返回结果即可
            TupleSet table_selected;
            if (selects.aggr_num > 0) {
            aggr_execution(selects, db, tuple_sets[0], table_selected);
            table_selected.print(ss, is_single_table);
            } else {
                select_columns(tuple_sets[0], selects, table_selected, is_single_table);
                // 先排序
                if(selects.order_num > 0) {
                    table_selected.sort(is_single_table, selects.order_attr, selects.order_num);
                }

                table_selected.print(ss, is_single_table);
            }
        }
    }

    for (ExecutionNode *&tmp_node : select_nodes)
    {
        delete tmp_node;
    }
    session_event->set_response(ss.str());
    end_trx_if_need(session, trx, true);
    return rc;
}

bool match_table(const Selects &selects, const char *table_name_in_condition, const char *table_name_to_match)
{
    if (table_name_in_condition != nullptr)
    {
        return 0 == strcmp(table_name_in_condition, table_name_to_match);
    }

    return selects.relation_num == 1;
}

static RC schema_add_field(Table *table, const char *field_name, TupleSchema &schema)
{
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta)
    {
        LOG_WARN("No such field. %s.%s", table->name(), field_name);
        return RC::SCHEMA_FIELD_MISSING;
    }

    schema.add_if_not_exists(field_meta->type(), table->name(), field_meta->name());
    return RC::SUCCESS;
}

static RC schema_add_aggr_field(const AggrAttr* aggr, Table *table, const char *field_name, TupleSchema &schema)
{   
    if (aggr->is_constant) {
        schema.add_aggr(aggr->aggr_type, AttrType::UNDEFINED, table->name(), table, field_name, &aggr->constant_value);
        return RC::SUCCESS;
    }
    if (0 == strcmp("*", field_name)) {
        if (aggr->aggr_type == AggrType::COUNT) {
            schema.add_aggr(aggr->aggr_type, AttrType::INTS, table->name(), table, field_name, nullptr);
            return RC::SUCCESS;
        } else {
            LOG_DEBUG("AVG MIN MAX not support * field");
            return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
    }
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta)
    {
        LOG_WARN("No such field in aggregation. %s.%s", table->name(), field_name);
        return RC::SCHEMA_FIELD_MISSING;
    }
    switch(aggr->aggr_type) {
        case AggrType::COUNT:
            schema.add_aggr(aggr->aggr_type, AttrType::INTS, table->name(), table, field_meta->name(), nullptr);
            break;
        case AggrType::AVG:
            if (field_meta->type() != AttrType::INTS && field_meta->type() != AttrType::FLOATS) {
                LOG_WARN("Field type in aggregation not support aggregation op of avg. %s.%s", table->name(), field_meta->name());
                return RC::SCHEMA_FIELD_TYPE_MISMATCH;
            }
            schema.add_aggr(aggr->aggr_type, AttrType::FLOATS, table->name(), table, field_meta->name(), nullptr);
            break;
        case AggrType::MAX:
        case AggrType::MIN:
            if (field_meta->type() == AttrType::UNDEFINED) {
                // Min Max 支持所有类型，除了UNDEFINED
                LOG_WARN("Field type in aggregation not support aggregation op of max, min. %s.%s", table->name(), field_meta->name());
                return RC::SCHEMA_FIELD_TYPE_MISMATCH;
            }
            schema.add_aggr(aggr->aggr_type, field_meta->type(), table->name(), table, field_meta->name(), nullptr);
            break;
        default:
            LOG_DEBUG("Not Support AGGR_UNDEGINED.");
            return RC::GENERIC_ERROR;
            break;
    }
    return RC::SUCCESS;
}

RC aggr_execution(const Selects &selects, const char *db, TupleSet& tupleset_in, TupleSet& tupleset_out) {
    RC rc = RC::SUCCESS;
    TupleSchema schema;
    std::vector<const AggrAttr *> attrs;
    std::vector<GroupAttr> groupattrs_compare;
    if (selects.group_num > 0) {
        for (int i = selects.group_num - 1; i >= 0; i--) {
            // 反向遍历以与输入顺序对齐
            GroupAttr group_attr(selects.group_attr[i]);
            Table *table;
            if (group_attr.relation_name == nullptr) {
                if (selects.relation_num > 1) {
                    LOG_WARN("Aggr must have table name in multi table, field [%s]", group_attr.attribute_name);
                    return RC::SCHEMA_TABLE_NOT_EXIST; 
                } else {
                    group_attr.relation_name = selects.relations[0];
                    table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
                }
            } else {
                table = DefaultHandler::get_default().find_table(db, group_attr.relation_name);
            }
            if (nullptr == table)
            {
                LOG_WARN("No such table [%s] in db [%s]", group_attr.relation_name, db);
                return RC::SCHEMA_TABLE_NOT_EXIST;
            }
            const FieldMeta *field_meta = table->table_meta().field(group_attr.attribute_name);
            if (nullptr == field_meta)
            {
                LOG_WARN("No such field in group. %s.%s", table->name(), group_attr.attribute_name);
                return RC::SCHEMA_FIELD_MISSING;
            }
            groupattrs_compare.push_back(group_attr);
        }   
    }
    if (selects.aggr_num > 0) {
        for (int i = 0; i < selects.aggr_num; i++) {
            const AggrAttr &attr = selects.aggr_attr[i];
            char* rel_comp;
            char* attr_comp;
            Table *table;
            if (attr.relation_name == nullptr) {
                if (selects.relation_num > 1) {
                    LOG_WARN("Aggr must have table name in multi table, field [%s]", attr.attribute_name);
                    return RC::SCHEMA_TABLE_NOT_EXIST; 
                } else {
                    rel_comp = selects.relations[0];
                    table = DefaultHandler::get_default().find_table(db, selects.relations[0]);
                }
            } else {
                rel_comp = attr.relation_name;
                table = DefaultHandler::get_default().find_table(db, attr.relation_name);
            }
            if (nullptr == table)
            {
                LOG_WARN("No such table [%s] in db [%s]", attr.relation_name, db);
                return RC::SCHEMA_TABLE_NOT_EXIST;
            }
            const FieldMeta *field_meta = table->table_meta().field(attr.attribute_name);
            if (nullptr == field_meta)
            {
                LOG_WARN("No such field in aggr. %s.%s", table->name(), attr.attribute_name);
                return RC::SCHEMA_FIELD_MISSING;
            }
            attr_comp = attr.attribute_name;
            attrs.push_back(&attr);
            schema_add_aggr_field(&attr, table, attr.attribute_name, schema);
        }
        tupleset_out.set_schema(schema);
    }
    AggregationRecordConverter converter(tupleset_out, attrs, &tupleset_in.schema());
    for (int i=0; i < tupleset_in.size(); i++) {
        converter.read_tuple(tupleset_in.get(i));
    }
    rc = converter.final_add_record();
    return rc;
}

RC create_selection_aggregation_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectAggregationExeNode &select_node) {
    // 列出跟这张表关联的Attr
    TupleSchema schema;
    Table *table = DefaultHandler::get_default().find_table(db, table_name);
    if (nullptr == table)
    {
        LOG_WARN("No such table [%s] in db [%s]", table_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    const TableMeta &table_meta = table->table_meta();

    if (selects.aggr_num > 0) {
        for (int i = selects.aggr_num - 1; i >= 0; i--) {
            // 反向遍历以与输入顺序对齐
            const AggrAttr &attr = selects.aggr_attr[i];
            if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name)) {
                select_node.add_aggr_attr(&attr);
                RC rc = schema_add_aggr_field(&attr, table, attr.attribute_name, schema);
                if (rc != RC::SUCCESS)
                {
                    return rc;
                }
                // schema.add_aggr(attr.aggr_type, field->type(), table_name, attr.attribute_name);
            } else {
                return RC::SCHEMA_TABLE_NAME_ILLEGAL;
            }
        }
    }
    // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
    std::vector<DefaultConditionFilter *> condition_filters;
    for (size_t i = 0; i < selects.condition_num; i++)
    {
        const Condition &condition = selects.conditions[i];
        if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) ||                                                                         // 两边都是值
            (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
            (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) || // 左边是值，右边是属性名
            (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
             match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
        )
        {
            DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
            RC rc = condition_filter->init(*table, condition);
            if (rc != RC::SUCCESS)
            {
                delete condition_filter;
                for (DefaultConditionFilter *&filter : condition_filters)
                {
                    delete filter;
                }
                return rc;
            }
            condition_filters.push_back(condition_filter);
        }
    }
    return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}

// 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node)
{
    // 列出跟这张表关联的Attr
    TupleSchema schema;
    Table *table = DefaultHandler::get_default().find_table(db, table_name);
    if (nullptr == table)
    {
        LOG_WARN("No such table [%s] in db [%s]", table_name, db);
        return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    // for (int i = selects.attr_num - 1; i >= 0; i--)
    // {
    //     const RelAttr &attr = selects.attributes[i];
    //     if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name))
    //     {
    //         if (0 == strcmp("*", attr.attribute_name))
    //         {
    //             // 列出这张表所有字段
    //             TupleSchema::from_table(table, schema);
    //             break; // 没有校验，给出* 之后，再写字段的错误
    //         }
    //         else
    //         {
    //             // 列出这张表相关字段
    //             RC rc = schema_add_field(table, attr.attribute_name, schema);
    //             if (rc != RC::SUCCESS)
    //             {
    //                 return rc;
    //             }
    //         }
    //     }
    // }

    // 找出仅与此表相关的过滤条件, 或者都是值的过滤条件
    std::vector<DefaultConditionFilter *> condition_filters;
    for (size_t i = 0; i < selects.condition_num; i++)
    {
        const Condition &condition = selects.conditions[i];
        if ((condition.left_is_attr == 0 && condition.right_is_attr == 0) ||                                                                         // 两边都是值
            (condition.left_is_attr == 1 && condition.right_is_attr == 0 && match_table(selects, condition.left_attr.relation_name, table_name)) ||  // 左边是属性右边是值
            (condition.left_is_attr == 0 && condition.right_is_attr == 1 && match_table(selects, condition.right_attr.relation_name, table_name)) || // 左边是值，右边是属性名
            (condition.left_is_attr == 1 && condition.right_is_attr == 1 &&
             match_table(selects, condition.left_attr.relation_name, table_name) && match_table(selects, condition.right_attr.relation_name, table_name)) // 左右都是属性名，并且表名都符合
        )
        {
            DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
            RC rc = condition_filter->init(*table, condition);
            if (rc != RC::SUCCESS)
            {
                delete condition_filter;
                for (DefaultConditionFilter *&filter : condition_filters)
                {
                    delete filter;
                }
                return rc;
            }
            condition_filters.push_back(condition_filter);
        }

        //add by cmk : 如果需要作多表查询时，有和多表相关的条件，则仍然将每个表的所有字段都列出
        // if(condition.left_is_attr == 1 && condition.right_is_attr == 1 && 
        //         (0 != strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name)))
        // {
        //     // 列出这张表所有字段
        //     LOG_INFO("multi join conditions , still list all attr of one table");
        //     schema.clear();
        //     TupleSchema::from_table(table, schema);
        // }
    }

    TupleSchema::from_table(table, schema);
    return select_node.init(trx, table, std::move(schema), std::move(condition_filters));
}


//add by cmk : 选出要查询的列名组成新的schema
RC select_column_names(const TupleSchema& old_schema, const Selects& selects, TupleSchema& new_schema, bool is_single_table)
{
    const std::vector<TupleField> fields = old_schema.fields();
    if(fields.size() <= 0 ){
        LOG_INFO("old_schema does not have fields");
        return RC::GENERIC_ERROR;
    }
    if(selects.attr_num <= 0){
        LOG_INFO("selects dose not have attributes");
        return RC::GENERIC_ERROR;
    }
     //列出表的相关字段
    for(int j = (selects.attr_num - 1) ; j >= 0 ; j --){
        const RelAttr& attr = selects.attributes[j];
        LOG_INFO("attr table name is [%s], field name is [%s]",attr.relation_name, attr.attribute_name);
        //多表查询时，不允许condition里的表名为空
        if(!is_single_table && (0 != strcmp(attr.attribute_name, "*")) && attr.relation_name == nullptr ){
            return RC::GENERIC_ERROR;
        }
        for(auto field : fields){
            if(!is_single_table){ //多表
                if((0 == strcmp(attr.attribute_name, "*")) || 
                    ((0 == strcmp(field.table_name(), attr.relation_name)) && (0 == strcmp(field.field_name(), attr.attribute_name)) ) ){
                    LOG_DEBUG("select_columns_name() multi table: new_schema table name is [%s],field name is [%s]",field.table_name(),field.field_name());
                    new_schema.add_if_not_exists(field.type(), field.table_name(), field.field_name());
                }
            }else{ //单表
                LOG_DEBUG("SINGLE");
                if (nullptr ==  attr.relation_name) {
                    if (0 == strcmp(field.field_name(), attr.attribute_name) || (0 == strcmp(attr.attribute_name, "*"))) {
                        LOG_DEBUG("select_columns_name() single table: new_schema table name is [%s],field name is [%s]",field.table_name(),field.field_name());
                        new_schema.add_if_not_exists(field.type(), field.table_name(), field.field_name());
                    }
                } else {
                    if(((0 == strcmp(field.table_name(), attr.relation_name)) && (0 == strcmp(field.field_name(), attr.attribute_name)))){
                        LOG_DEBUG("select_columns_name() single table: new_schema table name is [%s],field name is [%s]",field.table_name(),field.field_name());
                        new_schema.add_if_not_exists(field.type(), field.table_name(), field.field_name());
                    }
                }
            }
        }
    }
    if(new_schema.fields().size() <=0 ){
        LOG_INFO("select column names failed");
        return RC::GENERIC_ERROR;
    }
    return RC::SUCCESS;

}

//add by cmk : 根据查询语句中指定的要查询的列，从joined table中选出指定列组成新表
void select_columns(const TupleSet& joined_table, const Selects& selects, TupleSet& joined_table_selected,bool is_single_table)
{
    if(joined_table.size() == 0){
        LOG_WARN("src table size is 0");
    }
    const TupleSchema& joined_table_schema = joined_table.schema();
    const std::vector<TupleField>& joined_table_fields = joined_table_schema.fields();
    LOG_DEBUG("select columns() :joined table schema size is [%d]", joined_table_fields.size());
    TupleSchema schema;
    //列出表的相关字段
    if(RC::SUCCESS !=  select_column_names(joined_table_schema, selects, schema, is_single_table)){
        LOG_INFO("select columns names failed");
    }
    LOG_DEBUG("select columns() :selected schema size is [%d]", schema.fields().size());
    const std::vector<TupleField>& selected_table_fields = schema.fields();
    for(int i = 0; i < joined_table.size(); i++)
    {
        //根据表的相关字段选出列
        const Tuple& tuple = joined_table.get(i);
        Tuple select_tuple;
        for(auto field : selected_table_fields){
            int value_index = joined_table_schema.index_of_field(field.table_name(), field.field_name());
            auto& value = tuple.get_pointer(value_index);
            select_tuple.add(value);
        }
        joined_table_selected.add(std::move(select_tuple));
        
    }
    joined_table_selected.set_schema(schema);
    LOG_DEBUG("joined_table_selected size is [%d]", joined_table_selected.size());
}

//add by cmk(2021/10/24) : 找出和多个表相关的过滤条件
void find_conditions(const Selects& selects, std::vector<Condition>& conditions)
{
    for(size_t i = 0; i < selects.condition_num; i++){
        const Condition& condition = selects.conditions[i];
        if(condition.left_is_attr == 1 && condition.right_is_attr == 1 && 
                (0 != strcmp(condition.left_attr.relation_name, condition.right_attr.relation_name))){
            conditions.push_back(condition);
        }
    }
}
//add by cmk : 合并两个Tuple
void merge_tuple(const Tuple& left_tuple, const Tuple& right_tuple, Tuple& dst_tuple)
{
    for(int i =0 ; i < left_tuple.size(); i++){
        dst_tuple.add(left_tuple.get_pointer(i));
    }

    for(int j =0 ; j < right_tuple.size(); j++){
        dst_tuple.add(right_tuple.get_pointer(j));
    }
    return;
}

void strexchg(char **a, char **b){
    char *c;
    c=*a; 
    *a=*b;
    *b=c;
}
//判断一个join过的TupleSchema里是否有某个表名
bool exist_tabl_name(const TupleSchema& schema, const char* table_name)
{
    std::vector<TupleField> fields = schema.fields();
    for(int i = 0; i < fields.size(); i++){
        if((0 == strcmp(fields[i].table_name(), table_name))){
            return true;
        }
    }
    return false;
}

//add by cmk : 找到和两个表有关的conditions
//(注意在3表以上的多表查询时，TupleSchema里可能有多个表名)
void find_two_table_condition(const TupleSchema& left_table_schema, 
                                const TupleSchema& right_table_schema, 
                                    const std::vector<Condition>& conditions,
                                     std::vector<Condition>& cond_with_two_table )
{

    for(auto condition : conditions){
        if (((true == exist_tabl_name(left_table_schema,condition.left_attr.relation_name)) && 
            (true == exist_tabl_name(right_table_schema, condition.right_attr.relation_name)))){
                cond_with_two_table.push_back(condition);
        }
        //如果查询的条件里的列名和表的列名顺序相反
        if(((true == exist_tabl_name(left_table_schema, condition.right_attr.relation_name)) && 
            (true == exist_tabl_name(right_table_schema, condition.left_attr.relation_name)))){
            //切换条件的顺序
            LOG_DEBUG("find_two_table_condition : before ex condition.right_attr.relation_name is [%s]",condition.right_attr.relation_name);
            LOG_DEBUG("find_two_table_condition : before ex condition.left_attr.relation_name is [%s]",condition.left_attr.relation_name);
            strexchg(&condition.left_attr.relation_name, &condition.right_attr.relation_name);
            strexchg(&condition.left_attr.attribute_name, &condition.right_attr.attribute_name);

            LOG_DEBUG("find_two_table_condition : after ex condition.right_attr.relation_name is [%s]",condition.right_attr.relation_name);
            LOG_DEBUG("find_two_table_condition : after ex condition.left_attr.relation_name is [%s]",condition.left_attr.relation_name);
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

//add by cmk(2021/10/24): 判断两个Tuple是否满足某些conditions
 bool tuple_matched_conditions(const Tuple& left_tuple,
                                const Tuple& right_tuple,
                                    const TupleSchema& left_table_schema,
                                        const TupleSchema& right_table_schema,
                                            const std::vector<Condition>& conditions)
{   
    bool matched_tuple = false; 
    for(auto condition : conditions){
        int tuple_value_left_index = 
            left_table_schema.index_of_field(condition.left_attr.relation_name, condition.left_attr.attribute_name);
        int tuple_value_right_index = 
            right_table_schema.index_of_field(condition.right_attr.relation_name, condition.right_attr.attribute_name);
        auto& left = left_tuple.get(tuple_value_left_index);
        auto& right = right_tuple.get(tuple_value_right_index);

        double cmp_result = 0;
        cmp_result = left.compare(right);
        LOG_INFO("CMP_RESULT is [%d]",cmp_result);
        ValueType left_attr_type = left.type();
        ValueType right_attr_type = right.type();

        if (left_attr_type != right_attr_type) {
            if (left_attr_type != ValueType::NULLTYPE && right_attr_type != ValueType::NULLTYPE) {
                if (!((left_attr_type == ValueType::INT && right_attr_type == ValueType::FLOAT) || (left_attr_type == ValueType::FLOAT && right_attr_type == ValueType::INT))) {
                    LOG_DEBUG("Type left not the same as type right");
                    return false;
                }
            }
        }
        switch (left_attr_type) {
            case ValueType::STRING: {  // 字符串都是定长的，直接比较
                // 按照C字符串风格来定
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    break;
                }
                cmp_result = left.compare(right);
            } 
            break;
            // DATES 底层于INTS采取相同存储方式
            case ValueType::DATE:
            case ValueType::INT: {
                // 没有考虑大小端问题
                // 对int和float，要考虑字节对齐问题,有些平台下直接转换可能会跪
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    break;
                }
                const IntValue& int_left = (const IntValue&)left;
                double left_in = int_left.value();
                double right_in;
                if (right_attr_type == ValueType::INT) {
                    const IntValue& int_right = (const IntValue&)right;
                    right_in = int_right.value();
                } else if(right_attr_type == ValueType::DATE) {
                    const DateValue& date_right = (const DateValue&)right;
                    right_in = date_right.value();
                } 
                else {
                    const FloatValue& date_right = (const FloatValue&)right;
                    right_in = date_right.value();
                }
                cmp_result = left_in - right_in;
            }
            break;
            case FLOATS: {
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    break;
                }
                const FloatValue& float_left = (const FloatValue&)left;
                double left_in = float_left.value();
                double right_in;
                if (right_attr_type == ValueType::INT) {
                    const IntValue& int_right = (const IntValue&)right;
                    right_in = int_right.value();
                } else if(right_attr_type == ValueType::DATE) {
                    const DateValue& date_right = (const DateValue&)right;
                    right_in = date_right.value();
                } 
                else {
                    const FloatValue& date_right = (const FloatValue&)right;
                    right_in = date_right.value();
                }
                cmp_result = left_in - right_in;
            } 
            break;
            default: {
            }
        }
        switch (condition.comp) {
            case EQUAL_TO:
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    matched_tuple = false;
                    break;
                }
                matched_tuple = (0 == cmp_result);
                break;
            case LESS_EQUAL:
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    matched_tuple = false;
                    break;
                }
                matched_tuple = (cmp_result <= 0);
                break;
            case NOT_EQUAL:
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    matched_tuple = false;
                    break;
                }
                matched_tuple = (cmp_result != 0);
                break;
            case LESS_THAN:
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    matched_tuple = false;
                    break;
                }
                matched_tuple = (cmp_result < 0);
                break;
            case GREAT_EQUAL:
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    matched_tuple = false;
                    break;
                }
                matched_tuple = (cmp_result >= 0);
                break;
            case GREAT_THAN:
                if (NULLTYPE == left_attr_type || NULLTYPE == right_attr_type ) {
                    matched_tuple = false;
                    break;
                }
                matched_tuple = (cmp_result > 0);
                break;
            case IS:
                if (NULLTYPE == left_attr_type && NULLTYPE == right_attr_type ) {
                    matched_tuple = true;
                } else {
                    matched_tuple = false;
                }
                break;
            case ISNOT:
                if ((NULLTYPE != left_attr_type && NULLTYPE == right_attr_type) || (NULLTYPE == left_attr_type && NULLTYPE != right_attr_type)) {
                    matched_tuple = true;
                } else {
                    matched_tuple = false;
                }
                break;
            default:
                matched_tuple = false;
                break;
        }
        //目前只有And的条件判断(其中一个条件不满足则认为不满足)
        if(matched_tuple == false) return false;
    }
    return matched_tuple;

}

//使用nested loop join（后续需要使用其他算法优化）
void join_two_table(const TupleSet& left_table, const TupleSet& right_table, std::vector<Condition>& conditions, TupleSet& joined_table)
{
    LOG_DEBUG("left table has [%d] tuples",left_table.size());
    LOG_DEBUG("right table has [%d] tuples",right_table.size());

    const TupleSchema& left_tuple_schema = left_table.get_schema();
    const TupleSchema& right_tuple_schema = right_table.get_schema();
    TupleSchema merge_schema;
    merge_schema.append(left_tuple_schema);
    merge_schema.append(right_tuple_schema);

    LOG_INFO("conditions size is [%d]",conditions.size());
    for(int i = 0 ; i < left_table.size(); i++){
        const Tuple& left_tuple = left_table.get(i);
        LOG_DEBUG("left tuple has [%d] tuple value",left_tuple.size());
        for(int j = 0; j < right_table.size(); j++){
            const Tuple& right_tuple = right_table.get(j);
            LOG_DEBUG("right tuple has [%d] tuple value",right_tuple.size());
            //如果没有conditions，直接join两张表所有的tuple
            //如果有conditions，join两张表中满足条件的tuple
            if(conditions.size() == 0 
                    || tuple_matched_conditions(left_tuple, right_tuple, left_tuple_schema, right_tuple_schema, conditions)){
                Tuple dst_tuple;
                merge_tuple(left_tuple, right_tuple, dst_tuple);
                LOG_DEBUG("merge tuple has [%d] tuple value",dst_tuple.size());
                joined_table.add(std::move(dst_tuple));
            }
        }
    }
    joined_table.set_schema(merge_schema);
    LOG_INFO("joined table size is [%d]",joined_table.size());
}

//add by cmk : 多表join
void join_multiple_table(std::vector<TupleSet>& tuple_sets,const std::vector<Condition>& conditions,TupleSet& joined_table)
{
    if(tuple_sets.size() < 2){
        LOG_INFO("no more than 2 tables, no need to join");
        return;
    }
    int end = tuple_sets.size() - 1;
    LOG_INFO("tuple sets number to be joined is [%d]",end+1);
    //每次从待join的所有表中取出相邻的两个表
    while(true){
        std::vector<Condition> condition_with_two_table;
        TupleSchema left_table_schema = tuple_sets[end].get_schema();
        TupleSchema right_table_schema = tuple_sets[end-1].get_schema();
        find_two_table_condition(left_table_schema, right_table_schema, conditions, condition_with_two_table);

        if(tuple_sets.size() == 2){
            //最后一次join
            LOG_INFO("tuple sets number is 2 , just join two table");
            join_two_table(tuple_sets[end], tuple_sets[end-1], condition_with_two_table, joined_table);
            return;
        }
        TupleSet tmp_joined_table;
        join_two_table(tuple_sets[end], tuple_sets[end-1], condition_with_two_table, tmp_joined_table);

        tuple_sets.pop_back();
        tuple_sets.pop_back();
        //join后的表继续和其他表join
        tuple_sets.push_back(std::move(tmp_joined_table));
        end--;
    }
}