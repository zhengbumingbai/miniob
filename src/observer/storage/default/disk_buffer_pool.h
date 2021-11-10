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
#ifndef __OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_
#define __OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_

#include <cstdint>
#include <fcntl.h>
#include <cstdio>
#include <sys/types.h>
#include <cstring>
#include <sys/stat.h>
#include <ctime>

#include "common/log/log.h"
#include <list>
#include <vector>
#include <unordered_map>

#include "rc.h"

typedef int PageNum;

using namespace common;
//
#define BP_INVALID_PAGE_NUM (-1)
#define BP_PAGE_SIZE (1 << 12)
#define BP_PAGE_DATA_SIZE (BP_PAGE_SIZE - sizeof(PageNum))
#define BP_FILE_SUB_HDR_SIZE (sizeof(BPFileSubHeader))
#define BP_BUFFER_SIZE 50
#define MAX_OPEN_FILE 1024

typedef struct {
  PageNum page_num;
  char data[BP_PAGE_DATA_SIZE];
} Page;
// sizeof(Page) should be equal to BP_PAGE_SIZE

typedef struct {
  PageNum page_count;
  int allocated_pages;
} BPFileSubHeader;

typedef struct {
  bool dirty;
  unsigned int pin_count;
  // unsigned long acc_time;
  int file_desc;
  Page page;
} Frame;

typedef struct {
  bool open;
  Frame *frame;
} BPPageHandle;

class BPFileHandle{
public:
  BPFileHandle() {
    memset(this, 0, sizeof(*this));
  }

public:
  bool bopen;
  const char *file_name;
  int file_desc;
  Frame *hdr_frame;
  Page *hdr_page;
  char *bitmap;
  BPFileSubHeader *file_sub_header;
} ;

