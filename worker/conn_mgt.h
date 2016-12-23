
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

 

#ifndef _CONN_MGT_H
#define _CONN_MGT_H

#include "base/base_common.h"
#include "base/base_queue.h"
#include "base/base_singleton_t.h"
#include "base/base_thread_mutex.h"
#include "base/base_timer_select.h"
#include "conn.h"

USING_NS_BASE;

class XConn_Mgt
{
public:
	XConn_Mgt();

	~XConn_Mgt();

	//��ʼ�������ӳ�
	int init();

	void refresh();

	//ͨ��hb  ��ʱ������г�����
	void check();

	void stop_check();

	bool get_conn(const std::string &router_id, XConn_Ptr &conn);

	void unregister_workers();
	
private:
	//router id <---> conn
	std::map<std::string, XConn_Ptr> _conns;
	Thread_Mutex _mutex;

	Select_Timer *timer_req;
	
};

#define PSGT_XConn_Mgt Singleton_T<XConn_Mgt>::getInstance()


#endif


