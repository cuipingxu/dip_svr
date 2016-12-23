
/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: 89617663@qq.com
 */
 

#ifndef _COMMON_H
#define _COMMON_H

#include "base/base_common.h"


const unsigned int MIN_MSG_LEN = 50;       //最小消息长度{"head":{"cmd":"hb", "time":111111, "msg_id":"xxxxxx"}}
const unsigned int MAX_MSG_LEN = 102400;   //最大消息长度100KB


typedef struct StServer
{
   	std::string _ip;
	unsigned short _port;
	
} StServer;

#endif


