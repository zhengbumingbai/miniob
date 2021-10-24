/*
 * @Author: your name
 * @Date: 2021-10-20 11:54:16
 * @LastEditTime: 2021-10-21 14:31:48
 * @LastEditors: Please set LastEditors
 * @Description: 多张表笛卡尔积操作
 * @FilePath: /Ocean/miniob/src/observer/util/Cartesian_product.h
 */

#include "cartesian_product.h"
#include "sql/executor/tuple.h"
#include "common/log/log.h"

void append_tuple(Tuple& dst_tuple, const Tuple& tuple)
{
    for(int i =0 ; i < tuple.size(); i++){
        dst_tuple.add(tuple.get_pointer(i));
    }
    return;
}

void cartesian_product(const std::vector<TupleSet>& src_tables, TupleSet& dest_table)
{
    if(src_tables.size() < 1){
        LOG_WARN("Source tables is empty");
        return;
    }
    std::vector<std::vector<const Tuple*>> results;

    int table_num = src_tables.size();
    std::vector<const Tuple*> tmp;
    for(int i = 0; i < table_num; ++i){
        tmp.push_back(&(src_tables[i].get(0)));
    }

    results.push_back(tmp);

    for(int i = table_num-1; i>=0; i--){
        int results_size = results.size();
        
        for(int k = 1; k < src_tables[i].size(); k++){
            for(int j = 0; j < results_size; j++){
                
                tmp = results[j];
                tmp[i] = &(src_tables[i].get(k));
                results.push_back(tmp);
            }
        }
    }

    TupleSchema schemas;
    for(size_t row = 0 ; row < results.size(); row++){
        //将results中的tuple移动到dest_table中
        Tuple merge_tuple;
        for(size_t col = 0; col < results[row].size(); col ++){
            append_tuple(merge_tuple,*results[row][col]);
        }
        dest_table.add(std::move(merge_tuple));
    }
    for(int j = 0; j < table_num; j++){
        schemas.append(src_tables[j].get_schema());
    }
    dest_table.set_schema(schemas);

    return;
}