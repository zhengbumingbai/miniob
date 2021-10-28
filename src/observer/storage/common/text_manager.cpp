#include "text_manager.h"

TextManager::TextManager() {
  fd_ = open("TEXT.data", O_RDWR | O_CREAT | O_EXCL, S_IREAD | S_IWRITE);
  read(fd_, &offset_, sizeof(int));
}

TextManager::~TextManager() {}

RC TextManager::WriteText(int *offset, char *data, int length) {
  int ret = 0;
  //   返回该TEXT的偏移
  *offset = offset_;
  // 写入数据到偏移处
  ret = lseek(fd_, *offset * TEXT_SIZE, SEEK_SET);
  if (ret == -1) return RC::GENERIC_ERROR;
  ret = write(fd_, data, length);
  if (ret == -1) return RC::GENERIC_ERROR;
  // 写入数据长度到第一页
  ret = lseek(fd_, *offset * 4, SEEK_SET);
  if (ret == -1) return RC::GENERIC_ERROR;
  
  ret = write(fd_, offset, 4);
  if (ret == -1) return RC::GENERIC_ERROR;
  return RC::SUCCESS;
}

RC TextManager::ReadText(int offset, char *data, int length) {
  int ret = 0;
  ret = lseek(fd_, offset * TEXT_SIZE, SEEK_SET);
  if (ret == -1) return RC::GENERIC_ERROR;
  ret = read(fd_, data, length);
  if (ret == -1) return RC::GENERIC_ERROR;
  return RC::SUCCESS;
}

RC TextManager::GetLen(int offset, int *len) {
  int ret = 0;
  ret = lseek(fd_, offset * 4, SEEK_SET);
  if (ret == -1) return RC::GENERIC_ERROR;
  ret = read(fd_, len, 4);
  if (ret == -1) return RC::GENERIC_ERROR;
  return RC::SUCCESS;
}

RC TextManager::CloseFile() {
  int ret = 0;
  ret = lseek(fd_, 0, SEEK_SET);
  if (ret == -1) return RC::GENERIC_ERROR;
  ret = write(fd_, &offset_, sizeof(offset_));
  if (ret == -1) return RC::GENERIC_ERROR;
  close(fd_);
  return RC::SUCCESS;
}