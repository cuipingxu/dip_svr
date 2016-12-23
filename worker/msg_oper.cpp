
#include "msg_oper.h"
#include "conn_mgt.h"
#include "base/base_socket_oper.h"
#include "base/base_convert.h"
#include "base/base_logger.h"
#include "libcomm/protocol.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;

int Msg_Oper::send_msg(std::string router_id, const std::string &cmd, const std::string &req_id, const std::string &msg_id,  
	const int err, const std::string &error_info, const std::string &body, bool is_object)
{
	int nRet = 0;

	XConn_Ptr conn;
	if(!PSGT_XConn_Mgt->get_conn(router_id, conn))
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "no conn is found, router id:%s\n", router_id.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, -1, "no conn is found, router id:%s", router_id.c_str());
		return -1;
	}

	std::string msg = XProtocol::set_rsp_msg(cmd, req_id, msg_id, err, error_info, body, is_object);
	unsigned int len = msg.size();
	conn->_client->send_msg(msg.c_str(), len, 0, 300000);
	if(nRet == 0)
	{
		//XCP_LOGGER_INFO(&g_logger_debug, "send success. router id:%s, msg(%u):%s\n", router_id.c_str(), msg.size(), msg.c_str());
	}
	else if(nRet == 2)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "send msg timeout, ret:%d, router id:%s, msg(%u):%s\n", 
			nRet, router_id.c_str(), msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, -1, "send msg timeout, ret:%d, router id:%s, msg(%u):%s", 
			nRet, router_id.c_str(), msg.size(), msg.c_str());
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "send msg to failed, ret:%d, router id:%s, msg(%u):%s\n", 
			nRet, router_id.c_str(), msg.size(), msg.c_str());
		DEBUGGER_INFO(&g_debugger, req_id, msg_id, -1, "send msg to failed, ret:%d, router id:%s, msg(%u):%s", 
			nRet, router_id.c_str(), msg.size(), msg.c_str());		
	}
	
	return nRet;

}


