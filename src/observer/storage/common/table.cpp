/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its
affiliates. All rights reserved. miniob is licensed under Mulan PSL v2. You can
use this software according to the terms and conditions of the Mulan PSL v2. You
may obtain a copy of Mulan PSL v2 at: http://license.coscl.org.cn/MulanPSL2 THIS
SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2021/5/13.
//

#include "storage/common/table.h"

#include <limits.h>
#include <string.h>
#include <vector>

#include <algorithm>

#include "common/lang/string.h"
#include "common/log/log.h"
#include "storage/common/bplus_tree_index.h"
#include "storage/common/condition_filter.h"
#include "storage/common/index.h"
#include "storage/common/meta_util.h"
#include "storage/common/record_manager.h"
#include "storage/common/table_meta.h"
#include "storage/default/disk_buffer_pool.h"
#include "storage/trx/trx.h"

bool file_exist(const char *file) {
  FILE *fp;
  fp = fopen(file, "r");

  if (fp == NULL) {
    return false;
  } else {
    fclose(fp);
    return true;
  }
}

Table::Table()
    : data_buffer_pool_(nullptr), file_id_(-1), record_handler_(nullptr) {}

Table::~Table() {
  delete record_handler_;
  record_handler_ = nullptr;

  if (data_buffer_pool_ != nullptr && file_id_ >= 0) {
    data_buffer_pool_->close_file(file_id_);
    data_buffer_pool_ = nullptr;
  }

  LOG_INFO("Table has been closed: %s", name());
}

RC Table::create(const char *path, const char *name, const char *base_dir,
                 int attribute_count, const AttrInfo attributes[]) {
  if (nullptr == name || common::is_blank(name)) {
    LOG_WARN("Name cannot be empty");
    return RC::INVALID_ARGUMENT;
  }
  LOG_INFO("Begin to create table %s:%s", base_dir, name);

  if (attribute_count <= 0 || nullptr == attributes) {
    LOG_WARN(
        "Invalid arguments. table_name=%s, attribute_count=%d, attributes=%p",
        name, attribute_count, attributes);
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;

  // 使用 table_name.table记录一个表的元数据
  // 判断表文件是否已经存在

  int fd = ::open(path, O_WRONLY | O_CREAT | O_EXCL | O_CLOEXEC, 0600);
  if (-1 == fd) {
    if (EEXIST == errno) {
      LOG_ERROR(
          "Failed to create table file, it has been created. %s, EEXIST, %s",
          path, strerror(errno));
      return RC::SCHEMA_TABLE_EXIST;
    }
    LOG_ERROR("Create table file failed. filename=%s, errmsg=%d:%s", path,
              errno, strerror(errno));
    return RC::IOERR;
  }

  close(fd);

  // 创建文件
  if ((rc = table_meta_.init(name, attribute_count, attributes)) !=
      RC::SUCCESS) {
    LOG_ERROR("Failed to init table meta. name:%s, ret:%d", name, rc);
    return rc;  // delete table file
  }

  std::fstream fs;
  fs.open(path, std::ios_base::out | std::ios_base::binary);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s", path,
              strerror(errno));
    return RC::IOERR;
  }

  // 记录元数据到文件中
  table_meta_.serialize(fs);
  fs.close();

  std::string data_file =
      std::string(base_dir) + "/" + name + TABLE_DATA_SUFFIX;
  data_buffer_pool_ = theGlobalDiskBufferPool();
  rc = data_buffer_pool_->create_file(data_file.c_str());
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to create disk buffer pool of data file. file name=%s",
              data_file.c_str());
    return rc;
  }

  rc = init_record_handler(base_dir);

  base_dir_ = base_dir;
  LOG_INFO("Successfully create table %s:%s", base_dir, name);
  return rc;
}

RC Table::open(const char *meta_file, const char *base_dir) {
  // 加载元数据文件
  std::fstream fs;
  std::string meta_file_path = std::string(base_dir) + "/" + meta_file;
  fs.open(meta_file_path, std::ios_base::in | std::ios_base::binary);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open meta file for read. file name=%s, errmsg=%s",
              meta_file, strerror(errno));
    return RC::IOERR;
  }
  if (table_meta_.deserialize(fs) < 0) {
    LOG_ERROR("Failed to deserialize table meta. file name=%s", meta_file);
    return RC::GENERIC_ERROR;
  }
  fs.close();

  // 加载数据文件
  RC rc = init_record_handler(base_dir);

  base_dir_ = base_dir;

  const int index_num = table_meta_.index_num();
  for (int i = 0; i < index_num; i++) {
    const IndexMeta *index_meta = table_meta_.index(i);
    const FieldMeta *field_meta;
    const char *field_name = nullptr;
    for (int i = 0; i < index_meta->field_names().size(); i++)
    {
        field_name = index_meta->field_names()[i].c_str();
        field_meta = table_meta_.field(field_name);

        if (field_meta == nullptr && field_name != nullptr) {
          LOG_PANIC("Found invalid index meta info which has a non-exists field. "
            "table=%s, index=%s, field=%s",name(), index_meta->name(), field_name);
          return RC::GENERIC_ERROR;
        }
    }
    
    // const FieldMeta *field_meta = table_meta_.field(index_meta->field());

    BplusTreeIndex *index = new BplusTreeIndex();
    std::string index_file =
        index_data_file(base_dir, name(), index_meta->name());
    rc = index->open(index_file.c_str(), *index_meta, table_meta_.fields());
    if (rc != RC::SUCCESS) {
      delete index;
      LOG_ERROR("Failed to open index. table=%s, index=%s, file=%s, rc=%d:%s",
                name(), index_meta->name(), index_file.c_str(), rc, strrc(rc));
      return rc;
    }
    indexes_.push_back(index);
  }
  return rc;
}

