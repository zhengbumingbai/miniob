#ifndef E89A49CA_1E0F_4422_82F3_97C54F1C20B2
#define E89A49CA_1E0F_4422_82F3_97C54F1C20B2

#include "common/lang/bitmap.h"
#include <string>
#include <fcntl.h>

class TextManager {
 public:
    TextManager();
    size_t get_page_num();
    // store_text();
 private:
    int fd_;
    char *bitmap_;
    int bitmap_size_ ;
    std::string filename_;
};

#endif /* E89A49CA_1E0F_4422_82F3_97C54F1C20B2 */
