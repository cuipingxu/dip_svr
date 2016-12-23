
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

#ifndef _SERVICE_INTERFACE_H
#define _SERVICE_INTERFACE_H


#include "base/base_common.h"
#include "base/base_logger.h"
#include "conf_mgt.h"

#define DLL_MAX_ARGS_EXT  10

struct service_args 
{
    base::Logger              *logger_debug;
	Conf_Mgt				  *conf_mgt;
    int                       argc_ext;
    void*                     argv_ext[DLL_MAX_ARGS_EXT];
};


class service_object
{
public:
    service_object() {}
    virtual ~service_object() {}
    
    virtual int init(service_args* args) = 0;
    virtual int process(const std::string& req, std::string& rsp, std::string& debug_info, bool &is_obj) = 0;
    virtual int release() = 0;
	
protected:
	service_args *_args;
};


#ifdef __cplusplus
extern "C"
{
#endif

typedef int (*service_func_t)(service_object *&);

int get_service_object(service_object *&obj);

#ifdef __cplusplus
}
#endif

#endif

