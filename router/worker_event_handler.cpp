
#include "worker_event_handler.h"
#include "base/base_socket_oper.h"
#include "base/base_net.h"
#include "req_mgt.h"
#include "session_mgt.h"
#include "worker_mgt.h"
#include "libcomm/cmd.h"
#include "libcomm/common.h"
#include "libcomm/error_code.h"
#include "libcomm/protocol.h"
#include "base/base_logger.h"
#include "base/base_time.h"
#include "base/base_string.h"
#include "worker_mgt.h"
#include "msg_oper.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;

Worker_Event_Handler::Worker_Event_Handler(): _buf("")
{

};


Worker_Event_Handler::~Worker_Event_Handler()
{

};

	
//处理建立连接请求事件
int Worker_Event_Handler::handle_accept(int fd)
{
	int nRet = 0;

	std::string local_ip = "";
	unsigned short local_port = 0;
	get_local_socket(fd, local_ip, local_port);
	
	std::string remote_ip = "";
	unsigned short remote_port = 0; 
	get_remote_socket(fd, remote_ip, remote_port);
	
	XCP_LOGGER_INFO(&g_logger_debug, "accept success(fd:%d) from worker, %s:%u --> %s:%u\n", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
	DEBUGGER_INFO(&g_debugger, "", "", fd, "accept success(fd:%d) from worker, %s:%u --> %s:%u", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
	
	return nRet;
};



//处理读事件
int Worker_Event_Handler::handle_input(int fd)
{
	int nRet = 0;
		
	char buf[4096];
	unsigned int buf_len = 4095;
	nRet = Socket_Oper::recv(fd, buf, buf_len, 300000);
	if(nRet == 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "rcv close from worker(fd:%d), ret:%d\n", fd, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv close from worker(fd:%d), ret:%d", fd, nRet);
		return -1;
	}
	else if(nRet == 1)
	{
		//XCP_LOGGER_INFO(&g_logger_debug, "rcv success from worker(fd:%d), req(%u):%s\n", fd, buf_len, buf);
	}
	else
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "rcv failed from worker(fd:%d), ret:%d\n", fd, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", fd, "rcv failed from worker(fd:%d), ret:%d", fd, nRet);
		return -2;
	}
	buf[buf_len] = '\0';

	
	//追加缓存
	_buf += buf;
	std::string::size_type pos = _buf.find("\n");
	while(pos != std::string::npos)
	{
		//解析完整请求串
		std::string req_src = _buf.substr(0, pos);
		_buf.erase(0, pos+1);

		trim(req_src);

		//XCP_LOGGER_INFO(&g_logger_debug, "req(%u):%s\n", req_src.size(), req_src.c_str());
		//DEBUGGER_INFO(&g_debugger, "", "", fd, "req(%u):%s", req_src.size(), req_src.c_str());

		//获取cmd
		std::string cmd = "";
		unsigned long long time = 0;
		std::string msg_id = "";
		std::string req_id = "";
		int err = 0;
		std::string err_info = "";
		nRet = XProtocol::get_admin_head(req_src, cmd, time, msg_id, req_id, err, err_info);
		//XCP_LOGGER_INFO(&g_logger_debug, "cmd(%u):%s\n", cmd.size(), cmd.c_str());
		if(nRet != 0)
		{
			XCP_LOGGER_INFO(&g_logger_debug, "invlid msg from worker, msg(%u)%s\n", req_src.size(), req_src.c_str());
			DEBUGGER_INFO(&g_debugger, "", "", fd, "invlid msg from worker, msg(%u)%s", req_src.size(), req_src.c_str());
		}
		else if(cmd == CMD_REGISTER)
		{
			XCP_LOGGER_INFO(&g_logger_debug, "rcv register req from worker, req(%u)%s\n", req_src.size(), req_src.c_str());
			DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "rcv register req from worker, req(%u)%s", req_src.size(), req_src.c_str());

			//注册worker
			Worker_Ptr worker = new Worker;
			worker->_fd = fd;
			worker->_create_stmp = getTimestamp();
			nRet = get_remote_socket(fd, worker->_ip, worker->_port);
			if(nRet != 0)
			{
				XCP_LOGGER_ERROR(&g_logger_debug, "get_remote_socket(fd:%d) failed, ret:%d\n", fd, nRet);
				DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "get_remote_socket(fd:%d) failed, ret:%d", fd, nRet);
			}
			else
			{
				std::string worker_id = "";
				std::string group_id = "";
				std::string service_id = "";
				std::string server_id = "";
				std::string cmd_list = "";
				nRet = XProtocol::get_worker_info(req_src, worker_id, group_id, service_id, server_id, cmd_list);
				XCP_LOGGER_ERROR(&g_logger_debug, "register info: worker_id:%s, group_id:%s\n", worker_id.c_str(), group_id.c_str());
				DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "register info: worker_id:%s, group_id:%s", worker_id.c_str(), group_id.c_str());
				if(nRet != 0)
				{
					XCP_LOGGER_ERROR(&g_logger_debug, "get worker info failed, ret:%d, req(%u):%s\n", 
						nRet, req_src.size(), req_src.c_str());
					DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "get worker info failed, ret:%d, req(%u):%s", 
						nRet, req_src.size(), req_src.c_str());
					nRet = ERR_INVALID_REQ;
				}
				else if((worker_id == "") || (group_id == ""))
				{
					//非法请求串
					XCP_LOGGER_ERROR(&g_logger_debug, "the register request(worker id:%s, group id:%s) is invalid form worker(%s).\n",
						worker_id.c_str(), group_id.c_str(), worker->to_string().c_str());
					DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "the register request(worker id:%s, group id:%s) is invalid form worker(%s)",
						worker_id.c_str(), group_id.c_str(), worker->to_string().c_str());					
					nRet = ERR_INVALID_REQ;
				}
				else
				{
					worker->_id = worker_id;
					worker->_group_id = group_id;
					nRet = PSGT_Worker_Mgt->register_worker(worker);
					if(nRet == 0)
					{
						XCP_LOGGER_ERROR(&g_logger_debug, "register worker success.\n");
						DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "register worker success.");						
					}
					else
					{
						XCP_LOGGER_ERROR(&g_logger_debug, "register worker failed, ret:%d\n", nRet);
						DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "register worker failed, ret:%d", nRet);	
					}
				}
				worker->send_rsp(cmd, req_id, msg_id, nRet);
				
			}
			
		}
		else if(cmd == CMD_HB)
		{
			//心跳
			//XCP_LOGGER_INFO(&g_logger_debug, "rcv hb from worker.\n", req_src.size(), req_src.c_str());
		}
		else if(cmd == CMD_UNREGISTER)
		{
			//去注册
			XCP_LOGGER_INFO(&g_logger_debug, "rcv unregister req from worker, fd:%d, req(%u)%s\n", fd, req_src.size(), req_src.c_str());
			DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "rcv unregister from worker, fd:%d, req(%u)%s", fd, req_src.size(), req_src.c_str());
			
			Worker_Ptr worker;
			if(!PSGT_Worker_Mgt->get_worker(fd, worker))
			{
				XCP_LOGGER_ERROR(&g_logger_debug, "unregister-get worker failed, fd:%d\n", fd);
				DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "unregister-get worker failed, fd:%d", fd);
			}
			else
			{
				PSGT_Worker_Mgt->unregister_worker(worker);
			}

		}			
		else
		{
			//发送返回信息给前端业务
			Request_Ptr req;	
			if(PSGT_Req_Mgt->get_asyn_req(msg_id, req))
			{
				XCP_LOGGER_INFO(&g_logger_debug, "===rsp from worker(req:%s), size:%u, msg:%s\n", 
					req->to_string().c_str(), req_src.size(), req_src.c_str());
				DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "===rsp from worker(req:%s), size:%u, msg:%s", 
					req->to_string().c_str(), req_src.size(), req_src.c_str());
							
				//通过session id 判断前端是否已经关闭
				if(PSGT_Session_Mgt->is_valid_sesssion(req->_session_id))
				{
					Msg_Oper::send_msg(req->_fd, req_src+std::string("\n"));	
				}
				else
				{
					XCP_LOGGER_ERROR(&g_logger_debug, "the session is closed ago, can't rsponse to app, req:%s\n", 
						req->to_string().c_str());
					DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "the session is closed ago, can't rsponse to app, req:%s", 
						req->to_string().c_str());					
				}
				
			}
			else
			{
				XCP_LOGGER_ERROR(&g_logger_debug, "get_asyn_req failed, req_src(%u):%s\n", 
					req_src.size(), req_src.c_str());
				DEBUGGER_INFO(&g_debugger, req_id, msg_id, fd, "get_asyn_req failed, req_src(%u):%s", 
					req_src.size(), req_src.c_str());				
			}
			
		}
		
		pos = _buf.find("\n");
			
	}

	return 0;

};




//处理连接关闭事件
int Worker_Event_Handler::handle_close(int fd)
{
	int nRet = 0;

	Worker *worker = new Worker;
	worker->_fd = fd;
	PSGT_Worker_Mgt->unregister_worker(worker);

	std::string local_ip = "";
	unsigned short local_port = 0;
	get_local_socket(fd, local_ip, local_port);
	
	std::string remote_ip = "";
	unsigned short remote_port = 0; 
	get_remote_socket(fd, remote_ip, remote_port);
	
	XCP_LOGGER_INFO(&g_logger_debug, "close (fd:%d) from worker, %s:%u --> %s:%u\n", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
	DEBUGGER_INFO(&g_debugger, "", "", fd, "close (fd:%d) from worker, %s:%u --> %s:%u", 
		fd, remote_ip.c_str(), remote_port, local_ip.c_str(), local_port);
				
	return nRet;
	
};



Event_Handler* Worker_Event_Handler::renew()
{
	return new Worker_Event_Handler;
};


