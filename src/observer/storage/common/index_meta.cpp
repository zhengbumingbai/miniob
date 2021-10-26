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
// Created by wangyunlai.wyl on 2021/5/18.
//

#include "storage/common/index_meta.h"
#include "storage/common/field_meta.h"
#include "storage/common/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "rc.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELDS_NAME("fields_name");
const static Json::StaticString IS_UNIQUE("unique");


RC IndexMeta::init(const char *name, std::vector<std::string> attribute_names, int isUnique) {
  if (nullptr == name || common::is_blank(name)) {
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
// zt  初始化多个属性名
  for (int i = 0; i < attribute_names.size(); i++)
  {
      field_names_.push_back(attribute_names[i]);
  }

//   field_ = field.name();
//   初始化Unique
  isUnique_  = isUnique;
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const {
  json_value[FIELD_NAME] = name_;
  for (int i = 0; i < field_names_.size(); i++)
  {
    json_value[FIELD_FIELDS_NAME].append(field_names_[i]);
  }
  json_value[IS_UNIQUE]  = isUnique_;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index) {
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &fields_value = json_value[FIELD_FIELDS_NAME];
  const Json::Value &isUnique = json_value[IS_UNIQUE];
  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }

  for (int i = 0; i < fields_value.size(); i++)
  {
    if (!fields_value[i].isString()) {
        LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
                name_value.asCString(), fields_value[i].toStyledString().c_str());
        return RC::GENERIC_ERROR;
    }
  }
  
  if(!isUnique.isInt()){
      LOG_ERROR("IsUnique is not a int. json value=%s", isUnique.toStyledString().c_str());
    return RC::GENERIC_ERROR;
  }
  std::vector<std::string> attribute_names;
  for (int i = 0; i < fields_value.size(); i++)
  {
      const FieldMeta *field = table.field(fields_value[i].asCString());
      if (nullptr == field) {
        LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), fields_value[i].asCString());
        return RC::SCHEMA_FIELD_MISSING;
      }
      attribute_names.push_back(fields_value[i].asCString());
  }
  
  return index.init(name_value.asCString(), attribute_names,isUnique.asInt());
}

const char *IndexMeta::name() const {
  return name_.c_str();
}

// const char *IndexMeta::field() const {
//   return field_.c_str();
// }

const std::vector<std::string> &IndexMeta::field_names() const{
    return field_names_;
}
const int IndexMeta::isUnique() const{
    return isUnique_;
}
void IndexMeta::desc(std::ostream &os) const {
  os << "index name=" << name_ << ", fields =";
  for (int i = 0; i < field_names_.size(); i++)
  {
      os << " ," << field_names_[i];
  }
}