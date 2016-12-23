

#include "request.h"
#include "base/base_string.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"

extern Logger g_logger_debug;

Request::Request(): _cmd(""), _msg_id(""), _req_id(""), _req(""), 
	_fd(-1), _ip(""), _port(0), _stmp(0)
{

}


Request::~Request()
{

}


void Request::log()
{
	XCP_LOGGER_INFO(&g_logger_debug, "req: cmd=%s, msg_id=%s, req_id=%s, fd=%d, stmp=%llu, app=(%s:%u), req(%u)=%s\n", 
		_cmd.c_str(), _msg_id.c_str(), _req_id.c_str(), _fd, _stmp, _ip.c_str(), _port, 
		_req.size(), _req.c_str());
}



std::string Request::to_string()
{
	return format("cmd=%s, msg_id=%s, req_id=%s, fd=%d, stmp=%llu, app=(%s:%u), req(%u)=%s", 
		_cmd.c_str(), _msg_id.c_str(), _req_id.c_str(), _fd, _stmp, _ip.c_str(), _port, 
		 _req.size(), _req.c_str());
}

