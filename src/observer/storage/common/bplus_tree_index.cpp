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
// Created by wangyunlai.wyl on 2021/5/19.
//

#include "storage/common/bplus_tree_index.h"
#include "common/log/log.h"

BplusTreeIndex::~BplusTreeIndex() noexcept {
  close();
}

RC BplusTreeIndex::create(const char *file_name, const IndexMeta &index_meta, const std::vector<FieldMeta> &field_metas)
{
    if (inited_)
    {
        return RC::RECORD_OPENNED;
    }

    // Index 类记录索引元信息和每个属性的元信息
    RC rc = Index::init(index_meta, field_metas);
    if (rc != RC::SUCCESS)
    {
        return rc;
    }
    // 索引文件的整体结构 IndexFileHeader||IndexNode||keys||RID
    rc = index_handler_.create(file_name, field_metas, index_meta.isUnique());
    if (RC::SUCCESS == rc)
    {
        inited_ = true;
    }
    return rc;
}

RC BplusTreeIndex::open(const char *file_name, const IndexMeta &index_meta, const std::vector<FieldMeta > &field_metas) {
  if (inited_) {
    return RC::RECORD_OPENNED;
  }
  RC rc = Index::init(index_meta, field_metas);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  rc = index_handler_.open(file_name);
  if (RC::SUCCESS == rc) {
    inited_ = true;
  }
  return rc;
}

RC BplusTreeIndex::close() {
  if (inited_) {
    index_handler_.close();
    inited_ = false;
  }
  return RC::SUCCESS;
}

// zt 修改插入的key
RC BplusTreeIndex::insert_entry(const char *record, const RID *rid) {
    // 构建一个新key
    int attribute_length = 0;
    for (int i = 0; i < field_metas.size(); i++)
    {
        attribute_length += field_metas[i].len();
    }
    char *pkey = (char *)malloc(attribute_length);
    int offset = 0;
    for (size_t i = 0; i < field_metas.size(); i++)
    {
        memcpy(pkey + offset, record + field_metas[i].offset(), field_metas[i].len());
        offset += field_metas[i].len();
    }

    RC rc =  index_handler_.insert_entry(pkey, rid);
    // 释放掉
    free(pkey);
    return rc;
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid) {
    int attribute_length = 0;
    for (int i = 0; i < field_metas.size(); i++)
    {
        attribute_length += field_metas[i].len();
    }

    char *pkey = (char *)malloc(attribute_length);
    int offset = 0;
    for (size_t i = 0; i < field_metas.size(); i++)
    {
        memcpy(pkey + offset, record + field_metas[i].offset(), field_metas[i].len());
        offset += field_metas[i].len();
    }
    RC rc = index_handler_.delete_entry(pkey, rid);
    free(pkey);
    return rc;
}

IndexScanner *BplusTreeIndex::create_scanner(std::vector<CompareObject> compare_objects) {
  BplusTreeScanner *bplus_tree_scanner = new BplusTreeScanner(index_handler_);
  RC rc = bplus_tree_scanner->open(compare_objects);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete bplus_tree_scanner;
    return nullptr;
  }

  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(bplus_tree_scanner);
  return index_scanner;
}

RC BplusTreeIndex::sync() {
  return index_handler_.sync();
}

////////////////////////////////////////////////////////////////////////////////
BplusTreeIndexScanner::BplusTreeIndexScanner(BplusTreeScanner *tree_scanner) :
    tree_scanner_(tree_scanner) {
}

BplusTreeIndexScanner::~BplusTreeIndexScanner() noexcept {
  tree_scanner_->close();
  delete tree_scanner_;
}

RC BplusTreeIndexScanner::next_entry(RID *rid) {
  return tree_scanner_->next_entry(rid);
}

RC BplusTreeIndexScanner::destroy() {
  delete this;
  return RC::SUCCESS;
}