struct pair_hash
{
  template <class T1, class T2>
  std::size_t operator() (const std::pair<T1, T2> &pair) const {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

class BPManager {
public:
  BPManager(int size = BP_BUFFER_SIZE) {
    this->size_ = size;
    frame_ = new Frame[size];
    allocated_ = new bool[size];
    for (int i = 0; i < size; i++) {
      allocated_[i] = false;
      frame_[i].pin_count = 0;
      replacer_list.push_back(&frame_[i]);
    }
  }

  ~BPManager() {
    delete[] frame_;
    delete[] allocated_;
    size_ = 0;
    frame_ = nullptr;
    allocated_ = nullptr;
  }

  RC alloc(int file_desc, PageNum page_num, Frame*& frame) {
    if (replacer_list.size() == 0) {
      return RC::BUFFERPOOL_NOBUF;
    }

    frame = replacer_list.front();
    uint64_t frame_pos = frame - frame_; 
    replacer_list.pop_front();
    
    if (frame->dirty) {
      RC rc = flush_block(frame);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to flush block of %d for %d.", frame_pos, frame->file_desc);
        return rc;
      }
    }

    memset(frame, 0, sizeof(Frame));
    
    auto pair_rm_iter = frame_page_map.find(frame);
    if (pair_rm_iter != frame_page_map.end()) {
      auto pair_rm = pair_rm_iter->second;
      frame_rep_pos_map.erase(frame);
      frame_page_map.erase(pair_rm_iter);
      page_frame_pos_map.erase(pair_rm);
    }
    
    auto pair_new = std::pair<int, PageNum>(file_desc, page_num);
    page_frame_pos_map[pair_new] = frame_pos;
    frame_page_map[frame] = pair_new;

    frame->pin_count = 1;
    frame->file_desc = file_desc;
    frame->page.page_num = page_num;
    allocated_[frame_pos] = true;
    return RC::SUCCESS;

    // for (auto &it : frame_list)
    // {
    //     LOG_ERROR( "fd: %d page: %d\n", it->file_desc, it->page.page_num);
    // }
    // LOG_ERROR( "---------------------------------------------------------");
    // return nullptr; // TODO for test
  }

  RC get(int file_desc, PageNum page_num, Frame*& frame_get) {
    auto pair_seek = std::pair<int, PageNum>(file_desc, page_num);
    
    auto frame_pos_iter = page_frame_pos_map.find(pair_seek);
    if (frame_pos_iter == page_frame_pos_map.end()) {
      return RC::BUFFERPOOL_EOF;
    }
    uint64_t frame_pos = frame_pos_iter->second;
    frame_get = frame_ + frame_pos;

    RC rc = pin(frame_get);
    return rc;
  }

  RC unpin(Frame* frame) {
    if (frame->pin_count != 0) {
      frame->pin_count--;
    }
    if (frame->pin_count == 0) {
      // 放到 replacer_list 中
      replacer_list.push_back(frame);
      frame_rep_pos_map[frame] = --replacer_list.end();
    }
    return RC::SUCCESS;
  }

  RC pin(Frame* frame) {
    if (frame->pin_count == 0) {
      // 从 replacer_list 里面拿出来。（如果拿不到出错否？）
      auto rep_pos_iter = frame_rep_pos_map.find(frame);
      if (rep_pos_iter != frame_rep_pos_map.end()) {
        auto rep_pos = rep_pos_iter->second;
        replacer_list.erase(rep_pos);
        frame_rep_pos_map.erase(rep_pos_iter);
      } else {
        LOG_DEBUG("Frame must in replacer list when pin_count == 0");
      }
    }
    frame->pin_count++;
    return RC::SUCCESS;
  }

  RC flush_block(Frame *frame)
  {
    // The better way is use mmap the block into memory,
    // so it is easier to flush data to file.
    s64_t offset = ((s64_t)frame->page.page_num) * sizeof(Page);
    if (lseek(frame->file_desc, offset, SEEK_SET) == offset - 1) {
      LOG_ERROR("Failed to flush page %lld of %d due to failed to seek %s.", offset, frame->file_desc, strerror(errno));
      return RC::IOERR_SEEK;
    }

    if (write(frame->file_desc, &(frame->page), sizeof(Page)) != sizeof(Page)) {
      LOG_ERROR("Failed to flush page %lld of %d due to %s.", offset, frame->file_desc, strerror(errno));
      return RC::IOERR_WRITE;
    }
    frame->dirty = false;
    LOG_DEBUG("Flush block. file desc=%d, page num=%d", frame->file_desc, frame->page.page_num);

    return RC::SUCCESS;
  }

  RC evict_frame(Frame* frame) {
    if (frame->pin_count != 0) {
      LOG_WARN("Begin to free page %d of %d, but it's pinned.", frame->page.page_num, frame->file_desc);
      return RC::BUFFERPOOL_PAGE_PINNED;
    }
    
    // if (frame->dirty) {
    //   RC rc = flush_block(frame);
    //   if (rc != RC::SUCCESS) {
    //     LOG_WARN("Failed to flush block %d of %d during dispose block.", frame->page.page_num, frame->file_desc);
    //     return rc;
    //   }
    // }

    replacer_list.push_back(frame);
    auto frame_page_iter = frame_page_map.find(frame);
    if (frame_page_iter != frame_page_map.end()) {
      auto page_pair = frame_page_iter->second;
      page_frame_pos_map.erase(page_pair);
      frame_page_map.erase(frame_page_iter);
    }
    
    frame->dirty = false;
    uint64_t pos = frame - frame_;
    allocated_[pos] = false;
    LOG_DEBUG("evict block frame =%p", frame);
    return RC::SUCCESS;
  }

  RC force_evict_frame(Frame* frame) {
    if (frame->dirty) {
      RC rc = flush_block(frame);
      if (rc != RC::SUCCESS) {
        LOG_WARN("Failed to flush block %d of %d during dispose block.", frame->page.page_num, frame->file_desc);
        return rc;
      }
    }
    
    replacer_list.push_back(frame);
    auto frame_page_iter = frame_page_map.find(frame);
    if (frame_page_iter != frame_page_map.end()) {
      auto page_pair = frame_page_iter->second;
      page_frame_pos_map.erase(page_pair);
      frame_page_map.erase(frame_page_iter);
    }

    frame->dirty = false;
    uint64_t pos = frame - frame_;
    allocated_[pos] = false;
    LOG_DEBUG("force evict block frame =%p", frame);
    return RC::SUCCESS;
  }

  RC force_evict_all_frames_from_file(BPFileHandle *file_handle) {
    for (int i = 0; i < BP_BUFFER_SIZE; i++) {
      if (!allocated_[i])
        continue;

      if (frame_[i].file_desc != file_handle->file_desc)
        continue;

      RC rc = force_evict_frame(&frame_[i]);
      if (rc != RC::SUCCESS) {
        LOG_ERROR("Failed to flush all pages' of %s.", file_handle->file_name);
        return rc;
      }
      allocated_[i] = false;
    }
    return RC::SUCCESS;
  }

  Frame *getFrame() { return frame_; }

  bool *getAllocated() { return allocated_; }

private:
  int size_;
  Frame * frame_ = nullptr;
  bool *allocated_ = nullptr;
  std::list<Frame *> replacer_list;
  // LRU后面再做
  std::unordered_map<std::pair<int, PageNum>, uint64_t, pair_hash> page_frame_pos_map;
  std::unordered_map<Frame*, std::list<Frame *>::iterator> frame_rep_pos_map;
  std::unordered_map<Frame*, std::pair<int, PageNum>> frame_page_map;
};

class DiskBufferPool {
public:
  /**
  * 创建一个名称为指定文件名的分页文件
  */
  RC create_file(const char *file_name);

