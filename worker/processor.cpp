
#include "processor.h"
#include "req_mgt.h"
#include "conn_mgt.h"
#include "libcomm/error_code.h"
#include "libcomm/common.h"
#include "base/base_string.h"
#include "base/base_logger.h"
#include "service_adapter.h"
#include "msg_oper.h"
#include "logclient/debugger.h"

extern Logger g_logger_debug;
extern Service_Adapter service_adapter;
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
		XCP_LOGGER_INFO(&g_logger_debug, "prepare to process req from router, %s\n", req->to_string().c_str());
		DEBUGGER_INFO(&g_debugger, req->_req_id, req->_msg_id, req->_fd, "prepare to process req from router, %s", req->to_string().c_str());
		
		std::string rsp = "";
		std::string debug_info = "";
		bool is_obj = true;
		nRet = service_adapter.process(req->_req, rsp, debug_info, is_obj);
		if(nRet != 0)
		{
			XCP_LOGGER_INFO(&g_logger_debug, "service process failed, ret:%d, req(%u):%s\n", nRet, req->_req.size(), req->_req.c_str());
			DEBUGGER_INFO(&g_debugger, req->_req_id, req->_msg_id, req->_fd, "service process failed, ret:%d, req(%u):%s", nRet, req->_req.size(), req->_req.c_str());			
		}
		else
		{
			XCP_LOGGER_INFO(&g_logger_debug, "service process success, req(%u):%s, rsp(%u):%s\n", 
				req->_req.size(), req->_req.c_str(), rsp.size(), rsp.c_str());
			DEBUGGER_INFO(&g_debugger, req->_req_id, req->_msg_id, req->_fd, "service process success, req(%u):%s, rsp(%u):%s", 
				req->_req.size(), req->_req.c_str(), rsp.size(), rsp.c_str());	
		}

		//发送操作结果给Router
		std::string router_id = format("%s_%u", req->_ip.c_str(), req->_port);
		Msg_Oper::send_msg(router_id, req->_cmd, req->_req_id, req->_msg_id, nRet, debug_info, rsp, is_obj);
		
	}
	
	return 0;
	
}