RC Table::destroy_table() {
  RC rc = RC::SUCCESS;
  std::string table_file_path = table_meta_file(base_dir_.c_str(), name());
  std::string table_data_path = table_data_file(base_dir_.c_str(), name());

  // delete table metadata file
  if (file_exist(table_file_path.c_str())) {
    remove(table_file_path.c_str());
  } else {
    return RC::IOERR_DELETE;
  }

  // delete table data file
  if (file_exist(table_data_path.c_str())) {
    remove(table_data_path.c_str());
  } else {
    return RC::IOERR_DELETE;
  }

  // delete index files
  const int index_num = table_meta_.index_num();
  for (int i = 0; i < index_num; i++) {
    const IndexMeta *index_meta = table_meta_.index(i);
    const FieldMeta *field_meta;
    char *field_name = nullptr;
    for (int i = 0; i < index_meta->field_names().size(); i++)
    {
        // std::cerr <<"1. "<<index_meta->field_names()[i]<<std::endl;
        field_name = strdup(index_meta->field_names()[i].c_str());
        // LOG_DEBUG("field_name: %s", field_name);
        field_meta = table_meta_.field(field_name);

        if (field_meta == nullptr && field_name != nullptr) {
          LOG_PANIC("Found invalid index meta info which has a non-exists field. "
            "table=%s, index=%s, field=%s",name(), index_meta->name(), field_name);
          return RC::GENERIC_ERROR;
        }
        free(field_name);
    }
    
    std::string index_file =
        index_data_file(base_dir_.c_str(), name(), index_meta->name());
    if (file_exist(index_file.c_str())) {
      remove(index_file.c_str());
    } else {
      return RC::IOERR_DELETE;
    }
  }

  // release index BPlus tree
  for (Index *index : indexes_) {
    delete index;
  }
  return rc;
}

std::string Table::base_dir(){
    return base_dir_;
}

RC Table::commit_insert(Trx *trx, const RID &rid) {
  Record record;
  RC rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return trx->commit_insert(this, record);
}

RC Table::rollback_insert(Trx *trx, const RID &rid) {
  Record record;
  RC rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  // remove all indexes
  rc = delete_entry_of_indexes(record.data, rid, false);
  if (rc != RC::SUCCESS) {
    LOG_ERROR(
        "Failed to delete indexes of record(rid=%d.%d) while rollback insert, "
        "rc=%d:%s",
        rid.page_num, rid.slot_num, rc, strrc(rc));
  } else {
    rc = record_handler_->delete_record(&rid);
  }
  return rc;
}

RC Table::insert_record(Trx *trx, Record *record) {
  RC rc = RC::SUCCESS;

  if (trx != nullptr) {
    trx->init_trx_info(this, *record);
  }
  rc = record_handler_->insert_record(record->data, table_meta_.record_size(),
                                      &record->rid);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Insert record failed. table name=%s, rc=%d:%s",
              table_meta_.name(), rc, strrc(rc));
    return rc;
  }

// zt 暂时不考虑事务
//   if (trx != nullptr) {
//     rc = trx->insert_record(this, record);
//     // zt 不考虑事务
//     if (rc != RC::SUCCESS) {
//       LOG_ERROR("Failed to log operation(insertion) to trx");

//       RC rc2 = record_handler_->delete_record(&record->rid);
//       if (rc2 != RC::SUCCESS) {
//         LOG_PANIC(
//             "Failed to rollback record data when insert index entries failed. "
//             "table name=%s, rc=%d:%s",
//             name(), rc2, strrc(rc2));
//       }
//       return rc;
//     }
//   }

  rc = insert_entry_of_indexes(record->data, record->rid);
  if (rc != RC::SUCCESS) {
    RC rc2 = delete_entry_of_indexes(record->data, record->rid, true);
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC(
          "Failed to rollback index data when insert index entries failed. "
          "table name=%s, rc=%d:%s",
          name(), rc2, strrc(rc2));
    }
    rc2 = record_handler_->delete_record(&record->rid);
    if (rc2 != RC::SUCCESS) {
      LOG_PANIC(
          "Failed to rollback record data when insert index entries failed. "
          "table name=%s, rc=%d:%s",
          name(), rc2, strrc(rc2));
    }
    return rc;
  }
  return rc;
}
RC Table::insert_record(Trx *trx, int value_num, const Value *values, RID *rid)
{
  if (value_num <= 0 || nullptr == values) {
    LOG_ERROR("Invalid argument. value num=%d, values=%p", value_num, values);
    return RC::INVALID_ARGUMENT;
  }

  char *record_data;
  RC rc = make_record(value_num, values, record_data);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to create a record. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  Record record;
  record.data = record_data;
  // record.valid = true;
  rc = insert_record(trx, &record);

  //   通过传指针 记录下rid
  if (rid)
  {
    *rid = record.rid;
  }

  delete[] record_data;
  return rc;
}

