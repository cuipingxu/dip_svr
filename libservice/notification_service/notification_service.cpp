
#include "notification_service.h"
#include "libcomm/protocol.h"
#include "base/base_curl.h"
#include "base/base_string.h"
#include "base/base_url_parser.h"
#include "eqq_mgt.h"

notification_service::notification_service() 
{

}


notification_service::~notification_service() 
{

}

  
int notification_service::init(service_args* args)
{
	int nRet = 0;
	
	_args = args;
	std::string conf = _args->conf_mgt->_conf;
	if(conf == "")
	{
		printf("init notification_service failed, conf is empty.\n");
		return -1;
	}

	nRet = _args->conf_mgt->init_notification_cfg(std::string("../conf/")+conf);
	if(nRet != 0)
	{
		printf("init_notification_cfg failed, ret:%d\n", nRet);
		return nRet;
	}

	if(!_args->conf_mgt->_stEQQ._appid.empty())
	{
		nRet = PSGT_EQQ_Mgt->init(args);
	}
	
	return nRet;
	
}



int notification_service::process(const std::string& req, std::string& rsp, std::string& debug_info, bool &is_obj)
{
	int nRet = 0;

	XCP_LOGGER_INFO(_args->logger_debug, "prepare to process notification service, req(%u):%s\n", req.size(), req.c_str());

	//返回串是json对象
	is_obj = true;

	std::string cmd = "";
	std::string title = "";
	std::string content = "";
	std::string sender = "";
	std::vector<std::string> receiver;
	std::vector<std::string> groups;
	std::vector<std::string> CC;
	nRet = XProtocol::get_notification(req, cmd, title, content, sender, receiver, groups, CC, debug_info);
	if(nRet != 0)
	{
		XCP_LOGGER_INFO(_args->logger_debug, "get_notification failed, ret:%d\n", nRet);
		return nRet;
	}

	std::string new_title = URL_Parser::unescape(title);
	std::string new_content = URL_Parser::unescape(content);

	//printf("new_title:%s\n", new_title.c_str());
	//printf("new_content:%s\n", new_content.c_str());

	if((cmd == "notification_mail") && (!_args->conf_mgt->_stEmail._host.empty()))
	{	
		nRet = Curl_Tool::smtp(_args->conf_mgt->_stEmail._host, _args->conf_mgt->_stEmail._port, 
			_args->conf_mgt->_stEmail._user, _args->conf_mgt->_stEmail._pwd,
			_args->conf_mgt->_stEmail._user, receiver, CC, new_title, new_content);
		if(nRet != 0)
		{
			XCP_LOGGER_INFO(_args->logger_debug, "smtp failed, ret:%d\n", nRet);
			debug_info = "smtp send failed!";
			return nRet;
		}
		
	}
	else if((cmd == "notification_EQQ") && (!_args->conf_mgt->_stEQQ._appid.empty()))
	{		
		std::string recipients_list = "";
		std::string dept_list = "";
		if(!receiver.empty())
		{	
			std::map<std::string, std::string> openid_list = PSGT_EQQ_Mgt->get_openid_list();
			
			//转换成openid
			bool first = true;
			for(unsigned int i=0; i<receiver.size(); i++)
			{
				std::map<std::string, std::string>::iterator itr = openid_list.find(receiver[i]);
				if(itr != openid_list.end())
				{
					if(first)
					{
						recipients_list = itr->second;
						first = false;
						continue;
					}
					recipients_list += (std::string(",") + itr->second);
					
				}
				else
				{
					XCP_LOGGER_INFO(_args->logger_debug, "can't find openid by no account(%s)\n", receiver[i].c_str());
				}
			}
			
			if(recipients_list.empty())
			{
				XCP_LOGGER_INFO(_args->logger_debug, "recipients_list is empty.\n");
				debug_info = "recipients_list is empty.";
			}

		}


		if(!groups.empty())
		{
			std::map<std::string, std::string> group_list = PSGT_EQQ_Mgt->get_eqq_group_list();
			
			//转换成group id
			bool first = true;
			for(unsigned int i=0; i<groups.size(); i++)
			{
				std::map<std::string, std::string>::iterator itr = group_list.find(groups[i]);
				if(itr != group_list.end())
				{
					if(first)
					{
						dept_list = itr->second;
						first = false;
						continue;
					}
					dept_list += (std::string(",") + itr->second);
					
				}
				else
				{
					XCP_LOGGER_INFO(_args->logger_debug, "can't find dept id by no group(%s)\n", groups[i].c_str());
				}
			}
			
			if(dept_list.empty())
			{
				XCP_LOGGER_INFO(_args->logger_debug, "dept_list is empty.\n");
				debug_info = "dept_list is empty.";
			}

		}
		

		//发广播消息
		nRet = PSGT_EQQ_Mgt->send_broadcast(title, content, recipients_list, dept_list, debug_info);
		if(nRet != 0)
		{
			XCP_LOGGER_INFO(_args->logger_debug, "EQQ send_broadcast failed, ret:%d, debug_info:%s\n", nRet, debug_info.c_str());
		}
		else
		{
			XCP_LOGGER_INFO(_args->logger_debug, "EQQ send_broadcast success.\n");
		}
		
	}
	else
	{
		XCP_LOGGER_INFO(_args->logger_debug, "invalid cmd:%s\n", cmd.c_str());
		debug_info = "invalid cmd.";
		return -1;
	}
	
	XCP_LOGGER_INFO(_args->logger_debug, "complete to process notification service, ret:%d\n", nRet);
	
	return nRet;
	
}



int notification_service::release()
{
	int nRet = 0;

	return nRet;
}


extern "C" 
{
	int get_service_object(service_object *&obj)
	{
		obj = new notification_service;
		return 0;
	}
}


