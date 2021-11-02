#ifndef BF951E0A_D183_4E90_9514_12BDA90A5AAE
#define BF951E0A_D183_4E90_9514_12BDA90A5AAE
#include <vector>

#include "string.h"
#include "tuple.h"

class tuple_sort {
 public:
  //   tuple_sort(/* args */);
  tuple_sort(bool is_single_table, const OrderAttr *order_attr,
             const int order_num, TupleSchema schema);
  ~tuple_sort();

  bool operator()(const Tuple &tuple1, const Tuple &tuple2) {
    for (int i = order_num - 1; i >= 0; i--) {
      int index = indexs[i];
      int order_type = order_attr[i].order_type;
      if (order_type == ASC_T) {
        if (tuple1.get(index).compare(tuple2.get(index)) < 0) {
          return true;
        } else if (tuple1.get(index).compare(tuple2.get(index)) == 0) {
          continue;
        } else {
          return false;
        }
      } else if (order_type == DESC_T) {
        if (tuple1.get(index).compare(tuple2.get(index)) > 0) {
          return true;
        } else if (tuple1.get(index).compare(tuple2.get(index)) == 0) {
          continue;
        } else {
          return false;
        }
      }
    }
    return true;
  }

 private:
  /* data */
  bool is_single_table;
  const OrderAttr *order_attr;
  const int order_num;
  TupleSchema schema;
  std::vector<int> indexs;
};

tuple_sort::tuple_sort(bool is_single_table, const OrderAttr *order_attr,
                       const int order_num, TupleSchema schema)
    : is_single_table(is_single_table),
      order_attr(order_attr),
      order_num(order_num),
      schema(schema) {
  //   构造出需要比较的位置偏移数组
  const std::vector<TupleField> fields = schema.fields();
  //   反向构造比较顺序
  if (is_single_table) {
    // 单表只需要确认每个属性在Tuple的位置
    for (int i = order_num - 1; i >= 0; i--) {
      for (int j = 0; j < fields.size(); j++) {
        if (strcmp(order_attr[i].attribute_name, fields[j].field_name()) == 0) {
          indexs.push_back(j);
          break;
        }
      }
    }
  } else {
    for (int i = order_num - 1; i >= 0; i--) {
      for (int j = 0; j < fields.size(); j++) {
        if (strcmp(order_attr[i].attribute_name, fields[j].field_name()) == 0 &&
            strcmp(order_attr[i].relation_name, fields[j].table_name()) == 0) {
          indexs.push_back(j);
          break;
        }
      }
    }
  }
}

// tuple_sort::tuple_sort(/* args */) {}

tuple_sort::~tuple_sort() {}

#endif /* BF951E0A_D183_4E90_9514_12BDA90A5AAE */
