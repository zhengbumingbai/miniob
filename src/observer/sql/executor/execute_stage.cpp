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
#include "util/cartesian_product.h"

using namespace common;

RC create_selection_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectExeNode &select_node);
RC create_selection_aggregation_executor(Trx *trx, const Selects &selects, const char *db, const char *table_name, SelectAggregationExeNode &select_node);

//以下三个函数(for 多表查询): add by cmk
void tupleset_cond_filt(const TupleSet& src_tupleset, const std::vector<Condition>& conditions ,TupleSet& dst_tupleset);

bool tuple_matched_conditions(const Tuple& src_tuple, 
                                    const TupleSchema& src_tuple_schema,
                                    const std::vector<Condition>& conditions);

void find_conditions(const Selects& selects, std::vector<Condition>& conditions);
//! Constructor
ExecuteStage::ExecuteStage(const char *tag) : Stage(tag) {}

//! Destructor
ExecuteStage::~ExecuteStage() {}

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



    // 把所有的表和只跟这张表关联的condition都拿出来，生成最底层的select 执行节点
    std::vector<ExecutionNode *> select_nodes;
    for (size_t i = 0; i < selects.relation_num; i++)
    {
        const char *table_name = selects.relations[i];
        ExecutionNode* select_node;
        if (selects.aggr_num > 0) {
            select_node = new SelectAggregationExeNode;
            SelectAggregationExeNode* select_node_in = dynamic_cast<SelectAggregationExeNode*>(select_node);
            rc = create_selection_aggregation_executor(trx, selects, db, table_name, *select_node_in);
        } else {
            select_node = new SelectExeNode;
            SelectExeNode* select_node_in = dynamic_cast<SelectExeNode*>(select_node);
            rc = create_selection_executor(trx, selects, db, table_name, *select_node_in);
        }
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

    //add by cmk (2021/10/21)
    std::vector<TupleSet> tuple_sets;
    tuple_sets.reserve(selects.relation_num);
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
            //modified by cmk(2021/10/21)
            //原来使用push_back()时，tuple_sets里的表和查询时输入的表的顺序是相反的。改为insert
            // tuple_sets.push_back(std::move(tuple_set));
            tuple_sets.insert(tuple_sets.begin(), std::move(tuple_set));
        }
    }

    std::stringstream ss;
    if (tuple_sets.size() > 1)
    {   
        // 本次查询了多张表，需要做join操作
        bool exist_empty_table = false;
        LOG_INFO("tuple sets size is %d", tuple_sets.size());
        TupleSchema all_tables_schemas;
        for(auto iter = tuple_sets.begin(); iter != tuple_sets.end(); iter++){
            all_tables_schemas.append((*iter).get_schema());
            if((*iter).size() == 0){
                exist_empty_table = true;
                break;
            }
        }
        //如果有空表，则只打印列名
        if(exist_empty_table){
            LOG_INFO("exist empty table,print fields only");
            all_tables_schemas.print(ss);
        }else{
            TupleSet table_after_join;
            cartesian_product(tuple_sets, table_after_join);
            std::vector<Condition> conditions;
            find_conditions(selects, conditions);

            if(conditions.size() == 0){ //不带条件的多表查询
                LOG_INFO("no multiple table select conditions");
                table_after_join.print(ss);
            }else{ //带条件的多表查询
                LOG_INFO("multiple table select conditions number is [%d]", conditions.size());
                TupleSet table_after_join_and_filt;
                tupleset_cond_filt(table_after_join, conditions, table_after_join_and_filt);
                table_after_join_and_filt.print(ss);
            }
        }
    }
    else
    {
        // 当前只查询一张表，直接返回结果即可
        tuple_sets.front().print(ss);
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
        schema.add_aggr(aggr->aggr_type, AttrType::UNDEFINED, table->name(), field_name, &aggr->constant_value);
        return RC::SUCCESS;
    }
    if (0 == strcmp("*", field_name)) {
        if (aggr->aggr_type == AggrType::COUNT) {
            schema.add_aggr(aggr->aggr_type, AttrType::INTS, table->name(), field_name, nullptr);
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
            schema.add_aggr(aggr->aggr_type, AttrType::INTS, table->name(), field_meta->name(), nullptr);
            break;
        case AggrType::AVG:
            if (field_meta->type() != AttrType::INTS && field_meta->type() != AttrType::FLOATS) {
                LOG_WARN("Field type in aggregation not support aggregation op of avg. %s.%s", table->name(), field_meta->name());
                return RC::SCHEMA_FIELD_TYPE_MISMATCH;
            }
            schema.add_aggr(aggr->aggr_type, AttrType::FLOATS, table->name(), field_meta->name(), nullptr);
            break;
        case AggrType::MAX:
        case AggrType::MIN:
            if (field_meta->type() == AttrType::UNDEFINED) {
                // Min Max 支持所有类型，除了UNDEFINED
                LOG_WARN("Field type in aggregation not support aggregation op of max, min. %s.%s", table->name(), field_meta->name());
                return RC::SCHEMA_FIELD_TYPE_MISMATCH;
            }
            schema.add_aggr(aggr->aggr_type, field_meta->type(), table->name(), field_meta->name(), nullptr);
            break;
        default:
            LOG_DEBUG("Not Support AGGR_UNDEGINED.");
            return RC::GENERIC_ERROR;
            break;
    }
    return RC::SUCCESS;
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

    for (int i = selects.attr_num - 1; i >= 0; i--)
    {
        const RelAttr &attr = selects.attributes[i];
        if (nullptr == attr.relation_name || 0 == strcmp(table_name, attr.relation_name))
        {
            if (0 == strcmp("*", attr.attribute_name))
            {
                // 列出这张表所有字段
                TupleSchema::from_table(table, schema);
                break; // 没有校验，给出* 之后，再写字段的错误
            }
            else
            {
                // 列出这张表相关字段
                RC rc = schema_add_field(table, attr.attribute_name, schema);
                if (rc != RC::SUCCESS)
                {
                    return rc;
                }
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

//add by cmk(2021/10/24) : 从多表查询的结果TupleSet中取出满足conditions的tuple,组成新的Tupleset
void tupleset_cond_filt(const TupleSet& src_tupleset, const std::vector<Condition>& conditions,TupleSet& dst_tupleset)
{
    //针对TupleSet里的每一个Tuple
    const TupleSchema& src_tuple_schema = src_tupleset.get_schema();
    for(int j = 0; j < src_tupleset.size(); j++){
        const Tuple& src_tuple = src_tupleset.get(j);
        //若符合所有conditions，则添加到dst_tupleset
        if(tuple_matched_conditions(src_tuple, src_tuple_schema, conditions)){
            Tuple dst_tuple;
            append_tuple(dst_tuple, src_tuple);
            dst_tupleset.add(std::move(dst_tuple));
        }
    }

    //设置dst_tupleset的字段
    dst_tupleset.set_schema(src_tuple_schema);
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

//add by cmk(2021/10/24): 判断一个Tuple是否满足某些conditions
 bool tuple_matched_conditions(const Tuple& src_tuple, 
                                    const TupleSchema& src_tuple_schema,
                                    const std::vector<Condition>& conditions)
{   
    bool matched_tuple = false; 
    for(auto condition : conditions){
        int tuple_value_left_index = 
            src_tuple_schema.index_of_field(condition.left_attr.relation_name, condition.left_attr.attribute_name);
        int tuple_value_right_index = 
            src_tuple_schema.index_of_field(condition.right_attr.relation_name, condition.right_attr.attribute_name);
        auto& left = src_tuple.get(tuple_value_left_index);
        auto& right = src_tuple.get(tuple_value_right_index);

        int cmp_result = 0;
        cmp_result = left.compare(right);
        LOG_INFO("CMP_RESULT is [%d]",cmp_result);

        switch (condition.comp) {
            case EQUAL_TO:
                matched_tuple = (0 == cmp_result);
                break;
            case LESS_EQUAL:
                matched_tuple = (cmp_result <= 0);
                break;
            case NOT_EQUAL:
                matched_tuple = (cmp_result != 0);
                break;
            case LESS_THAN:
                matched_tuple = (cmp_result < 0);
                break;
            case GREAT_EQUAL:
                matched_tuple = (cmp_result >= 0);
                break;
            case GREAT_THAN:
                matched_tuple = (cmp_result > 0);
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