const char *Table::name() const { return table_meta_.name(); }

const TableMeta &Table::table_meta() const { return table_meta_; }

RC Table::make_record(int value_num, const Value *values, char *&record_out) {
  // 检查字段类型是否一致
  if (value_num + table_meta_.sys_field_num() != table_meta_.field_num()) {
    return RC::SCHEMA_FIELD_MISSING;
  }

  const int normal_field_start_index = table_meta_.sys_field_num();
  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];
    // zt 校验UNIX时间戳是否合法
    if (value.type == DATES && *(int *)value.data == INT32_MIN) {
        LOG_DEBUG("INSERT DATES TYPE INVAILD");
      return RC::RECORD;
    }
    if (field->type() != value.type && value.type != AttrType::NULLFIELD) {
      LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
                field->name(), field->type(), value.type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
    if (!field->nullable() && value.type == AttrType::NULLFIELD) {
      LOG_ERROR("Fieid is not nullable. Invalid value type. field name=%s, type=%d, nullable=%d, but given=%d",
                field->name(), field->type(), field->nullable(), value.type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  // 复制所有字段的值
  int record_size = table_meta_.record_size();
  char *record = new char[record_size];

  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    const Value &value = values[i];
    
    if(value.type != AttrType::NULLFIELD) {
      int8_t is_null = 0;
      memcpy(record + field->offset(), value.data, field->len()-1);
      memcpy(record + field->offset()+field->len()-1, &is_null, 1);
    } else {
      int8_t is_null = 1;
      memcpy(record + field->offset()+field->len()-1, &is_null, 1);
    }
  }

  record_out = record;
  return RC::SUCCESS;
}

RC Table::make_updated_record(const char *record_in, const char *attribute_name,
                              const Value *value, char *&record_out,char *&copyed_old_record) {
  // 检查字段类型是否一致
  int value_num = table_meta_.field_num() - table_meta_.sys_field_num();

  const int normal_field_start_index = table_meta_.sys_field_num();
  int attribute_loc = -1;
  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field = table_meta_.field(i + normal_field_start_index);
    if (std::string(field->name()) == std::string(attribute_name)) {
      attribute_loc = i;
      // zt 校验UNIX时间戳是否合法
      if (value->type == DATES && *(int *)value->data == INT32_MIN) {
          LOG_DEBUG("INSERT DATES TYPE INVAILD");
        return RC::RECORD;
      }
      if (field->type() != value->type && value->type != AttrType::NULLFIELD) {
        LOG_ERROR("Invalid value type. field name=%s, type=%d, but given=%d",
                  field->name(), field->type(), value->type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }else{
        //   找到了就break
          break;
      }
      if (!field->nullable() && value->type == AttrType::NULLFIELD) {
        LOG_ERROR("Fieid is not nullable. Invalid value type. field name=%s, type=%d, nullable=%d, but given=%d",
                  field->name(), field->type(), field->nullable(), value->type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
  }

  if (attribute_loc == -1) {
    LOG_ERROR("Attribute not exists. attribute name=%s", attribute_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  // 复制原来所有字段的值
  int record_size = table_meta_.record_size();
  char *record = new char[record_size];
  char *copied_record = new char[record_size];
  memcpy(record, record_in, record_size);
  memcpy(copied_record, record_in, record_size);

  // 复制新值
  const FieldMeta *field =
      table_meta_.field(attribute_loc + normal_field_start_index);
  if(value->type != AttrType::NULLFIELD) {
      int8_t is_null = 0;
      memcpy(record + field->offset(), value->data, field->len()-1);
      memcpy(record + field->offset()+field->len()-1, &is_null, 1);
    } else {
      int8_t is_null = 1;
      memcpy(record + field->offset()+field->len()-1, &is_null, 1);
    }

  record_out = record;
  copyed_old_record = copied_record;
  return RC::SUCCESS;
}

RC Table::init_record_handler(const char *base_dir) {
  std::string data_file =
      std::string(base_dir) + "/" + table_meta_.name() + TABLE_DATA_SUFFIX;
  if (nullptr == data_buffer_pool_) {
    data_buffer_pool_ = theGlobalDiskBufferPool();
  }

  int data_buffer_pool_file_id;
  RC rc = data_buffer_pool_->open_file(data_file.c_str(),
                                       &data_buffer_pool_file_id);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to open disk buffer pool for file:%s. rc=%d:%s",
              data_file.c_str(), rc, strrc(rc));
    return rc;
  }

  record_handler_ = new RecordFileHandler();
  rc = record_handler_->init(*data_buffer_pool_, data_buffer_pool_file_id);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to init record handler. rc=%d:%s", rc, strrc(rc));
    return rc;
  }

  file_id_ = data_buffer_pool_file_id;
  return rc;
}

/**
 * 为了不把Record暴露出去，封装一下
 */
class RecordReaderScanAdapter {
 public:
  explicit RecordReaderScanAdapter(void (*record_reader)(const char *data,
                                                         void *context),
                                   void *context)
      : record_reader_(record_reader), context_(context) {}

  void consume(const Record *record) { record_reader_(record->data, context_); }

 private:
  void (*record_reader_)(const char *, void *);
  void *context_;
};
static RC scan_record_reader_adapter(Record *record, void *context) {
  RecordReaderScanAdapter &adapter = *(RecordReaderScanAdapter *)context;
  adapter.consume(record);
  return RC::SUCCESS;
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit,
                      void *context,
                      void (*record_reader)(const char *data, void *context)) {
  RecordReaderScanAdapter adapter(record_reader, context);
  return scan_record(trx, filter, limit, (void *)&adapter,
                     scan_record_reader_adapter);
}

RC Table::scan_record(Trx *trx, ConditionFilter *filter, int limit,
                      void *context,
                      RC (*record_reader)(Record *record, void *context)) {
  if (nullptr == record_reader) {
    return RC::INVALID_ARGUMENT;
  }

  if (0 == limit) {
    return RC::SUCCESS;
  }

  if (limit < 0) {
    limit = INT_MAX;
  }

// zt 建索引时filter为 null，此处select条件过滤时执行的
  IndexScanner *index_scanner = find_index_for_scan(filter);
  if (index_scanner != nullptr) {
    RC rc = scan_record_by_index(trx, index_scanner, filter, limit, context,
                                record_reader);
    if(rc == RC::RECORD_NO_MORE_IDX_IN_MEM || rc == RC::RECORD_EOF || rc == RC::SUCCESS) return RC::SUCCESS;
  }

  RC rc = RC::SUCCESS;
  RecordFileScanner scanner;
  rc = scanner.open_scan(*data_buffer_pool_, file_id_, filter);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("failed to open scanner. file id=%d. rc=%d:%s", file_id_, rc,
              strrc(rc));
    return rc;
  }

    //zt 执行逐条查输入
  int record_count = 0;
  Record record;
  rc = scanner.get_first_record(&record);
  for (; RC::SUCCESS == rc && record_count < limit;
       rc = scanner.get_next_record(&record)) {
    if (trx == nullptr || trx->is_visible(this, &record)) {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS) {
        break;
      }
      record_count++;
    }
  }

  if (RC::RECORD_EOF == rc) {
    rc = RC::SUCCESS;
  } else {
    LOG_ERROR("failed to scan record. file id=%d, rc=%d:%s", file_id_, rc,
              strrc(rc));
  }
  scanner.close_scan();
  return rc;
}

RC Table::scan_record_by_index(Trx *trx, IndexScanner *scanner,
                               ConditionFilter *filter, int limit,
                               void *context,
                               RC (*record_reader)(Record *, void *)) {
  RC rc = RC::SUCCESS;
  RID rid;
  Record record;
  int record_count = 0;
  while (record_count < limit) {
    rc = scanner->next_entry(&rid);
    if (rc != RC::SUCCESS) {
      if (RC::RECORD_EOF == rc) {
        rc = RC::SUCCESS;
        break;
      }
      LOG_ERROR("Failed to scan table by index. rc=%d:%s", rc, strrc(rc));
      break;
    }

    rc = record_handler_->get_record(&rid, &record);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to fetch record of rid=%d:%d, rc=%d:%s", rid.page_num,
                rid.slot_num, rc, strrc(rc));
      break;
    }

    if ((trx == nullptr || trx->is_visible(this, &record)) &&
        (filter == nullptr || filter->filter(record))) {
      rc = record_reader(&record, context);
      if (rc != RC::SUCCESS) {
        LOG_TRACE("Record reader break the table scanning. rc=%d:%s", rc,
                  strrc(rc));
        break;
      }
    }

    record_count++;
  }

  scanner->destroy();
  return rc;
}

