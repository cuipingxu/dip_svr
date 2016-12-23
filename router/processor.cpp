
#include "processor.h"
#include "req_mgt.h"
#include "worker_mgt.h"
#include "libcomm/error_code.h"
#include "libcomm/common.h"
#include "libcomm/protocol.h"
#include "base/base_net.h"
#include "base/base_string.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"
#include "base/base_url_parser.h"
#include "msg_oper.h"
#include "logclient/debugger.h"
#include "libcomm/conf_mgt.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;

Processor::Processor()
{

}


Processor::~Processor()
{

}



int Processor::do_init(void *args)
{
	int nRet = 0;

	return nRet;
}



int Processor::svc()
{
	int nRet = 0;

	//获取req 并且处理
	Request_Ptr req;
	nRet = PSGT_Req_Mgt->get_req(req);
	if(nRet == 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "prepare to process req from app, %s\n", req->to_string().c_str());
		DEBUGGER_INFO(&g_debugger, "", req->_msg_id, req->_fd, "prepare to process req from app, %s", req->to_string().c_str());

		//白名单判断
		std::set<std::string> white_list = PSGT_Conf_Mgt->get_white_list();
		if(!white_list.empty())
		{
			std::set<std::string>::iterator itr = white_list.find(req->_ip);
			if(itr == white_list.end())
			{
				XCP_LOGGER_INFO(&g_logger_debug, "it is invalid ip(%s).\n", req->_ip.c_str());
				DEBUGGER_INFO(&g_debugger, "", req->_msg_id, req->_fd, "it is invalid ip(%s).", req->_ip.c_str());
				Msg_Oper::send_msg(req->_fd, "null", "null", req->_msg_id, ERR_INVALID_REQ, "it is invalid ip.");
				return 0;
			}
		}
		
		//路由判断， 获取组ID
		std::string cmd = "";
		unsigned long long time = 0;
		std::string req_id = ""; 
		std::string channel = "";
		std::string peer = "";
		std::string service_id = "";
		std::string server_id = "";
		nRet = XProtocol::get_req_head(req->_req, cmd, time, req_id, channel, peer, service_id, server_id);
		if(nRet != 0)
		{
			XCP_LOGGER_INFO(&g_logger_debug, "get req head failed\n");
			DEBUGGER_INFO(&g_debugger, "", req->_msg_id, req->_fd, "get req head failed, ret:%d", ERR_INVALID_REQ);
			Msg_Oper::send_msg(req->_fd, "null", "null", req->_msg_id, ERR_INVALID_REQ, "get req head failed.");
			return 0;
		}

		//更新req 信息
		req->_req_id = req_id;
		req->_channel = channel;
		req->_peer = peer;
		req->_app_stmp = time;
		

		//流控检查
		//to-do
		
	
		//通过组ID 获取一个负载的Worker 
		Worker_Ptr worker;
		if(!PSGT_Worker_Mgt->get_worker(service_id, worker))
		{
			XCP_LOGGER_INFO(&g_logger_debug, "get worker failed, service_id:%s\n", service_id.c_str());
			DEBUGGER_INFO(&g_debugger, req_id, req->_msg_id, req->_fd, "get worker failed, service_id:%s", service_id.c_str());
			Msg_Oper::send_msg(req->_fd, cmd, req_id, req->_msg_id, ERR_NO_WORKER_FOUND, "get worker failed.");
			return 0;
		}

		//插入异步队列
		PSGT_Req_Mgt->push_asyn_req(req->_msg_id, req);
		
		//发送请求给worker
		nRet = worker->send_req(req->_req, req_id, req->_msg_id);
		if(nRet != 0)
		{
			Msg_Oper::send_msg(req->_fd, cmd, req_id, req->_msg_id, ERR_SEND_FAIL, "send req to worker failed.");
			PSGT_Req_Mgt->del_asyn_req(req->_msg_id);
			return 0;
		}

	}
	
	return 0;
	
}


