
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

 

#ifndef _WORKER_EVENT_HANDLER_H
#define _WORKER_EVENT_HANDLER_H

#include "base/base_common.h"
#include "base/base_event_handler.h"

USING_NS_BASE;

class Worker_Event_Handler : public Event_Handler
{	
public:
	Worker_Event_Handler();

	virtual ~Worker_Event_Handler();

	//handle_xxxx 成功返回0，失败返回非0
	
	//处理建立连接请求事件
	virtual int handle_accept(int fd);

	//处理读事件
	virtual int handle_input(int fd);

	//处理连接关闭事件
	virtual int handle_close(int fd);

	/*
	这个接口子类必须实现, 样例如下:
	virtual Event_Handler* renew(){return new Event_Handler;};
	*/
	virtual Event_Handler* renew();
	
private:
	std::string _buf;
	
};


#endif


