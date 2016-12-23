
#include "eqq_mgt.h"
#include "libcomm/protocol.h"
#include "eqq_timer_handler.h"
#include "base/base_curl.h"
#include "base/base_url_parser.h"


const std::string EQQ_GET_USER_LIST = "https://openapi.b.qq.com/api/user/list";
const std::string EQQ_GET_GROUP_LIST = "https://openapi.b.qq.com/api/dept/list";
const std::string EQQ_SEND_BROADCAST = "https://openapi.b.qq.com/api/broadcast/send";

eqq_mgt::eqq_mgt()
{

}


eqq_mgt::~eqq_mgt()
{

}



int eqq_mgt::init(service_args* args)
{
	int nRet = 0;
	
	_args = args;

	nRet = update_openid_list();
	if(nRet != 0)
	{
		printf("update_openid_list failed, ret:%d\n", nRet);
		return nRet;
	}

	nRet = PSGT_EQQ_Mgt->update_eqq_group_list();
	if(nRet != 0)
	{
		printf("update_group_list failed, ret:%d\n", nRet);
		return nRet;
	}
	

	//启动conn mgt  timer，十分钟刷新一次openid list 和company token
	XCP_LOGGER_INFO(_args->logger_debug, "--- prepare to start eqq mgt timer ---\n");
	Select_Timer *timer = new Select_Timer;
	EQQ_Timer_handler *eqq_thandler = new EQQ_Timer_handler;
	nRet = timer->register_timer_handler(eqq_thandler, 600000000);
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(_args->logger_debug, "register eqq mgt timer handler falied, ret:%d\n", nRet);
		return nRet;
	}
		
	nRet = timer->init();
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(_args->logger_debug, "int eqq mgt timer failed, ret:%d\n", nRet);
		return nRet;
	}

	nRet = timer->run();
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(_args->logger_debug, "eqq mgt timer run failed, ret:%d\n", nRet);
		return nRet;
	}
	XCP_LOGGER_INFO(_args->logger_debug, "=== complete to start eqq mgt timer ===\n");
	
	return 0;
	
}


int eqq_mgt::update_openid_list()
{
	int nRet = 0;

	char url[1024] = {0};
	snprintf(url, 1024, "%s?company_id=%s&company_token=%s&app_id=%s&client_ip=127.0.0.1&oauth_version=2&timestamp=0",
		EQQ_GET_USER_LIST.c_str(), _args->conf_mgt->_stEQQ._company_id.c_str(), _args->conf_mgt->_stEQQ._company_token.c_str(),
		_args->conf_mgt->_stEQQ._appid.c_str());
	std::string rsp = "";
	nRet = Curl_Tool::http_get(url, rsp, _args->conf_mgt->_conn_timeout, _args->conf_mgt->_rw_timeout);
	if(nRet != 0)
	{
		printf("send update_openid_list req failed, ret:%d\n", nRet);
		XCP_LOGGER_ERROR(_args->logger_debug, "send update_openid_list req failed, ret:%d\n", nRet);
	}
	else
	{
		printf("send update_openid_list req success.\n");
		//printf("openid_list(%u):%s\n", (unsigned int)rsp.size(), rsp.c_str());

		Thread_Mutex_Guard guard(_mutex);
		nRet = XProtocol::get_openid_list(rsp, _openid);
		if(nRet != 0)
		{
			printf("get_openid_list failed, ret:%d\n", nRet);
			XCP_LOGGER_ERROR(_args->logger_debug, "update_openid_list failed, ret:%d\n", nRet);
		}
		else
		{
			printf("update_openid_list(%u) success.\n", (unsigned int)_openid.size());
			XCP_LOGGER_ERROR(_args->logger_debug, "update_openid_list(%u) success.\n", (unsigned int)_openid.size());
		}
	}
	
	return nRet;
	
}



int eqq_mgt::update_eqq_group_list()
{
	int nRet = 0;

	char url[1024] = {0};
	snprintf(url, 1024, "%s?company_id=%s&company_token=%s&app_id=%s&client_ip=127.0.0.1&oauth_version=2&timestamp=0",
		EQQ_GET_GROUP_LIST.c_str(), _args->conf_mgt->_stEQQ._company_id.c_str(), _args->conf_mgt->_stEQQ._company_token.c_str(),
		_args->conf_mgt->_stEQQ._appid.c_str());

	std::string rsp = "";
	nRet = Curl_Tool::http_get(url, rsp, _args->conf_mgt->_conn_timeout, _args->conf_mgt->_rw_timeout);
	if(nRet != 0)
	{
		printf("send update_group_list req failed, ret:%d\n", nRet);
		XCP_LOGGER_ERROR(_args->logger_debug, "send update_group_list req failed, ret:%d\n", nRet);
	}
	else
	{
		printf("send update_group_list req success.\n");
		//printf("group_list(%u):%s\n", (unsigned int)rsp.size(), rsp.c_str());

		Thread_Mutex_Guard guard(_mutex);
		nRet = XProtocol::get_eqq_group_list(rsp, _eqq_group_id);
		if(nRet != 0)
		{
			printf("get_eqq_group_list failed, ret:%d\n", nRet);
			XCP_LOGGER_ERROR(_args->logger_debug, "get_eqq_group_list failed, ret:%d\n", nRet);
		}
		else
		{
			printf("update_eqq_group_list(%u) success.\n", (unsigned int)_eqq_group_id.size());
			XCP_LOGGER_ERROR(_args->logger_debug, "update_eqq_group_list(%u) success.\n", (unsigned int)_eqq_group_id.size());
		}
		
	}
	
	return nRet;

}



std::map<std::string, std::string> eqq_mgt::get_openid_list()
{
	Thread_Mutex_Guard guard(_mutex);
	return _openid;
}




std::map<std::string, std::string> eqq_mgt::get_eqq_group_list()
{
	Thread_Mutex_Guard guard(_mutex);
	return _eqq_group_id;
}



int eqq_mgt::send_broadcast(std::string &title, std::string &content, std::string &receiver, std::string &groups, std::string &err_msg)
{
	int nRet = 0;

	char post[8192] = {0};
	snprintf(post, 8192, "company_id=%s&company_token=%s&app_id=%s&client_ip=127.0.0.1&oauth_version=2&is_online=0&recv_open_ids=%s&recv_dept_ids=%s&is_rich=1&title=%s&content=%s",
		_args->conf_mgt->_stEQQ._company_id.c_str(), _args->conf_mgt->_stEQQ._company_token.c_str(),
		_args->conf_mgt->_stEQQ._appid.c_str(), receiver.c_str(), groups.c_str(),
		title.c_str(), content.c_str());
	
	XCP_LOGGER_INFO(_args->logger_debug, "===post:%s\n", post);
	
	std::string rsp = "";
	nRet = Curl_Tool::http_post_msg(EQQ_SEND_BROADCAST, post, rsp);
	if(nRet != 0)
	{
		XCP_LOGGER_INFO(_args->logger_debug, "send eqq broadcast failed, ret:%d\n", nRet);
	}
	else
	{
		XCP_LOGGER_INFO(_args->logger_debug, "send eqq broadcast success, rsp(%u):%s\n", rsp.size(), rsp.c_str());
		return XProtocol::get_eqq_rsp_status(rsp, err_msg);
	}
	
	return nRet;

}