class IndexInserter {
 public:
  explicit IndexInserter(Index *index) : index_(index) {}

  RC insert_index(const Record *record) {
    return index_->insert_entry(record->data, &record->rid);
  }

 private:
  Index *index_;
};

static RC insert_index_record_reader_adapter(Record *record, void *context) {
  IndexInserter &inserter = *(IndexInserter *)context;
  return inserter.insert_index(record);
}

// zt 增加多列属性元信息录入
RC Table::create_index(Trx *trx, const char *index_name, std::vector<std::string> atteibute_names, int isUnique)
{
    // 检查索引名是否为nullptr或者空白字符串
    if (index_name == nullptr || common::is_blank(index_name))
    {
        return RC::INVALID_ARGUMENT;
    }
    // 检查每一个属性名是否为nullptr或者空百字符串 
    for (int i = 0; i < atteibute_names.size(); i++)
    {
        const char *attr_name = atteibute_names[i].c_str();
        if (attr_name == nullptr || common::is_blank(attr_name))
        {
            return RC::INVALID_ARGUMENT;
        }
    }

    // if (index_name == nullptr || common::is_blank(index_name) ||
    //     attribute_name == nullptr || common::is_blank(attribute_name))
    // {
    //     return RC::INVALID_ARGUMENT;
    // }
// zt 检查是否有重名索引或者结构的索引
    if (table_meta_.index(index_name) != nullptr ||
        table_meta_.find_index_by_field(atteibute_names))
    {
        return RC::SCHEMA_INDEX_EXIST;
    }

    // table元信息检查 查看是否每一个属性都存在
    std::vector<FieldMeta> field_metas;
    for (int i = 0; i < atteibute_names.size(); i++)
    {
        const FieldMeta *field_meta = table_meta_.field(atteibute_names[i].c_str());
        if (!field_meta)
        {
            return RC::SCHEMA_FIELD_MISSING;
        }
        field_metas.push_back(*field_meta);
    }

    // zt 初始化索引元信息 增加uniuqe 字段
  IndexMeta new_index_meta;
  RC rc = new_index_meta.init(index_name, atteibute_names, isUnique);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  // 创建索引相关数据
  BplusTreeIndex *index = new BplusTreeIndex();
  std::string index_file =
      index_data_file(base_dir_.c_str(), name(), index_name);
  rc = index->create(index_file.c_str(), new_index_meta, field_metas);
  if (rc != RC::SUCCESS) {
    delete index;
    LOG_ERROR("Failed to create bplus tree index. file name=%s, rc=%d:%s",
              index_file.c_str(), rc, strrc(rc));
    return rc;
  }

  // 遍历当前的所有数据，插入这个索引
// zt  只需要修改索引插入的适配器 函数指针指向的函数
  IndexInserter index_inserter(index);
  rc = scan_record(trx, nullptr, -1, &index_inserter,
                   insert_index_record_reader_adapter);
  if (rc != RC::SUCCESS) {
    // rollback
    delete index;
    LOG_ERROR("Failed to insert index to all records. table=%s, rc=%d:%s",
              name(), rc, strrc(rc));
    return rc;
  }
  indexes_.push_back(index);

  TableMeta new_table_meta(table_meta_);
  rc = new_table_meta.add_index(new_index_meta);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to add index (%s) on table (%s). error=%d:%s", index_name,
              name(), rc, strrc(rc));
    return rc;
  }
  // 创建元数据临时文件
  std::string tmp_file = table_meta_file(base_dir_.c_str(), name()) + ".tmp";
  std::fstream fs;
  fs.open(tmp_file,
          std::ios_base::out | std::ios_base::binary | std::ios_base::trunc);
  if (!fs.is_open()) {
    LOG_ERROR("Failed to open file for write. file name=%s, errmsg=%s",
              tmp_file.c_str(), strerror(errno));
    return RC::IOERR;  // 创建索引中途出错，要做还原操作
  }
  if (new_table_meta.serialize(fs) < 0) {
    LOG_ERROR("Failed to dump new table meta to file: %s. sys err=%d:%s",
              tmp_file.c_str(), errno, strerror(errno));
    return RC::IOERR;
  }
  fs.close();

  // 覆盖原始元数据文件
  std::string meta_file = table_meta_file(base_dir_.c_str(), name());
  int ret = rename(tmp_file.c_str(), meta_file.c_str());
  if (ret != 0) {
    LOG_ERROR(
        "Failed to rename tmp meta file (%s) to normal meta file (%s) while "
        "creating index (%s) on table (%s). "
        "system error=%d:%s",
        tmp_file.c_str(), meta_file.c_str(), index_name, name(), errno,
        strerror(errno));
    return RC::IOERR;
  }

  table_meta_.swap(new_table_meta);

  LOG_INFO("add a new index (%s) on the table (%s)", index_name, name());

  return rc;
}

