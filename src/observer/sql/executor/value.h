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
// Created by Wangyunlai on 2021/5/14.
//

#ifndef __OBSERVER_SQL_EXECUTOR_VALUE_H_
#define __OBSERVER_SQL_EXECUTOR_VALUE_H_

#include <string.h>

#include <string>
#include <ostream>
#include "common/log/log.h"

//属性值类型
typedef enum { VALUETYPE_UNDEFINED, STRING, INT, FLOAT, DATE, NULLTYPE } ValueType;

class TupleValue
{
public:
    TupleValue() = default;
    virtual ~TupleValue() = default;

    virtual void to_string(std::ostream &os) const = 0;
    virtual int compare(const TupleValue &other) const = 0;
    virtual ValueType type() const = 0;
private:
};

void unixTime2Str(int n, char strTime[], int bufLen);

class DateValue : public TupleValue
{
public:
    explicit DateValue(int value) : value_(value)
    {
    }

    void to_string(std::ostream &os) const override
    {
        char str_time[50];
        unixTime2Str(value_, str_time, 50);
        os << std::string(str_time);
        // LOG_DEBUG("date to string end");
    }

    int compare(const TupleValue &other) const override
    {
        const DateValue &int_other = (const DateValue &)other;
        return value_ - int_other.value_;
    }

    ValueType type() const override
    {
        return ValueType::DATE;
    }

    bool bigger_than(const int other)
    {
        return value_ > other;
    }

    int value() const{
        return value_;
    }

    void replace(int value) {
        value_ = value;
    }

private:
    int value_;
};

class IntValue : public TupleValue
{
public:
    explicit IntValue(int value) : value_(value)
    {
    }

    void to_string(std::ostream &os) const override
    {
        os << value_;
    }

    int compare(const TupleValue &other) const override
    {
        const IntValue &int_other = (const IntValue &)other;
        return value_ - int_other.value_;
    }

    ValueType type() const override
    {
        return ValueType::INT;
    }

    bool bigger_than(const int other)
    {
        return value_ > other;
    }

    void add(int b) {
        value_ += b;
    }

    int value() const{
        return value_;
    }

    void replace(int value) {
        value_ = value;
    }

private:
    int value_;
};

class FloatValue : public TupleValue
{
public:
    explicit FloatValue(float value) : value_(value)
    {
    }

    void to_string(std::ostream &os) const override
    {
        // 先保存两位小数 再转化为字符串处理
      char buf[128];
      sprintf(buf, "%.2f", value_);
    //   std::string s(buf);
      for (size_t i = 0; i < 128; i++) {
        if (buf[i] == '.') {
          // 最后一位
          if (i + 2 < 128 && buf[i + 2] == '0') {
            buf[i + 2] = '\0';

            if (i + 1 < 128 && buf[i + 1] == '0') {
              buf[i] = '\0';
            }
          }
          break;
        }
      }

        os << buf;
    }

    int compare(const TupleValue &other) const override
    {
        const FloatValue &float_other = (const FloatValue &)other;
        float result = value_ - float_other.value_;
        if (result > 0)
        { // 浮点数没有考虑精度问题
            return 1;
        }
        if (result < 0)
        {
            return -1;
        }
        return 0;
    }

    ValueType type() const override
    {
        return ValueType::FLOAT;
    }

    bool bigger_than(const float other)
    {
        return value_ > other;
    }

    void add(float b) {
        value_ += b;
    }

    float value() const{
        return value_;
    }

    void replace(float value) {
        value_ = value;
    }
private:
    float value_;
};

class StringValue : public TupleValue
{
public:
    StringValue(const char *value, int len)
    {
        value_ = std::string(value, len);
    }
    explicit StringValue(const char *value)
    {
        value_ = std::string(value);
    }

    void to_string(std::ostream &os) const override
    {
        os << value_;
    }

    int compare(const TupleValue &other) const override
    {
        const StringValue &string_other = (const StringValue &)other;
        return strcmp(value_.c_str(), string_other.value_.c_str());
    }

    ValueType type() const override
    {
        return ValueType::STRING;
    }

    std::string value() const{
        return value_;
    }

    void replace(std::string value) {
        value_ = value;
    }
private:
    std::string value_;
};

class NullValue : public TupleValue
{
public:
    explicit NullValue()
    {
    }

    void to_string(std::ostream &os) const override
    {
        os << "null";
    }

    int compare(const TupleValue &other) const override
    {
        return false;
    }

    ValueType type() const override
    {
        return ValueType::NULLTYPE;
    }
};

class TextValue : public TupleValue
{
public:
    TextValue(const char *value, int len) : value_(value, len)
    {
    }
    explicit TextValue(const char *value) : value_(value)
    {
    }

    void to_string(std::ostream &os) const override
    {
        os << value_;
    }

    int compare(const TupleValue &other) const override
    {
        const TextValue &string_other = (const TextValue &)other;
        return strcmp(value_.c_str(), string_other.value_.c_str());
    }

    ValueType type() const override
    {
        return ValueType::STRING;
    }

    std::string value() {
        return value_;
    }

    void replace(std::string value) {
        value_ = value;
    }
private:
    std::string value_;
};

#endif //__OBSERVER_SQL_EXECUTOR_VALUE_H_
