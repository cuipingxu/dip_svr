
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

 

#ifndef _REQUEST_H
#define _REQUEST_H

#include "base/base_common.h"
#include "base/base_smart_ptr_t.h"


USING_NS_BASE;

class Request : public RefCounter
{
public:
	Request();

	~Request();

	void log();

	std::string to_string();

public:
	std::string _msg_id;  //内部设置(唯一id)
	std::string _req_id;  //前端设置
	std::string _req;  //追加了_msg_id的请求串
	int _fd;   //前端fd
	std::string _channel;
	std::string _ip; //前端ip
	unsigned short _port; //前端port
	std::string _peer; //前端ip
	unsigned long long _stmp; //消息创建时间戳
	unsigned long long _app_stmp; //外部应用填写的时间戳
	std::string _worker_id;
	std::string _group_id;
	std::string _session_id;
	
};
typedef Smart_Ptr_T<Request>  Request_Ptr;


#endif


