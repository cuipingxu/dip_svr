
#include "libcomm/common.h"
#include "libcomm/error_code.h"
#include "base/base_socket_oper.h"
#include "base/base_time.h"
#include "base/base_net.h"
#include "base/base_string.h"
#include "base/base_uid.h"
#include "base/base_logger.h"
#include "req_tcp_event_handler.h"
#include "req_mgt.h"
#include "session_mgt.h"
#include "msg_oper.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern std::string g_id;
extern Debugger g_debugger;

Req_TCP_Event_Handler::Req_TCP_Event_Handler(): _buf("")
{

};

Req_TCP_Event_Handler::~Req_TCP_Event_Handler()
{

};

	
//处理建立连接请求事件
int Req_TCP_Event_Handler::handle_accept(int fd)
{
	int nRet = 0;
	
	std::string local_ip = "";
	unsigned short local_port = 0;
	get_local_socket(fd, local_ip, local_port);
	
	std::string remote_ip = "";
	unsigned short remote_port = 0; 
	get_remote_socket(fd, remote_ip, remote_port);
	
	XCP_LOGGER_INFO(&g_logger_debug, "accept from app(fd:%d), %s:%u --> %s:%u\n", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);

	DEBUGGER_INFO(&g_debugger, "", "", fd, "accept from app(fd:%d), %s:%u --> %s:%u", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
	
	PSGT_Session_Mgt->insert_session(fd);
	
	return nRet;
};



/*
处理读事件
telnet 消息结尾\r\n
客户端消息结尾\n
*/
int Req_TCP_Event_Handler::handle_input(int fd)
{
	int nRet = 0;

	//XCP_LOGGER_INFO(&g_logger_debug, "prepare to rcv from app(fd:%d)\n", fd);
	//DEBUGGER_INFO(&g_debugger, "", "", fd, "prepare to rcv from app(fd:%d)", fd);
	
	//读取fd
	char buf[4096];
	unsigned int buf_len = 4095;
	nRet = Socket_Oper::recv(fd, buf, buf_len, 300000);
	if(nRet == 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "rcv close form app(fd:%d), ret:%d\n", fd, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv close form app(fd:%d), ret:%d", fd, nRet);
		return -1;
	}
	else if(nRet == 1)
	{
		//XCP_LOGGER_INFO(&g_logger_debug, "rcv success from app(fd:%d), req(%u):%s\n", fd, buf_len, buf);
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "rcv failed from app(fd:%d), ret:%d\n", fd, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv failed from app(fd:%d), ret:%d", fd, nRet);
		return 0;
	}
	buf[buf_len] = '\0';

	std::string session_id = "";
	PSGT_Session_Mgt->update_read(fd, buf_len, session_id);

	//获取前端ip 和port
	std::string ip = "";
	unsigned short port = 0;
	get_remote_socket(fd, ip, port);
	
	
	//追加缓存
	_buf += buf;
	std::string::size_type pos = _buf.find("\n");
	while(pos != std::string::npos)
	{
		//解析完整请求串
		std::string req_src = _buf.substr(0, pos);
		_buf.erase(0, pos+1);

		trim(req_src);

		//追加内部请求msg_id
		std::string msg_id = UID::uid_inc(g_id);
					
		if(req_src.size() < MIN_MSG_LEN)
		{
			//XCP_LOGGER_ERROR(&g_logger_debug, "the req reach min len, req(%u):%s\n", req_src.size(), req_src.c_str());
		}		
		else if(req_src.size() > MAX_MSG_LEN)
		{
			XCP_LOGGER_ERROR(&g_logger_debug, "the req reach max len(%u)\n", MAX_MSG_LEN);
			DEBUGGER_INFO(&g_debugger, "", "", fd, "the req reach max len(%u)", MAX_MSG_LEN);
			Msg_Oper::send_msg(fd, "null", "null", msg_id, ERR_REACH_MAX_MSG, "the req reach max len.");
		}
		else
		{
			//处理正常请求
			Request *req = new Request;
			req->_stmp = getTimestamp();
			req->_req = req_src;
			req->_msg_id = msg_id;
			req->_ip = ip;
			req->_port = port;
			req->_session_id = session_id;
			req->_fd = fd;

			XCP_LOGGER_INFO(&g_logger_debug, "===req from app:%s\n", req->to_string().c_str());
			DEBUGGER_INFO(&g_debugger, "", msg_id, fd, "===req from app:%s", req->to_string().c_str());

			//首先先判断req queue 是否已经满了
			if(!(PSGT_Req_Mgt->full()))
			{	
				nRet = PSGT_Req_Mgt->push_req(req);
				if(nRet != 0)
				{
					XCP_LOGGER_ERROR(&g_logger_debug, "push req into req mgt failed, ret:%d, req(%u):%s\n", 
						nRet, buf_len, buf);
					DEBUGGER_INFO(&g_debugger, "", msg_id, fd, "push req into req mgt failed, ret:%d, req(%u):%s", 
						nRet, buf_len, buf);
					Msg_Oper::send_msg(fd, "null", "null", msg_id, ERR_PUSH_QUEUE_FAIL, "push req into req mgt failed.");
				}
				
			}
			else
			{		
				XCP_LOGGER_ERROR(&g_logger_debug, "req mgt is full, req(%u):%s\n", buf_len, buf);
				DEBUGGER_INFO(&g_debugger, "", msg_id, fd, "req mgt is full, req(%u):%s", buf_len, buf);				
				Msg_Oper::send_msg(fd, "null", "null", msg_id, ERR_QUEUE_FULL, "req mgt is full.");
			}

		}
		
		pos = _buf.find("\n");
			
	}

	return 0;

};



//处理连接关闭事件
int Req_TCP_Event_Handler::handle_close(int fd)
{
	int nRet = 0;
	
	std::string local_ip = "";
	unsigned short local_port = 0;
	get_local_socket(fd, local_ip, local_port);
	
	std::string remote_ip = "";
	unsigned short remote_port = 0; 
	get_remote_socket(fd, remote_ip, remote_port);

	XCP_LOGGER_INFO(&g_logger_debug, "close from app(fd:%d), %s:%u --> %s:%u\n", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);

	DEBUGGER_INFO(&g_debugger, "", "", fd, "close from app(fd:%d), %s:%u --> %s:%u", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
	
	PSGT_Session_Mgt->remove_session(fd);
	
	return nRet;
	
};



Event_Handler* Req_TCP_Event_Handler::renew()
{
	return new Req_TCP_Event_Handler;
};


