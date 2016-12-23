
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
 

#ifndef _STANDARD_SERVICE_H
#define _STANDARD_SERVICE_H

#include "libcomm/service_interface.h"

class standard_service : public service_object
{
public:
    standard_service();
    virtual ~standard_service();
    
    virtual int init(service_args* args);
    virtual int process(const std::string& req, std::string& rsp, std::string& debug_info, bool &is_obj);
    virtual int release();

private:
	std::string new_url(const std::string &body, StProtocol &stProtocol);
	
		
private:
	service_args *_args;
};

#endif

