
#include "base/base_socket_oper.h"
#include "base/base_net.h"
#include "base/base_logger.h"
#include "base/base_time.h"
#include "base/base_string.h"
#include "libcomm/common.h"
#include "libcomm/cmd.h"
#include "libcomm/error_code.h"
#include "libcomm/protocol.h"
#include "conn_mgt.h"
#include "req_mgt.h"
#include "req_event_handler.h"
#include "msg_oper.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;
extern std::string g_id;

Req_Event_Handler::Req_Event_Handler(): _buf("")
{

};


Req_Event_Handler::~Req_Event_Handler()
{

};


//处理读事件
int Req_Event_Handler::handle_input(int fd)
{
	int nRet = 0;
		
	char buf[4096];
	unsigned int buf_len = 4095;
	nRet = Socket_Oper::recv(fd, buf, buf_len, 300000);
	if(nRet == 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "rcv close from router(fd:%d), ret:%d\n", fd, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv close from router(fd:%d), ret:%d", fd, nRet);
		return -1;
	}
	else if(nRet == 1)
	{
		//XCP_LOGGER_INFO(&g_logger_debug, "rcv success from router(fd:%d), req(%u):%s\n", fd, buf_len, buf);
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "rcv failed from router(fd:%d), ret:%d\n", fd, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv failed from router(fd:%d), ret:%d", fd, nRet);
		return -2;
	}
	buf[buf_len] = '\0';

	//获取router server ip 和port 信息
	std::string ip = "";
	unsigned short port = 0;
	get_remote_socket(fd, ip, port);

	//获取指定conn
	std::string router_id = format("%s_%u", ip.c_str(), port);

	//追加缓存
	_buf += buf;
	std::string::size_type pos = _buf.find("\n");
	while(pos != std::string::npos)
	{
		//解析完整请求串
		std::string req_src = _buf.substr(0, pos);
		_buf.erase(0, pos+1);
		XCP_LOGGER_INFO(&g_logger_debug, "rcv request form router, req(%u):%s\n", req_src.size(), req_src.c_str());
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv request form router, req(%u):%s", req_src.size(), req_src.c_str());
		
		trim(req_src);
		
		if(req_src.size() < MIN_MSG_LEN)
		{
			//XCP_LOGGER_ERROR(&g_logger_debug, "the req reach min len, req(%u):%s\n", req_src.size(), req_src.c_str());
		}
		else if(req_src.size() > MAX_MSG_LEN)
		{
			//单条日志最大长度4K， 这里会有截取
			XCP_LOGGER_ERROR(&g_logger_debug, "the req reach max len, req(%u)\n", req_src.size());
			DEBUGGER_INFO(&g_debugger, "", "", fd, "the req reach max len, req(%u)", req_src.size());
		}
		else
		{
			//获取admin head
			std::string cmd = "";
			unsigned long long time = 0;
			std::string msg_id = "";
			std::string req_id = "";
			int err = 0;
			std::string err_info = "";
			nRet = XProtocol::get_admin_head(req_src, cmd, time, msg_id, req_id, err, err_info);
			if(nRet != 0)
			{
				XCP_LOGGER_INFO(&g_logger_debug, "get_admin_head failed, ret:%d, req(%u):%s\n", 
					nRet, req_src.size(), req_src.c_str());
				DEBUGGER_INFO(&g_debugger, "", "", fd, "get_admin_head failed, ret:%d, req(%u):%s", 
					nRet, req_src.size(), req_src.c_str());

				Msg_Oper::send_msg(router_id, "null", "null", "null", ERR_INVALID_REQ, "invalid req	from router.");
			}
			else
			{
				if(cmd == CMD_REGISTER)
				{
					XConn_Ptr conn;
					if(!PSGT_XConn_Mgt->get_conn(router_id, conn))
					{
						XCP_LOGGER_ERROR(&g_logger_debug, "register:no conn is found, fd:%d, router id:%s\n", fd, router_id.c_str());
						DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "register:no conn is found, fd:%d, router id:%s", fd, router_id.c_str());
					}
					else
					{
						if(err == 0)
						{
							conn->_registered = true;
							XCP_LOGGER_INFO(&g_logger_debug, "register to router(%s) success\n", router_id.c_str());
							DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "register to router(%s) success", router_id.c_str());							
						}
						else
						{
							XCP_LOGGER_INFO(&g_logger_debug, "register to router(%s) failed, ret:%d\n", router_id.c_str(), err);
							DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "register to router(%s) failed, ret:%d", router_id.c_str(), err);
						}
						
					}
					
				}
				else if(cmd == CMD_HB)
				{
					//心跳响应
					//XCP_LOGGER_INFO(&g_logger_debug, "rcv the rsp hb from router.\n", req_src.size(), req_src.c_str());
				
					XConn_Ptr conn;
					if(!PSGT_XConn_Mgt->get_conn(router_id, conn))
					{
						XCP_LOGGER_ERROR(&g_logger_debug, "hb:no conn is found, fd:%d, router id:%s\n", fd, router_id.c_str());
						DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "hb:no conn is found, fd:%d, router id:%s", fd, router_id.c_str());
					}
					else
					{
						//刷新最后一次心跳的时间戳
						conn->_stmp_hb = getTimestamp();
					}		
				}	
				else
				{
					//其他的请求
					Request *req = new Request;
					req->_cmd = cmd;
					req->_stmp = getTimestamp();
					req->_req = req_src;
					req->_ip = ip;
					req->_port = port;
					req->_msg_id = msg_id;
					req->_req_id = req_id;
					req->_fd = fd;
					XCP_LOGGER_INFO(&g_logger_debug, "===req from router:%s\n", req->to_string().c_str());
					DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "===req from router:%s", req->to_string().c_str());
					
					//首先先判断req queue 是否已经满了
					if(!(PSGT_Req_Mgt->full()))
					{	
						nRet = PSGT_Req_Mgt->push_req(req);
						if(nRet != 0)
						{
							XCP_LOGGER_ERROR(&g_logger_debug, "push req into req mgt failed, ret:%d, req(%u):%s\n", nRet, buf_len, buf);
							DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "push req into req mgt failed, ret:%d, req(%u):%s", nRet, buf_len, buf);
							Msg_Oper::send_msg(router_id, cmd, req_id, msg_id, ERR_PUSH_QUEUE_FAIL, "push req into req mgt failed.");
						}
					}
					else
					{
						XCP_LOGGER_ERROR(&g_logger_debug, "req mgt is full, req(%u):%s\n", buf_len, buf);
						DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "req mgt is full, req(%u):%s", buf_len, buf);
						Msg_Oper::send_msg(router_id, cmd, req_id, msg_id, ERR_QUEUE_FULL, "req mgt is full.");
					}
				
				}

			}
			
		}
		
		pos = _buf.find("\n");
		
	}

	return 0;

};




//处理连接关闭事件
int Req_Event_Handler::handle_close(int fd)
{
	int nRet = 0;

	std::string local_ip = "";
	unsigned short local_port = 0;
	get_local_socket(fd, local_ip, local_port);
	
	std::string remote_ip = "";
	unsigned short remote_port = 0; 
	get_remote_socket(fd, remote_ip, remote_port);
	
	XCP_LOGGER_INFO(&g_logger_debug, "close (fd:%d) from router, %s:%u --> %s:%u\n", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
	DEBUGGER_INFO(&g_debugger, "", "", fd, "close (fd:%d) from router, %s:%u --> %s:%u", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);

	//刷新注册标志位
	XConn_Ptr conn;
	std::string router_id = format("%s_%u", remote_ip.c_str(), remote_port);
	if(PSGT_XConn_Mgt->get_conn(router_id, conn))
	{
		conn->_registered = false;
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "handle close: no conn is found, router id:%s\n", router_id.c_str());
		DEBUGGER_INFO(&g_debugger, "", "", fd, "handle close: no conn is found, router id:%s", router_id.c_str());
	}
			
	return nRet;
	
};



Event_Handler* Req_Event_Handler::renew()
{
	return new Req_Event_Handler;
};

