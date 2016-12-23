
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

 

#ifndef _CONN_H
#define _CONN_H

#include "base/base_common.h"
#include "base/base_smart_ptr_t.h"
#include "base/base_tcp_client_epoll.h"


USING_NS_BASE;

class XConn : public RefCounter
{
public:
	XConn(const std::string ip, unsigned short port);

	~XConn();

	int connect();

	void close();

	bool is_close();
		
	void log();

	std::string to_string();

	int register_worker();

	int unregister_worker();
	
	int hb();

public:
	std::string _ip; //前端ip
	unsigned short _port; //前端port
	unsigned long long _stmp_create; //连接创建时间戳
	unsigned long long _stmp_close;  //连接关闭时间戳
	unsigned long long _stmp_hb;     //最后心跳时间戳
	TCP_Client_Epoll *_client;

	//是否已经注册
	bool _registered;
	
};

typedef Smart_Ptr_T<XConn>  XConn_Ptr;



#endif


