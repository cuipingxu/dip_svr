

#include "request.h"
#include "base/base_string.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"

extern Logger g_logger_debug;

Request::Request(): _msg_id(""), _req_id(""), _req(""), 
	_fd(-1), _channel(""), _peer(""), _ip(""), _port(0), _stmp(0), _app_stmp(0), _worker_id(""), _group_id(""), _session_id("")
{

}


Request::~Request()
{

}


void Request::log()
{
	XCP_LOGGER_INFO(&g_logger_debug, "req: msg_id=%s, req_id=%s, fd=%d, channel:%s, stmp:%llu, app_stmp:%llu, peer:%s, app=(%s:%u), worker id:%s, group id:%s, session id:%s, req(%u):%s\n", 
		_msg_id.c_str(), _req_id.c_str(), _fd, _channel.c_str(), _stmp, _app_stmp, _peer.c_str(), _ip.c_str(), _port, 
		_worker_id.c_str(), _group_id.c_str(), _session_id.c_str(),
		_req.size(), _req.c_str());
}



std::string Request::to_string()
{
	return format("req: msg_id=%s, req_id=%s, fd=%d, channel:%s, stmp:%llu, app_stmp:%llu, peer:%s, app=(%s:%u), worker id:%s, group id:%s, session id:%s, req(%u):%s", 
		_msg_id.c_str(), _req_id.c_str(), _fd, _channel.c_str(), _stmp, _app_stmp, _peer.c_str(), _ip.c_str(), _port, 
		_worker_id.c_str(), _group_id.c_str(), _session_id.c_str(),
		_req.size(), _req.c_str());
}