class RecordUpdater {
 public:
  RecordUpdater(Table &table, Trx *trx, const char *attribute_name,
                const Value *value)
      : table_(table),
        trx_(trx),
        attribute_name_(attribute_name),
        value_(value) {}

  RC update_record(Record *old_record) {
    RC rc = RC::SUCCESS;


    Record *edited_record = new Record();
    Record *copy_old_record = new Record();
    edited_record->rid = old_record->rid;
    copy_old_record->rid = old_record->rid;
    rc = table_.make_updated_record(old_record->data, attribute_name_, value_,
                                    edited_record->data, copy_old_record->data);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    rc = table_.update_record(trx_, copy_old_record, edited_record);
    if (rc == RC::SUCCESS) {
      updated_count_++;
    }

    delete[] edited_record->data;
    delete[] copy_old_record->data;
    return rc;
  }

  int updated_count() const { return updated_count_; }

 private:
  Table &table_;
  Trx *trx_;
  const char *attribute_name_;
  const Value *value_;
  int updated_count_ = 0;
};

static RC record_reader_update_adapter(Record *record, void *context) {
  RecordUpdater &record_updater = *(RecordUpdater *)context;
  return record_updater.update_record(record);
}

// 第三题
RC Table::update_record(Trx *trx, const char *attribute_name,
                        const Value *value, int condition_num,
                        const Condition conditions[], int *updated_count) {
  RecordUpdater record_updater(*this, trx, attribute_name, value);
  CompositeConditionFilter condition_filter;
  RC rc = condition_filter.init(*this, conditions, condition_num);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  rc = scan_record(trx, &condition_filter, -1, &record_updater,
                   record_reader_update_adapter);
  if (updated_count != nullptr) {
    *updated_count = record_updater.updated_count();
  }
  return rc;
}

