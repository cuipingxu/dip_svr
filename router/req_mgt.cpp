

#include "req_mgt.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"
#include "libcomm/error_code.h"
#include "libcomm/conf_mgt.h"
#include "logclient/debugger.h"

extern Args_Parser g_args_parser;
extern Logger g_logger_debug;
extern Debugger g_debugger;


Req_Mgt::Req_Mgt()
{
	_queue = new X_Queue<Request_Ptr>(PSGT_Conf_Mgt->_max_queue_size, PSGT_Conf_Mgt->_rate_limiting);
}


Req_Mgt::~Req_Mgt()
{


}

int Req_Mgt::push_req(Request_Ptr req)
{
	return _queue->push(req);
}


int Req_Mgt::get_req(Request_Ptr &req)
{
	return _queue->pop(req);	
}


bool Req_Mgt::full()
{
	return _queue->full();
}


unsigned int Req_Mgt::count()
{
	return _queue->size();
}


bool Req_Mgt::push_asyn_req(const std::string &msg_id, Request_Ptr req)
{
	Thread_Mutex_Guard guard(_mutex);

	//XCP_LOGGER_INFO(&g_logger_debug, "push req, msg_id:%s\n", msg_id.c_str());
	
	std::map<std::string, Request_Ptr>::iterator itr = _reqs.find(msg_id);
	if(itr != _reqs.end())
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "push req is exist, msg_id:%s\n", msg_id.c_str());
		DEBUGGER_INFO(&g_debugger, "", msg_id, -1, "push req is exist, msg_id:%s", msg_id.c_str());
		return false;
	}

	_reqs.insert(std::make_pair(msg_id, req));

	return true;
	
}



bool Req_Mgt::get_asyn_req(const std::string &msg_id, Request_Ptr &req)
{
	Thread_Mutex_Guard guard(_mutex);

	//XCP_LOGGER_INFO(&g_logger_debug, "get req, msg_id:%s\n", msg_id.c_str());
	
	std::map<std::string, Request_Ptr>::iterator itr = _reqs.find(msg_id);
	if(itr != _reqs.end())
	{
		req = itr->second;
		_reqs.erase(itr);
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "get req isn't exist, msg_id:%s\n", msg_id.c_str());
		DEBUGGER_INFO(&g_debugger, "", msg_id, -1, "get req isn't exist, msg_id:%s", msg_id.c_str());
		return false;
	}

	return true;
}




void Req_Mgt::del_asyn_req(const std::string &msg_id)
{
	Thread_Mutex_Guard guard(_mutex);

	//XCP_LOGGER_INFO(&g_logger_debug, "del req, msg_id:%s\n", msg_id.c_str());
	
	std::map<std::string, Request_Ptr>::iterator itr = _reqs.find(msg_id);
	if(itr != _reqs.end())
	{
		_reqs.erase(itr);
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "del req isn't exist, msg_id:%s\n", msg_id.c_str());
		DEBUGGER_INFO(&g_debugger, "", msg_id, -1, "del req isn't exist, msg_id:%s", msg_id.c_str());
	}

}



void Req_Mgt::check_asyn_req()
{
	Thread_Mutex_Guard guard(_mutex);

	//XCP_LOGGER_INFO(&g_logger_debug, "check_asyn_req\n");

	//计算当前时间戳
	unsigned long long cur_stmp = time(NULL);
	std::map<std::string, Request_Ptr>::iterator itr = _reqs.begin();
	for(; itr != _reqs.end();)
	{
		/*
		超过30秒还没有收到worker 返回的请求，该请求处理超时
		需要从异步队列中将其删除
		*/
		if(cur_stmp - (itr->second->_stmp) > 30)
		{
			std::string rsp = "";
			XCP_LOGGER_ERROR(&g_logger_debug, "===timeout: the request is timeout, cur_stmp:%llu, req:%s\n", 
				cur_stmp, itr->second->to_string().c_str());

			DEBUGGER_INFO(&g_debugger, itr->second->_req_id, itr->second->_msg_id, itr->second->_fd, 
				"===timeout: the request is timeout, cur_stmp:%llu, req:%s", 
				cur_stmp, itr->second->to_string().c_str());

			_reqs.erase(itr++);
		}
		else
		{
			++itr;
		}
	}

}




