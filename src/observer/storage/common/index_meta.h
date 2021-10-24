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
// Created by Wangyunlai on 2021/5/12.
//

#ifndef __OBSERVER_STORAGE_COMMON_INDEX_META_H__
#define __OBSERVER_STORAGE_COMMON_INDEX_META_H__

#include <string>
#include <vector>
#include "rc.h"

class TableMeta;
class FieldMeta;

namespace Json {
class Value;
} // namespace Json

class IndexMeta {
public:
  IndexMeta() = default;

    // zt 修改初始化函数
//   RC init(const char *name, const FieldMeta &field,int isUnique = 0);
// zt 为了适配多列 再次修改初始化函数
  RC init(const char *name, const char **attribute_name, int attribute_length, int isUnique);

public:
  const char *name() const;
  //   zt 弃用field函数
  //   const char *field() const;
  const std::vector<std::string> &field_names() const;
  const int isUnique() const;

  void desc(std::ostream &os) const;
public:
  void to_json(Json::Value &json_value) const;
  static RC from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index);

private:
  std::string       name_;
//   std::string       field_;
// zt 元信息存储多个列名 用于索引匹配
  std::vector<std::string> field_names_;
  //   私有的 不允许修改
  int isUnique_; //zt 新增关键字
};
#endif // __OBSERVER_STORAGE_COMMON_INDEX_META_H__