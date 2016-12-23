
#include "msg_oper.h"
#include "base/base_socket_oper.h"
#include "base/base_convert.h"
#include "base/base_logger.h"
#include "session_mgt.h"
#include "libcomm/protocol.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;

Thread_Mutex Msg_Oper::_mutex_msg;


int Msg_Oper::send_msg(int fd, const std::string &cmd, const std::string &req_id, const std::string &msg_id,  
	const int err, const std::string &error_info, const std::string &body, bool is_object)
{
	int nRet = 0;

	Thread_Mutex_Guard guard(_mutex_msg);

	std::string msg = XProtocol::set_rsp_msg(cmd, req_id, msg_id, err, error_info, body, is_object);
	
	unsigned int len = msg.size();
	nRet = Socket_Oper::send_n(fd, msg.c_str(), len, 0, 300000);
	if(nRet == 0)
	{
		PSGT_Session_Mgt->update_write(fd, len);
		//XCP_LOGGER_INFO(&g_logger_debug, "send rsp msg success. fd:%d, msg(%u):%s\n", fd, msg.size(), msg.c_str());
	}
	else if(nRet == 2)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "send rsp msg timeout, ret:%d, fd:%d, msg(%u):%s\n", 
			nRet, fd, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "send rsp msg timeout, ret:%d, fd:%d, msg(%u):%s", 
			nRet, fd, msg.size(), msg.c_str());
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "send rsp msg to failed, ret:%d, fd:%d, msg(%u):%s\n", 
			nRet, fd, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "send rsp msg to failed, ret:%d, fd:%d, msg(%u):%s", 
			nRet, fd, msg.size(), msg.c_str());		
	}
	
	return nRet;

}



int Msg_Oper::send_msg(int fd, const std::string &msg)
{
	int nRet = 0;

	Thread_Mutex_Guard guard(_mutex_msg);
	
	unsigned int len = msg.size();
	nRet = Socket_Oper::send_n(fd, msg.c_str(), len, 0, 300000);
	if(nRet == 0)
	{
		PSGT_Session_Mgt->update_write(fd, len);
		//XCP_LOGGER_INFO(&g_logger_debug, "send msg success. fd:%d, msg(%u):%s\n", fd, msg.size(), msg.c_str());
	}
	else if(nRet == 2)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "send msg timeout, ret:%d, fd:%d, msg(%u):%s\n", 
			nRet, fd, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, "", "", fd, "send msg timeout, ret:%d, fd:%d, msg(%u):%s", 
			nRet, fd, msg.size(), msg.c_str());
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "send msg to failed, ret:%d, fd:%d, msg(%u):%s\n", 
			nRet, fd, msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, "", "", fd, "send msg to failed, ret:%d, fd:%d, msg(%u):%s", 
			nRet, fd, msg.size(), msg.c_str());		
	}
	
	return nRet;

}


