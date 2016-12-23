
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

 

#ifndef _WORKER_MGT_H
#define _WORKER_MGT_H

#include "base/base_common.h"
#include "base/base_thread_mutex.h"
#include "base/base_singleton_t.h"

#include "worker.h"

USING_NS_BASE;


//groupid <--->  workers
class Workers
{
public:
	Workers();

	~Workers();

	void register_worker(Worker_Ptr worker);

	//ͨ��fd ȥע��worker
	void unregister_worker(Worker_Ptr worker);	

	bool get_worker(Worker_Ptr &worker);

	void set_group_id(const std::string &group_id);

	bool empty();

	void log();

private:
	std::string _group_id;
	std::vector<Worker_Ptr> _workers;
	unsigned int _index; //��������
	
};



class Worker_Mgt
{
public:
	Worker_Mgt();

	virtual ~Worker_Mgt();

	void log();

	int register_worker(Worker_Ptr worker);

	void unregister_worker(Worker_Ptr worker);

	//ͨ��group id ȷ��һ�����ص�worker
	bool get_worker(const std::string &group_id, Worker_Ptr &worker);

	//ͨ��fd ����worker
	bool get_worker(int fd, Worker_Ptr &worker);
	
private:
	//group id <--->  Workers
	std::map<std::string, Workers> _groups;

	//fd <--->  worker
	std::map<int, Worker_Ptr> _fds;

	//worker id
	std::set<std::string> _workers;

	Thread_Mutex _mutex;
	unsigned int _index;
	
};


#define PSGT_Worker_Mgt Singleton_T<Worker_Mgt>::getInstance()


#endif