RC Table::update_record(Trx *trx, Record *old_record, Record *record) {
  RC rc = RC::SUCCESS;

  rc = record_handler_->update_record(record);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Update record failed. table name=%s, rc=%d:%s",
              table_meta_.name(), rc, strrc(rc));
    return rc;
  }

  // 官方说不测试事务，先跳过事务
  // if (trx != nullptr) {
  //   rc = trx->update_record(this, record);
  //   if (rc != RC::SUCCESS) {
  //     LOG_ERROR("Failed to log operation(update) to trx");

  //     RC rc2 = record_handler_->delete_record(&record->rid);
  //     if (rc2 != RC::SUCCESS) {
  //       LOG_PANIC("Failed to rollback record data when update index entries
  //       failed. table name=%s, rc=%d:%s",
  //                 name(), rc2, strrc(rc2));
  //     }
  //     return rc;
  //   }
  // }

  rc = update_entry_of_indexes(old_record->data, record->data, record->rid,
                               false);
  if (rc != RC::SUCCESS) {
    LOG_ERROR(
        "Update index failed when update operation. table name=%s, rc=%d:%s",
        table_meta_.name(), rc, strrc(rc));
    RC rc2 = record_handler_->update_record(old_record);

    if (rc2 != RC::SUCCESS) {
      LOG_ERROR("Rollback record failed. table name=%s, rc=%d:%s",
                table_meta_.name(), rc, strrc(rc));
      RC rc3 = record_handler_->delete_record(&old_record->rid);
      if (rc3 != RC::SUCCESS) {
        LOG_PANIC(
            "Failed to delete record data when update record failed and "
            "rollback old record failed. table name=%s, rc=%d:%s",
            name(), rc3, strrc(rc3));
      }
      return rc;
    }

    delete_entry_of_indexes(record->data, record->rid, true);
    delete_entry_of_indexes(old_record->data, old_record->rid, true);
    // 暂不处理删除旧索引以及新索引错误的rc，因为可能在update索引过程中，旧索引已经删除，新索引还没插入。这需要判断更详细的状态码。

    // 因为update_record错误不会删改旧数据，所以rollback索引，插回旧索引
    RC rc4 = insert_entry_of_indexes(old_record->data, old_record->rid);
    if (rc4 != RC::SUCCESS) {
      LOG_PANIC(
          "Failed to rollback index data when update index entries failed. "
          "table name=%s, rc=%d:%s",
          name(), rc4, strrc(rc4));
      // 暂不处理
      delete_entry_of_indexes(old_record->data, old_record->rid, true);
      RC rc5 = record_handler_->delete_record(&old_record->rid);
      if (rc5 != RC::SUCCESS) {
        LOG_PANIC(
            "Failed to delete record data when update index entries failed and "
            "rollback index entries failed. table name=%s, rc=%d:%s",
            name(), rc5, strrc(rc5));
      }
    }
    return rc;
  }
}

class RecordDeleter {
 public:
  RecordDeleter(Table &table, Trx *trx) : table_(table), trx_(trx) {}

  RC delete_record(Record *record) {
    RC rc = RC::SUCCESS;
    rc = table_.delete_record(trx_, record);
    if (rc == RC::SUCCESS) {
      deleted_count_++;
    }
    return rc;
  }

  int deleted_count() const { return deleted_count_; }

 private:
  Table &table_;
  Trx *trx_;
  int deleted_count_ = 0;
};

static RC record_reader_delete_adapter(Record *record, void *context) {
  RecordDeleter &record_deleter = *(RecordDeleter *)context;
  return record_deleter.delete_record(record);
}

RC Table::delete_record(Trx *trx, ConditionFilter *filter, int *deleted_count) {
  RecordDeleter deleter(*this, trx);
  RC rc = scan_record(trx, filter, -1, &deleter, record_reader_delete_adapter);
  if (deleted_count != nullptr) {
    *deleted_count = deleter.deleted_count();
  }
  return rc;
}

