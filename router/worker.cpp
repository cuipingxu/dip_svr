
#include "worker.h"
#include "base/base_logger.h"
#include "base/base_string.h"
#include "base/base_socket_oper.h"
#include "base/base_convert.h"
#include "base/base_uid.h"
#include "worker_mgt.h"
#include "libcomm/protocol.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;
extern unsigned int g_unregister_failed_num;  //剔除worker 的请求失败数
extern unsigned int g_unregister_timeout_num; //剔除worker 的请求超时数

Worker::Worker(): _index(0), _id(""), _fd(-1), _ip(""), _port(0), 
	_group_id(""), _create_stmp(0), _hb_stmp(0), _unregister_failed_num(0), _unregister_timeout_num(0)
{

}

Worker::~Worker()
{

}


bool Worker::operator ==(const Worker &worker)
{
	if(_id == worker._id)
	{
		return true;
	}

	return false;
}



int Worker::send_req(const std::string &req, const std::string &req_id, const std::string &msg_id)
{
	Thread_Mutex_Guard guard(_mutex);

	int nRet = 0;

	std::string msg = XProtocol::set_new_req(req, msg_id);
	if(msg.empty())
	{
		XCP_LOGGER_INFO(&g_logger_debug, "msg is empty, req(%u):%s\n", req.size(), req.c_str());
		return -1;
	}
	msg += std::string("\n");
	
	unsigned int len = msg.size();
	nRet = Socket_Oper::send_n(_fd, msg.c_str(), len, 0, 300000);
	if(nRet == 0)
	{
		_unregister_timeout_num = 0;
		_unregister_failed_num = 0;
		XCP_LOGGER_INFO(&g_logger_debug, "send req to worker(%s) success, req(%u):%s\n", 
			to_string().c_str(), msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "send req to worker(%s) success, req(%u):%s", 
			to_string().c_str(), req.size(), req.c_str());
	}
	else if(nRet == 2)
	{
		++_unregister_timeout_num;
		XCP_LOGGER_ERROR(&g_logger_debug, "send req to worker(%s) timeout, ret:%d, req(%u):%s\n", 
			to_string().c_str(), nRet, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "send admin to worker(%s) timeout, ret:%d, req(%u):%s", 
			to_string().c_str(), nRet, req.size(), req.c_str());
	}
	else
	{
		++_unregister_failed_num;
		XCP_LOGGER_ERROR(&g_logger_debug, "send req to worker(%s) failed, ret:%d, req(%u):%s\n", 
			to_string().c_str(), _fd, nRet, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "send admin to worker(%s) failed, ret:%d, req(%u):%s", 
			to_string().c_str(), nRet, req.size(), req.c_str());
	}

	//如果对一个worker 发送失败次数超过阈值就去注册worker
	if((_unregister_failed_num >= g_unregister_failed_num) ||
	   (_unregister_timeout_num >= g_unregister_timeout_num))
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "req unregister worker(%s), failed:%u, max_failed:%u, timeout:%u, max_timeout:%u\n", 
			to_string().c_str(), _unregister_failed_num, g_unregister_failed_num, 
			_unregister_timeout_num, g_unregister_timeout_num);
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "req unregister worker(%s), failed:%u, max_failed:%u, timeout:%u, max_timeout:%u", 
			to_string().c_str(), _unregister_failed_num, g_unregister_failed_num, 
			_unregister_timeout_num, g_unregister_timeout_num);

		PSGT_Worker_Mgt->unregister_worker(this);
		::close(_fd);
		_unregister_timeout_num = 0;
		_unregister_failed_num = 0;
	}
	
	return nRet;

}




int Worker::send_rsp(const std::string &cmd, const std::string &req_id, const std::string &msg_id, int err, const std::string &error_info)
{
	Thread_Mutex_Guard guard(_mutex);

	int nRet = 0;

	std::string msg = XProtocol::set_rsp_msg(cmd, req_id, msg_id, err, error_info);
	unsigned int len = msg.size();
	nRet = Socket_Oper::send_n(_fd, msg.c_str(), len, 0, 300000);
	if(nRet == 0)
	{
		_unregister_timeout_num = 0;
		_unregister_failed_num = 0;
		XCP_LOGGER_INFO(&g_logger_debug, "send rsp to worker(%s) success, rsp(%u):%s\n", 
			to_string().c_str(), msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "send rsp to worker(%s) success, cmd:%s, err:%d, error_info:%s", 
			to_string().c_str(), cmd.c_str(), err, error_info.c_str());

	}
	else if(nRet == 2)
	{
		++_unregister_timeout_num;
		XCP_LOGGER_ERROR(&g_logger_debug, "send rsp to worker(%s) timeout, ret:%d, rsp(%u):%s\n", 
			to_string().c_str(), nRet, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "send admin to worker(%s) timeout, ret:%d, cmd:%s, err:%d, error_info:%s", 
			to_string().c_str(), nRet, cmd.c_str(), err, error_info.c_str());

	}
	else
	{
		++_unregister_failed_num;
		XCP_LOGGER_ERROR(&g_logger_debug, "send rsp to worker(%s) failed, ret:%d, rsp(%u):%s\n", 
			to_string().c_str(), _fd, nRet, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "send admin to worker(%s) failed, ret:%d, cmd:%s, err:%d, error_info:%s", 
			to_string().c_str(), nRet, cmd.c_str(), err, error_info.c_str());

	}

	//如果对一个worker 发送失败次数超过阈值就去注册worker
	if((_unregister_failed_num >= g_unregister_failed_num) ||
	   (_unregister_timeout_num >= g_unregister_timeout_num))
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "rsp unregister worker(%s), failed:%u, max_failed:%u, timeout:%u, max_timeout:%u\n", 
			to_string().c_str(), _unregister_failed_num, g_unregister_failed_num, 
			_unregister_timeout_num, g_unregister_timeout_num);
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, _fd, "rsp unregister worker(%s), failed:%u, max_failed:%u, timeout:%u, max_timeout:%u", 
			to_string().c_str(), _unregister_failed_num, g_unregister_failed_num, 
			_unregister_timeout_num, g_unregister_timeout_num);
		
		PSGT_Worker_Mgt->unregister_worker(this);
		::close(_fd);
		_unregister_timeout_num = 0;
		_unregister_failed_num = 0;
	}
	
	return nRet;

}


void Worker::log()
{
	XCP_LOGGER_INFO(&g_logger_debug, "worker: index=%u, id=%s, fd=%d, group id=%s, "
		"ip=%s, port=%u, create_stmp=%llu, hb_stmp=%llu, failed=%u, timeout=%u\n",
		_index, _id.c_str(), _fd, _group_id.c_str(), _ip.c_str(), _port, 
		_create_stmp, _hb_stmp, _unregister_failed_num, _unregister_timeout_num);
}



std::string Worker::to_string()
{
	return format("index=%u, id=%s, fd=%d, group id=%s, "
		"ip=%s, port=%u, create_stmp=%llu, hb_stmp=%llu, failed=%u, timeout=%u",
		_index, _id.c_str(), _fd, _group_id.c_str(), _ip.c_str(), _port, 
		_create_stmp, _hb_stmp, _unregister_failed_num, _unregister_timeout_num);
}


