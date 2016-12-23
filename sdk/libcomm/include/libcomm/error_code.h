
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
	ERR_REQ_TIMEOUT = -2,               //����ʱ
	ERR_COMMON_BEGIN = -200,
	ERR_QUEUE_FULL = -199,              //������
	ERR_INVALID_REQ = -198,             //���󴮸�ʽ�Ƿ�
	ERR_UNINITIALIZED = -197,           //û�г�ʼ��
	ERR_REACH_MAX_MSG = -196,           //���󴮴�����󳤶�
	ERR_REACH_MIN_MSG = -195,           //����С����С����
	ERR_SEND_FAIL = -194,               //����ʧ��
	ERR_RCV_FAIL = -193,                //����ʧ��
	ERR_SESSION_TIMEOUT = -192,         //�Ự��ʱ
	ERR_PUSH_QUEUE_FAIL = -191,         //�������ʧ��
	
	//router -600 ~ -201
	ERR_ROUTER_BEGIN = -600,
	ERR_NO_GROUP_FOUND = -599,          //û���ҵ���Ӧ�ķ���
	ERR_NO_WORKER_FOUND = -598,         //û���ҵ���Ӧ��worker
	ERR_WORKER_REGISTERED = -587,       //workerע��ʧ��

	//worker -900 ~ -601
	ERR_WORKER_BEGIN = -900,

	//libservice -1200 ~ -901
	ERR_LIBSERVICE_BEGIN = -1200,
	
	ERR_END
};


#endif