RC Table::delete_record(Trx *trx, Record *record) {
  RC rc = RC::SUCCESS;
  if (trx != nullptr) {
    rc = trx->delete_record(this, record);
  } else {
    rc = delete_entry_of_indexes(record->data, record->rid,
                                 false);  // 重复代码 refer to commit_delete
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to delete indexes of record (rid=%d.%d). rc=%d:%s",
                record->rid.page_num, record->rid.slot_num, rc, strrc(rc));
    } else {
      rc = record_handler_->delete_record(&record->rid);
    }
  }
  return rc;
}

RC Table::commit_delete(Trx *trx, const RID &rid) {
  RC rc = RC::SUCCESS;
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  rc = delete_entry_of_indexes(record.data, record.rid, false);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to delete indexes of record(rid=%d.%d). rc=%d:%s",
              rid.page_num, rid.slot_num, rc, strrc(rc));  // panic?
  }

  rc = record_handler_->delete_record(&rid);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return rc;
}

RC Table::rollback_delete(Trx *trx, const RID &rid) {
  RC rc = RC::SUCCESS;
  Record record;
  rc = record_handler_->get_record(&rid, &record);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return trx->rollback_delete(this, record);  // update record in place
}

RC Table::insert_entry_of_indexes(const char *record, const RID &rid) {
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->insert_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      break;
    }
  }
  return rc;
}

RC Table::update_entry_of_indexes(const char *old_record, const char *record,
                                  const RID &rid, bool error_on_not_exists) {
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    LOG_DEBUG("DELETE INDEX ENTRY.");
    // LOG_DEBUG("OLD RECORD: ");
    // for (int i = 0; i < table_meta_.fields().size(); i++)
    // {
    //     switch (table_meta_.fields()[i].type())
    //     {
    //     case INTS:
    //         LOG_DEBUG("INT: %d", *(int *)(old_record+table_meta_.fields()[i].offset()));
    //         break;
    //     case DATES:
    //         LOG_DEBUG("DATE: %d", *(int *)(old_record+table_meta_.fields()[i].offset()));
    //         break;
    //     case FLOATS:
    //         LOG_DEBUG("FLOAT: %.2f", *(float *)(old_record+table_meta_.fields()[i].offset()));
    //         break;
    //     case CHARS:
    //         LOG_DEBUG("CHARS: %s", old_record + table_meta_.fields()[i].offset());
    //         break;
    //     default:
    //         break;
    //     }
    // }
    
    rc = index->delete_entry(old_record, &rid);
    if (rc != RC::SUCCESS) {
      if (rc != RC::RECORD_INVALID_KEY || error_on_not_exists) {
        break;
      } else {
        rc = RC::SUCCESS;
      }
    }
    LOG_DEBUG("INSERT INDEX ENTRY.");
    // LOG_DEBUG("NEW RECORD: ");
    // for (int i = 0; i < table_meta_.fields().size(); i++)
    // {
    //     switch (table_meta_.fields()[i].type())
    //     {
    //     case INTS:
    //         LOG_DEBUG("INT: %d", *(int *)(record + table_meta_.fields()[i].offset()));
    //         break;
    //     case DATES:
    //         LOG_DEBUG("DATE: %d", *(int *)(record+table_meta_.fields()[i].offset()));
    //         break;
    //     case FLOATS:
    //         LOG_DEBUG("FLOAT: %.2f", *(float *)(record+table_meta_.fields()[i].offset()));
    //         break;
    //     case CHARS:
    //         LOG_DEBUG("CHARS: %s", record + table_meta_.fields()[i].offset());
    //         break;
    //     default:
    //         break;
    //     }
    // }
    rc = index->insert_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      break;
    }
  }
  return rc;
}

RC Table::delete_entry_of_indexes(const char *record, const RID &rid,
                                  bool error_on_not_exists) {
  RC rc = RC::SUCCESS;
  for (Index *index : indexes_) {
    rc = index->delete_entry(record, &rid);
    if (rc != RC::SUCCESS) {
      if (rc != RC::RECORD_INVALID_KEY || error_on_not_exists) {
        break;
      } else {
        rc = RC::SUCCESS;
      }
    }
  }
  return rc;
}

Index *Table::find_index(const char *index_name) const {
  for (Index *index : indexes_) {
    if (0 == strcmp(index->index_meta().name(), index_name)) {
      return index;
    }
  }
  return nullptr;
}


