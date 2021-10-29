#include "text_manager.h"

TextManager::TextManager() {
  fd_ = open("TEXT.data", O_RDWR | O_CREAT, S_IREAD | S_IWRITE);
  if (fd_ < 0) {
    perror("TextManager()");
  }
  int ret = 0;
  //   ret = lseek(fd_, 0, SEEK_SET);
  //   if (ret == -1) {
  //     perror("TextManager()");
  //   }
  ret = read(fd_, &offset_, sizeof(int));
  if (ret == -1) {
    perror("TextManager()");
  }
  if (ret == EOF || offset_ == 0) {
    offset_ = 1;
    LOG_DEBUG("First open file.");
  }
}

TextManager::~TextManager() {}

RC TextManager::WriteText(int *offset, char *data, int length) {
  int ret = 0;
  //   返回该TEXT的偏移
  *offset = offset_;
  // 写入数据到偏移处
  ret = lseek(fd_, offset_ * TEXT_MAX_LENGTH, SEEK_SET);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  ret = write(fd_, data, length);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }

  // 写入数据长度到第一页
  ret = lseek(fd_, offset_ * 4, SEEK_SET);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }

  ret = write(fd_, &length, 4);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }

  offset_++;
  return RC::SUCCESS;
}

RC TextManager::ReadText(int offset, char *data, int length) {
  int ret = 0;
  ret = lseek(fd_, offset * TEXT_MAX_LENGTH, SEEK_SET);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  ret = read(fd_, data, length);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  return RC::SUCCESS;
}

RC TextManager::GetLen(int offset, int *len) {
  int ret = 0;
  ret = lseek(fd_, offset * 4, SEEK_SET);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  ret = read(fd_, len, 4);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  return RC::SUCCESS;
}

RC TextManager::CloseFile() {
  int ret = 0;
  ret = lseek(fd_, 0, SEEK_SET);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  ret = write(fd_, &offset_, sizeof(offset_));
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  ret = close(fd_);
  if (ret == -1) {
    perror("TextManager()");
    return RC::GENERIC_ERROR;
  }
  return RC::SUCCESS;
}