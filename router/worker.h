
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

 

#ifndef _WORKER_H
#define _WORKER_H

#include "base/base_common.h"
#include "base/base_smart_ptr_t.h"
#include "base/base_thread_mutex.h"


USING_NS_BASE;


class Worker : public RefCounter
{
public:
	Worker();

	~Worker();

	bool operator ==(const Worker &worker);

	int send_req(const std::string &req, const std::string &req_id, const std::string &msg_id);

	int send_rsp(const std::string &cmd, const std::string &req_id, const std::string &msg_id, int err, const std::string &error_info="");

	void log();

	std::string to_string();
	
public:
	unsigned int _index;  //���
	std::string _id;  //Worker_IP_seq
	int _fd;  //ÿ��woker ��Ψһ��fd
	std::string _ip;
	unsigned short _port;
	std::string _group_id;
	unsigned long long _create_stmp; //worker ע��ʱ���
	unsigned long long _hb_stmp; //worker ��������ʱ���
	unsigned int _unregister_failed_num;  //�޳�worker ������ʧ����
	unsigned int _unregister_timeout_num; //�޳�worker ������ʱ��
	
	Thread_Mutex _mutex;
	
};

typedef Smart_Ptr_T<Worker>  Worker_Ptr;

#endif


