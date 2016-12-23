
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
 

#ifndef _PROTOCOL_H
#define _PROTOCOL_H

#include "base/base_common.h"
#include "common.h"

USING_NS_BASE;


class XProtocol
{
public:
	static int get_req_head(const std::string &req, std::string &cmd, unsigned long long &time, std::string &req_id, 
		std::string &channel, std::string &peer, std::string &service_id, std::string &server_id);

	static int get_admin_head(const std::string &req, std::string &cmd, unsigned long long &time, 
		std::string &msg_id, std::string &req_id, int &err, std::string &err_info);
	
	static int get_worker_info(const std::string &req, std::string &worker_id, 
		std::string &group_id, std::string &service_id, std::string &server_id, std::string &cmd);
	
	static int get_rsp_staus(const std::string &req, std::string &cmd, unsigned long long &time, 
		std::string &msg_id, int &err, std::string &err_info);

	static int get_body(const std::string &req, std::string &cmd, std::string &body);

	static int get_notification(const std::string &req, std::string &cmd, 
		std::string &title, std::string &content, std::string &sender, 
		std::vector<std::string> &receiver, std::vector<std::string> &groups, std::vector<std::string> &CC, std::string &err_msg);

	static int get_openid_list(const std::string &rsp, std::map<std::string, std::string> &openid_list);

	static int get_eqq_group_list(const std::string &rsp, std::map<std::string, std::string> &group_list);

	static int get_eqq_rsp_status(const std::string &rsp, std::string &err_msg);

	static std::string set_hb(const std::string &msg_id);

	static std::string set_register(const std::string &msg_id, const std::string &worker_id, 
		std::string &group_id, std::string &service_id, std::string &server_id, std::string &cmd);

	static std::string set_unregister(const std::string &msg_id, const std::string &worker_id);

	static std::string set_rsp_msg(const std::string &cmd, const std::string &req_id, const std::string &msg_id,  
		const int err, const std::string &error_info, const std::string &body="", bool is_object = false);

	static std::string set_new_req(const std::string &req, const std::string &msg_id);
	
	
};


#endif


