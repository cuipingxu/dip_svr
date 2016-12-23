
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
 

#ifndef _EQQ_MGT_H
#define _EQQ_MGT_H

#include "base/base_thread_mutex.h"
#include "base/base_singleton_t.h"
#include "libcomm/service_interface.h"

USING_NS_BASE;

class eqq_mgt
{
public:
    eqq_mgt();
    virtual ~eqq_mgt();

	int init(service_args* args);

	int update_openid_list();

	int update_eqq_group_list();

	std::map<std::string, std::string> get_openid_list();

	std::map<std::string, std::string> get_eqq_group_list();
	
	int send_broadcast(std::string &title, std::string &content, std::string &receiver, std::string &groups, std::string &err_msg);

private:
	Thread_Mutex _mutex;
	service_args *_args;
	std::map<std::string, std::string> _openid;	
	std::map<std::string, std::string> _eqq_group_id;	
	
};


#define PSGT_EQQ_Mgt Singleton_T<eqq_mgt>::getInstance()

#endif

