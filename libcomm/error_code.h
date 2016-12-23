
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
 

#ifndef _ERROR_CODE_H
#define _ERROR_CODE_H


enum EnErrCode
{ 
	ERR_SUCCESS = 0,
	ERR_SYSTEM = -1,

	//common -200 ~ -2
	ERR_REQ_TIMEOUT = -2,               //请求超时
	ERR_COMMON_BEGIN = -200,
	ERR_QUEUE_FULL = -199,              //队列满
	ERR_INVALID_REQ = -198,             //请求串格式非法
	ERR_UNINITIALIZED = -197,           //没有初始化
	ERR_REACH_MAX_MSG = -196,           //请求串大于最大长度
	ERR_REACH_MIN_MSG = -195,           //请求串小于最小长度
	ERR_SEND_FAIL = -194,               //发送失败
	ERR_RCV_FAIL = -193,                //接收失败
	ERR_SESSION_TIMEOUT = -192,         //会话超时
	ERR_PUSH_QUEUE_FAIL = -191,         //插入队列失败
	
	//router -600 ~ -201
	ERR_ROUTER_BEGIN = -600,
	ERR_NO_GROUP_FOUND = -599,          //没有找到对应的分组
	ERR_NO_WORKER_FOUND = -598,         //没有找到对应的worker
	ERR_WORKER_REGISTERED = -587,       //worker注册失败

	//worker -900 ~ -601
	ERR_WORKER_BEGIN = -900,

	//libservice -1200 ~ -901
	ERR_LIBSERVICE_BEGIN = -1200,
	
	ERR_END
};


#endif


