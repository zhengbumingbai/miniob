/*
 * @Author: your name
 * @Date: 2021-10-20 11:54:16
 * @LastEditTime: 2021-10-21 11:18:41
 * @LastEditors: Please set LastEditors
 * @Description: 多张表笛卡尔积操作
 * @FilePath: /Ocean/miniob/src/observer/util/Cartesian_product.h
 */

#ifndef __OBSERVER_UTIL_CARTESIAN_PRODUCT_H__
#define __OBSERVER_UTIL_CARTESIAN_PRODUCT_H__

#include <vector>
class Tuple;
class TupleSet;

/**
 * @description: 将一个Tuple里的值追加到另一个Tuple 
 * @param {目标Tuple}
 * @return {源Tuple}
 */
void append_tuple(Tuple& dst_tuple, const Tuple& tuple);

/**
 * @description: 对多张表执行笛卡尔积操作
 * @param   {要进行笛卡尔积操作的所有表} 
 * @param   {笛卡尔积操作完成并合并后的表 } 
 * @return void
 */
void cartesian_product(const std::vector<TupleSet>& src_tables, TupleSet& dest_table);


#endif //__OBSERVER_UTIL_CARTESIAN_PRODUCT_H__