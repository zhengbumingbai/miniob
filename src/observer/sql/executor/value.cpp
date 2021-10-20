/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */
#include "value.h"
#include "common/log/log.h"

void unixTime2Str(int n, char *strTime, int bufLen)
{
    // LOG_DEBUG("unixTime2Str start");
    // printf("Output time: %ud\n", n);
    time_t unix_time = n;
    
    // time((time_t *)&unix_time);

    struct tm* tm = localtime(&unix_time);
    strftime(strTime, bufLen - 1, "%Y-%m-%d", tm);
    strTime[bufLen - 1] = '\0';
    // LOG_DEBUG("unixTime2Str end");
}