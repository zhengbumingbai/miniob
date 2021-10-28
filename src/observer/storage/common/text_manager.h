#ifndef E89A49CA_1E0F_4422_82F3_97C54F1C20B2
#define E89A49CA_1E0F_4422_82F3_97C54F1C20B2

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>

#include "common/lang/bitmap.h"
#include "rc.h"

const static int TEXT_SIZE = 4096;

class TextManager {
 public:
  TextManager();
  ~TextManager();
  RC WriteText(int *offset, char *data, int length);
  RC ReadText(int offset, char *data, int length);
  RC GetLen(int offset, int *len);
  RC CloseFile();

 private:
  int fd_;
  int offset_;  //下一个可存储位置
  //   int length_[1024];
};

#endif /* E89A49CA_1E0F_4422_82F3_97C54F1C20B2 */