//zt 为了满足多列索引 需要重构寻找合适的索引
// zt 只有一个列的情况
// 现在无论是单列条件或者是组合条件都只会找到单列的
// 需要修改匹配规则
IndexScanner *Table::find_index_for_scan(const DefaultConditionFilter &filter) {
  const ConDesc *field_cond_desc = nullptr;
  const ConDesc *value_cond_desc = nullptr;
//   zt 条件两端必须有一个不是常量 
  if (filter.left().is_attr && !filter.right().is_attr) {
    field_cond_desc = &filter.left();
    value_cond_desc = &filter.right();
  } else if (filter.right().is_attr && !filter.left().is_attr) {
    field_cond_desc = &filter.right();
    value_cond_desc = &filter.left();
  }

//   如果不满足条件就返回nullptr
  if (field_cond_desc == nullptr || value_cond_desc == nullptr) {
    return nullptr;
  }

  const FieldMeta *field_meta =
      table_meta_.find_field_by_offset(field_cond_desc->attr_offset);
  if (nullptr == field_meta) {
    LOG_PANIC("Cannot find field by offset %d. table=%s",
              field_cond_desc->attr_offset, name());
    return nullptr;
  }

  std::vector<CompareObject> compare_objects;
  CompareObject comp;
  comp.comp_op_ = filter.comp_op();
  comp.value_ = (const char *)value_cond_desc->value;
  compare_objects.push_back(comp);
// 根据filed名去找该表合适的索引元信息
//   const char* attibute_name[5];
  const char *attibute_name = field_meta->name();
  std::vector<std::string> attribute_names;
  attribute_names.push_back(attibute_name);


  const IndexMeta *index_meta =
      table_meta_.find_index_by_field(attribute_names);
  if (nullptr == index_meta) {
    return nullptr;
  }

  Index *index = find_index(index_meta->name());
  if (nullptr == index) {
    return nullptr;
  }
  
  return index->create_scanner(compare_objects);
}

IndexScanner *Table::find_index_for_scan(const ConditionFilter *filter) {
  if (nullptr == filter) {
    return nullptr;
  }

  // remove dynamic_cast
//   zt 查看是否是默认条件过滤 也就是只有一个过滤条件
  const DefaultConditionFilter *default_condition_filter =
      dynamic_cast<const DefaultConditionFilter *>(filter);
  if (default_condition_filter != nullptr) {
    return find_index_for_scan(*default_condition_filter);
  }

  const CompositeConditionFilter *composite_condition_filter = dynamic_cast<const CompositeConditionFilter *>(filter);
  if (composite_condition_filter != nullptr) {
    // int filter_num = composite_condition_filter->filter_num();
    // // zt 在每一个条件上进行查找索引，但是 目前处理的方法是找到第一个符合条件的索引就返回 
    // for (int i = 0; i < filter_num; i++) {
    //   IndexScanner *scanner =
    //       find_index_for_scan(&composite_condition_filter->filter(i));
    //   if (scanner != nullptr) {
    //     return scanner;  // 可以找到一个最优的，比如比较符号是=
    //   }
    // }
    return find_index_for_scan(*composite_condition_filter);
  }
  return nullptr;
}

IndexScanner *Table::find_index_for_scan(const CompositeConditionFilter &filters){
  int filter_num = filters.filter_num();
  std::vector<std::string> attribute_names;
  std::vector<CompareObject> compare_objects;
  for (int i = 0; i < filter_num; i++)
  {
    const DefaultConditionFilter *filter = dynamic_cast<const DefaultConditionFilter * >(&(filters.filter(i)));
    if(nullptr != filter){
        const ConDesc *field_cond_desc = nullptr;
        const ConDesc *value_cond_desc = nullptr;
        if (filter->left().is_attr && !filter->right().is_attr) {
        field_cond_desc = &filter->left();
        value_cond_desc = &filter->right();
        } else if (filter->right().is_attr && !filter->left().is_attr) {
        field_cond_desc = &filter->right();
        value_cond_desc = &filter->left();
        }

        if (field_cond_desc == nullptr || value_cond_desc == nullptr) {
            return nullptr;
        }

        const FieldMeta *field_meta =
            table_meta_.find_field_by_offset(field_cond_desc->attr_offset);
        if (nullptr == field_meta) {
            LOG_PANIC("Cannot find field by offset %d. table=%s",
                    field_cond_desc->attr_offset, name());
            return nullptr;
        }

        const char *attibute_name = field_meta->name();
        attribute_names.push_back(attibute_name);

        CompareObject comp;
        comp.comp_op_ = filter->comp_op();
        comp.value_ = (const char *)value_cond_desc->value;
        compare_objects.push_back(comp);
    }else {
        LOG_DEBUG("Can't convert ConditionFilter to DefaultConditionFilter");
    }
  }
  
  const IndexMeta *index_meta =
      table_meta_.find_index_by_field(attribute_names);
  if (nullptr == index_meta) {
    return nullptr;
  }

  Index *index = find_index(index_meta->name());
  if (nullptr == index) {
    return nullptr;
  }

  return index->create_scanner(compare_objects);

}
RC Table::sync() {
  RC rc = data_buffer_pool_->flush_all_pages(file_id_);
  if (rc != RC::SUCCESS) {
    LOG_ERROR("Failed to flush table's data pages. table=%s, rc=%d:%s", name(),
              rc, strrc(rc));
    return rc;
  }

  for (Index *index : indexes_) {
    rc = index->sync();
    if (rc != RC::SUCCESS) {
      LOG_ERROR("Failed to flush index's pages. table=%s, index=%s, rc=%d:%s",
                name(), index->index_meta().name(), rc, strrc(rc));
      return rc;
    }
  }
  LOG_INFO("Sync table over. table=%s", name());
  return rc;
}