  /**
   * 根据文件名打开一个分页文件，返回文件ID
   * @return
   */
  RC open_file(const char *file_name, int *file_id);

  /**
   * 关闭fileID对应的分页文件
   */
  RC close_file(int file_id);

  /**
   * 根据文件ID和页号获取指定页面到缓冲区，返回页面句柄指针。
   * @return
   */
  RC get_this_page(int file_id, PageNum page_num, BPPageHandle *page_handle);

  /**
   * 在指定文件中分配一个新的页面，并将其放入缓冲区，返回页面句柄指针。
   * 分配页面时，如果文件中有空闲页，就直接分配一个空闲页；
   * 如果文件中没有空闲页，则扩展文件规模来增加新的空闲页。
   */
  RC allocate_page(int file_id, BPPageHandle *page_handle);

  /**
   * 根据页面句柄指针返回对应的页面号
   */
  RC get_page_num(BPPageHandle *page_handle, PageNum *page_num);

  /**
   * 根据页面句柄指针返回对应的数据区指针
   */
  RC get_data(BPPageHandle *page_handle, char **data);

  /**
   * 丢弃文件中编号为pageNum的页面，将其变为空闲页
   */
  RC dispose_page(int file_id, PageNum page_num);

  /**
   * 释放指定文件关联的页的内存， 如果已经脏， 则刷到磁盘，除了pinned page
   * @param file_handle
   * @param page_num 如果不指定page_num 将刷新所有页
   */
  RC force_page(int file_id, PageNum page_num);

  /**
   * 标记指定页面为“脏”页。如果修改了页面的内容，则应调用此函数，
   * 以便该页面被淘汰出缓冲区时系统将新的页面数据写入磁盘文件
   */
  RC mark_dirty(BPPageHandle *page_handle);

  /**
   * 此函数用于解除pageHandle对应页面的驻留缓冲区限制。
   * 在调用GetThisPage或AllocatePage函数将一个页面读入缓冲区后，
   * 该页面被设置为驻留缓冲区状态，以防止其在处理过程中被置换出去，
   * 因此在该页面使用完之后应调用此函数解除该限制，使得该页面此后可以正常地被淘汰出缓冲区
   */
  RC unpin_page(BPPageHandle *page_handle);

  /**
   * 获取文件的总页数
   */
  RC get_page_count(int file_id, int *page_count);

  RC flush_all_pages(int file_id);

protected:
  RC allocate_block(Frame **buf, int file_desc, PageNum page_num);
  RC dispose_block(Frame *buf);

  /**
   * 刷新指定文件关联的所有脏页到磁盘，除了pinned page
   * @param file_handle
   * @param page_num 如果不指定page_num 将刷新所有页
   */
  RC force_page(BPFileHandle *file_handle, PageNum page_num);
  RC force_all_pages(BPFileHandle *file_handle);
  RC check_file_id(int file_id);
  RC check_page_num(PageNum page_num, BPFileHandle *file_handle);
  RC load_page(PageNum page_num, BPFileHandle *file_handle, Frame *frame);

private:
  BPManager bp_manager_;
  BPFileHandle *open_list_[MAX_OPEN_FILE] = {nullptr};
};

DiskBufferPool *theGlobalDiskBufferPool();

#endif //__OBSERVER_STORAGE_COMMON_PAGE_MANAGER_H_
