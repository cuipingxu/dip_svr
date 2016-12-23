
#include "standard_service.h"
#include "libcomm/protocol.h"
#include "base/base_kv_parser.h"
#include "base/base_md5.h"
#include "base/base_curl.h"



standard_service::standard_service() 
{

}


standard_service::~standard_service() 
{

}

  
int standard_service::init(service_args* args)
{
	_args = args;
	std::string conf = _args->conf_mgt->_conf;
	if(conf == "")
	{
		printf("init standard_service failed, conf is empty.\n");
		return -1;
	}

	return _args->conf_mgt->init_service_protocol(std::string("../conf/")+conf);
}



int standard_service::process(const std::string& req, std::string& rsp, std::string& debug_info, bool &is_obj)
{
	int nRet = 0;

	XCP_LOGGER_INFO(_args->logger_debug, "standard_service::process, req(%u):%s\n", req.size(), req.c_str());

	//���ش���json����
	is_obj = true;
		
	std::string cmd = "";
	std::string body = "";
	nRet = XProtocol::get_body(req, cmd, body);
	if(nRet != 0)
	{
		XCP_LOGGER_INFO(_args->logger_debug, "get_body failed, req(%u):%s\n", req.size(), req.c_str());
		return nRet;
	}

	std::string _new_url = "";
	std::string mode = "";
	std::map<std::string, StProtocol> protocols = _args->conf_mgt->get_service_protocol();
	std::map<std::string, StProtocol>::iterator itr = protocols.find(cmd);
	if(itr != protocols.end())
	{
		//��������
		mode = itr->second._mode;
		_new_url = new_url(body, itr->second);
	}
	else
	{
		//ȷ����û��ͳһ�ķ������
		itr = protocols.find("others");
		if(itr == protocols.end())
		{
			XCP_LOGGER_INFO(_args->logger_debug, "can't process the req(%u):%s\n", req.size(), req.c_str());
			return -1;
		}

		//ͳһ����
		mode = itr->second._mode;
		_new_url = new_url(body, itr->second);
		
	}

	XCP_LOGGER_INFO(_args->logger_debug, "prepare to %s, url:%s\n", mode.c_str(), _new_url.c_str());	
	if((mode == "http_get") || (mode == "https_get"))
	{
		nRet = Curl_Tool::http_get(_new_url, rsp, 
		_args->conf_mgt->_conn_timeout, _args->conf_mgt->_rw_timeout);
	}
	else
	{
		//no to do
	}

	XCP_LOGGER_INFO(_args->logger_debug, "complete to %s, ret:%d, rsp(%u):%s\n", 
		mode.c_str(), nRet, rsp.size(), rsp.c_str());
		
	if(nRet != 0)
	{
		rsp = "";
		debug_info = "process standard service failed.";
	}
	
	return nRet;
	
}



int standard_service::release()
{
	int nRet = 0;

	return nRet;
}


std::string standard_service::new_url(const std::string &body, StProtocol &stProtocol)
{
	std::string url = "";

	//����ҵ��KV
	KV_Parser parser;
	parser.msg(body);

	if(parser["username"].empty() && parser["time"].empty() && parser["serverid"].empty())
	{
		return url;
	}

	//�����µ�����׷�Ӽ��ܴ�
	//md5(username+time+serverid+md5(key+site))
	std::string tmp = stProtocol._key+ parser["site"];
	std::string md5_value = md5(tmp.c_str());

	tmp = "";
	for(unsigned int i=0; i<stProtocol._sign.size(); ++i )
	{
		tmp += parser[stProtocol._sign[i]];
	}
	tmp += md5_value;

	printf("plaintext sign:%s\n", tmp.c_str());
	
	std::string sign = md5(tmp.c_str());
	std::string new_body = body + std::string("&sign=") + sign;

	//���¼���url
	url = stProtocol._url;
	std::string::size_type pos = url.find("#");
	if(pos != std::string::npos)
	{
		url.replace(pos, 1, parser["serverid"]);
	}
	
	//�����µ�url ��
	char new_url[1024] = {0};
	snprintf(new_url, 1024, "%s?%s", url.c_str(), new_body.c_str());
	printf("new_url: %s\n", new_url);
	
	return new_url;

}




extern "C" 
{
	int get_service_object(service_object *&obj)
	{
		obj = new standard_service;
		return 0;
	}
}


