
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

 
#ifndef _CONF_MGT_H
#define _CONF_MGT_H

#include "base/base_common.h"
#include "base/base_singleton_t.h"
#include "base/base_thread_mutex.h"
#include "common.h"


USING_NS_BASE;


typedef struct StProtocol
{
	std::string _cmd;
	std::string _mode;
	std::string _url;
	std::string _key;
	std::vector<std::string> _sign;

	StProtocol()
	{
		_cmd = "";
		_mode = "";
		_url = "";
		_key = "";
	}
	
} StProtocol;


typedef struct StEmail
{
	std::string _host;
	unsigned short _port;
	std::string _user;
	std::string _pwd;

	StEmail()
	{
		_host = "";
		_port = 0;
		_user = "";
		_pwd = "";
	}
	
} StEmail;


typedef struct StEQQ
{
	std::string _appid;
	std::string _company_id;
	std::string _company_token;

	StEQQ()
	{
		_appid = "";
		_company_id = "";
		_company_token = "";
	}
	
} StEQQ;



//以后新加存储系统， 需要增加新的api
class Conf_Mgt
{
public:
	Conf_Mgt();

	~Conf_Mgt();

	int init_router(const std::string &cfg);
	
	int init_worker(const std::string &cfg);

	int init_service_protocol(const std::string &cfg);

	int init_white_list(const std::string &cfg);

	int init_notification_cfg(const std::string &cfg);

	//-------------------
	
	int refresh_router_logsys();

	int refresh_worker_logsys();

	int refresh_logsys(const std::string &tag);

	int refresh_routers();

	int refresh_service_protocol();

	int refresh_white_list();

	//获取副本
	std::vector<StServer> get_logsys();

	std::vector<StServer> get_routers();

	std::map<std::string, StProtocol> get_service_protocol();

	std::set<std::string> get_white_list();
	
public:	
	//common
	std::string _id;
	unsigned short _thr_num;
	unsigned int _max_queue_size;
	unsigned int _rate_limiting;
	std::vector<StServer> _logsys;
	std::set<std::string> _white_list;  //白名单列表
	std::string _category;
	std::string _regional;

	//router
	unsigned short _req_port;
	unsigned short _worker_port;
	unsigned int _unregister_failed_num;
	unsigned int _unregister_timeout_num;

	//worker
	std::string _group_id;
	std::string _service_id;
	std::string _server_id;
	std::string _cmd;
	std::string _plug_in;
	std::string _conf;
	std::vector<StServer> _routers;
	
	//service protocol
	std::string _key;
	unsigned int _conn_timeout;
	unsigned int _rw_timeout;
	std::map<std::string, StProtocol> _protocols;   //业务协议列表

	//notification protocol
	StEmail _stEmail;
	StEQQ _stEQQ;

private:
	Thread_Mutex _mutex;  //刷新数据需要锁保护
	std::string _cfg_router;
	std::string _cfg_worker;
	std::string _cfg_white_list;
	std::string _cfg_service_protocol;
	std::string _cfg_noticication;
	
};

#define PSGT_Conf_Mgt Singleton_T<Conf_Mgt>::getInstance()


#endif


