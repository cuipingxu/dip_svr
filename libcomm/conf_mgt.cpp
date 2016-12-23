
#include "base/base_os.h"
#include "base/base_convert.h"
#include "base/base_string.h"
#include "base/base_xml_parser.h"
#include "conf_mgt.h"



Conf_Mgt::Conf_Mgt(): _id(""), _thr_num(0), _max_queue_size(0), _rate_limiting(0), _category(""), _regional(""),
	_req_port(0), _worker_port(0), _unregister_failed_num(3), _unregister_timeout_num(5),
	_group_id(""), _service_id(""), _server_id(""), _cmd(""), _plug_in(""), _conf(""), 
	_key(""), _conn_timeout(3), _rw_timeout(3)
{

}


Conf_Mgt::~Conf_Mgt()
{

}


int Conf_Mgt::init_router(const std::string &cfg)
{
	int nRet = 0;

	_cfg_router = cfg;
		
	XML_Parser _parser;
	nRet = _parser.parse_file(cfg);
	if(nRet != 0)
	{
		printf("init conf mgt failed, ret:%d, cfg:%s\n", nRet, cfg.c_str());
		return nRet;
	}

	//id
	XML_Node node;
	nRet = _parser.get_node("dip_router/system/id", node);
	if(nRet != 0)
	{
		printf("get id failed, ret:%d\n", nRet);
		return nRet;
	}
	_id = node.get_text();
	if(_id == "")
	{
		printf("id is empty\n");
		return -1;
	}

	//req_port
	nRet = _parser.get_node("dip_router/system/req_port", node);
	if(nRet != 0)
	{
		printf("get req_port failed, ret:%d\n", nRet);
		return nRet;
	}
	else
	{
		_req_port = strTo<unsigned short>(node.get_text());
		if(_req_port == 0)
		{
			printf("req_port is 0\n");
			return -1;
		}
	}


	//worker_port
	nRet = _parser.get_node("dip_router/system/worker_port", node);
	if(nRet != 0)
	{
		printf("get worker_port failed, ret:%d\n", nRet);
		return nRet;
	}
	else
	{
		_worker_port = strTo<unsigned short>(node.get_text());
		if(_req_port == 0)
		{
			printf("worker_port is 0\n");
			return -1;
		}		
	}
	
	
	//thr_num
	nRet = _parser.get_node("dip_router/system/thr_num", node);
	if(nRet != 0)
	{
		printf("get thr_num failed, ret:%d\n", nRet);
		get_cpu_number_proc(_thr_num);
	}
	else
	{
		_thr_num = strTo<unsigned short>(node.get_text());
	}
	
	//max_queue_size
	nRet = _parser.get_node("dip_router/system/max_queue_size", node);
	if(nRet != 0)
	{
		printf("get max_queue_size failed, ret:%d\n", nRet);
	}
	_max_queue_size = strTo<unsigned int>(node.get_text());
	if(_max_queue_size < 100000)
	{
		printf("max_queue_size < 100000\n");
		_max_queue_size = 100000;
	}

	//rate_limiting
	nRet = _parser.get_node("dip_router/system/rate_limiting", node);
	if(nRet != 0)
	{
		printf("get rate_limiting failed, ret:%d\n", nRet);
	}
	else
	{
		_rate_limiting = strTo<unsigned int>(node.get_text());
	}

	//unregister_failed_num
	nRet = _parser.get_node("dip_router/system/unregister_failed_num", node);
	if(nRet != 0)
	{
		printf("get unregister_failed_num failed, ret:%d\n", nRet);
	}
	_unregister_failed_num = strTo<unsigned int>(node.get_text());
	if(_unregister_failed_num == 0)
	{
		printf("max_queue_size is 0\n");
		_unregister_failed_num = 3;
	}

	//unregister_timeout_num
	nRet = _parser.get_node("dip_router/system/unregister_timeout_num", node);
	if(nRet != 0)
	{
		printf("get unregister_timeout_num failed, ret:%d\n", nRet);
	}
	_unregister_timeout_num = strTo<unsigned int>(node.get_text());
	if(_unregister_timeout_num == 0)
	{
		printf("max_queue_size is 0\n");
		_unregister_timeout_num = 5;
	}

	//white list
	std::string white_list_file = "";
	nRet = _parser.get_node("dip_router/system/white_list", node);
	if(nRet != 0)
	{
		printf("get white list failed, ret:%d\n", nRet);
	}
	else
	{
		white_list_file = node.get_text();
		if(white_list_file == "")
		{
			printf("white_list is empty\n");
			return -1;
		}
		init_white_list(std::string("../conf/")+white_list_file);
	}

	//logsys
	nRet = _parser.get_node("dip_router/logsys", node);
	if(nRet != 0)
	{
		printf("get logsys failed, ret:%d\n", nRet);
		return nRet;
	}

	//logsys.category
	_category = node.get_attr_str("category");
	if(_category == "")
	{
		printf("logsys category is empty\n");
		return -1;
	}

	//logsys.regional
	_regional = node.get_attr_str("regional");
	if(_regional == "")
	{
		printf("logsys regional is empty\n");
		_regional = "LOCAL";
	}

	//logsys.svr
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("dip_router/logsys/svr", vecNode);
	if(nRet != 0)
	{
		printf("get logsys svr failed, ret:%d\n", nRet);
		return 0;
	}

	if(vecNode.empty())
	{
		printf("logsys is empty\n");
		return 0;
	}

	StServer stServer;
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		stServer._ip= vecNode[i].get_attr_str("ip");
		trim(stServer._ip);
		if(stServer._ip == "")
		{
			printf("logsys ip is empty\n");
			return -1;
		}
		
		stServer._port = strTo<unsigned short>(vecNode[i].get_attr_str("port"));
		if(stServer._port == 0)
		{
			printf("logsys port is empty\n");
			return -1;
		}
		_logsys.push_back(stServer);
	}
	
	if(_logsys.empty())
	{
		printf("_logsys is empty\n");
	}
	
	return nRet;

}



int Conf_Mgt::init_worker(const std::string &cfg)
{
	int nRet = 0;

	_cfg_worker = cfg;
	
	XML_Parser _parser;
	nRet = _parser.parse_file(cfg);
	if(nRet != 0)
	{
		printf("init conf mgt failed, ret:%d, cfg:%s\n", nRet, cfg.c_str());
		return nRet;
	}

	//id
	XML_Node node;
	nRet = _parser.get_node("dip_worker/system/id", node);
	if(nRet != 0)
	{
		printf("get id failed, ret:%d\n", nRet);
		return nRet;
	}
	_id = node.get_text();
	if(_id == "")
	{
		printf("id is empty\n");
		return -1;
	}
	
	//thr_num
	nRet = _parser.get_node("dip_worker/system/thr_num", node);
	if(nRet != 0)
	{
		printf("get thr_num failed, ret:%d\n", nRet);
		get_cpu_number_proc(_thr_num);
	}
	else
	{
		_thr_num = strTo<unsigned short>(node.get_text());
	}
	
	//max_queue_size
	nRet = _parser.get_node("dip_worker/system/max_queue_size", node);
	if(nRet != 0)
	{
		printf("get max_queue_size failed, ret:%d\n", nRet);
	}
	_max_queue_size = strTo<unsigned int>(node.get_text());
	if(_max_queue_size < 100000)
	{
		printf("max_queue_size < 100000\n");
		_max_queue_size = 100000;
	}

	//rate_limiting
	nRet = _parser.get_node("dip_worker/system/rate_limiting", node);
	if(nRet != 0)
	{
		printf("get rate_limiting failed, ret:%d\n", nRet);
	}
	else
	{
		_rate_limiting = strTo<unsigned int>(node.get_text());
	}

	//group
	nRet = _parser.get_node("dip_worker/system/group", node);
	if(nRet != 0)
	{
		printf("get group failed, ret:%d\n", nRet);
		return nRet;
	}

	//group id  ---  目前dip 以业务id 作路由依据
	_group_id = node.get_attr_str("group_id");
	if(_group_id == "")
	{
		printf("group_id is empty\n");
		return -1;
	}

	//service id
	_service_id = node.get_attr_str("service_id");
	if(_service_id == "")
	{
		printf("service_id is empty\n");
		_service_id = _group_id;
	}
	

	//server id
	_server_id = node.get_attr_str("server_id");
	if(_server_id == "")
	{
		printf("server_id is empty\n");
		_server_id = "*";
	}

	//cmd
	_cmd = node.get_attr_str("cmd");
	if(_cmd == "")
	{
		printf("cmd is empty\n");
		_cmd = "*";
	}

	//plug_in
	_plug_in = node.get_attr_str("plug-in");
	if(_plug_in == "")
	{
		printf("plug-in is empty\n");
		return -1;
	}

	//conf
	_conf = node.get_attr_str("conf");
	if(_conf == "")
	{
		printf("conf is empty\n");
	}
	printf("_conf: %s\n", _conf.c_str());

	//routers
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("dip_worker/router/svr", vecNode);
	if(nRet != 0)
	{
		printf("get router failed, ret:%d\n", nRet);
		return nRet;
	}

	if(vecNode.empty())
	{
		printf("routers is empty\n");
		return -1;
	}

	StServer stServer;
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		stServer._ip= vecNode[i].get_attr_str("ip");
		trim(stServer._ip);
		if(stServer._ip == "")
		{
			printf("router ip is empty\n");
			return -1;
		}
		
		stServer._port = strTo<unsigned short>(vecNode[i].get_attr_str("port"));
		if(stServer._port == 0)
		{
			printf("router port is empty\n");
			return -1;
		}
		_routers.push_back(stServer);
	}
	
	if(_routers.empty())
	{
		printf("_router is empty\n");
		return -1;
	}

	//logsys
	nRet = _parser.get_node("dip_worker/logsys", node);
	if(nRet != 0)
	{
		printf("get logsys failed, ret:%d\n", nRet);
		return nRet;
	}

	//logsys.category
	_category = node.get_attr_str("category");
	if(_category == "")
	{
		printf("logsys category is empty\n");
		return -1;
	}

	//logsys.regional
	_regional = node.get_attr_str("regional");
	if(_regional == "")
	{
		printf("logsys regional is empty\n");
		_regional = "LOCAL";
	}

	//logsys.svr
	vecNode.clear();
	nRet = _parser.get_nodes("dip_worker/logsys/svr", vecNode);
	if(nRet != 0)
	{
		printf("get logsys failed, ret:%d\n", nRet);
		return 0;
	}

	if(vecNode.empty())
	{
		printf("logsys is empty\n");
		return 0;
	}

	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		stServer._ip= vecNode[i].get_attr_str("ip");
		trim(stServer._ip);
		if(stServer._ip == "")
		{
			printf("logsys ip is empty\n");
			return -1;
		}
		
		stServer._port = strTo<unsigned short>(vecNode[i].get_attr_str("port"));
		if(stServer._port == 0)
		{
			printf("logsys port is empty\n");
			return -1;
		}
		_logsys.push_back(stServer);
	}
	
	if(_logsys.empty())
	{
		printf("_logsys is empty\n");
	}
	
	return nRet;
	
}




int Conf_Mgt::init_service_protocol(const std::string &cfg)
{
	int nRet = 0;

	_cfg_service_protocol = cfg;
	
	XML_Parser _parser;
	nRet = _parser.parse_file(cfg);
	if(nRet != 0)
	{
		printf("init service protocol failed, ret:%d, cfg:%s\n", nRet, cfg.c_str());
		return nRet;
	}

	//key
	XML_Node node;
	nRet = _parser.get_node("protocols/system/key", node);
	if(nRet != 0)
	{
		printf("get key failed, ret:%d\n", nRet);
	}
	else
	{
		_key = node.get_text();
	}

	//conn_timeout
	nRet = _parser.get_node("protocols/system/conn_timeout", node);
	if(nRet != 0)
	{
		printf("get conn_timeout failed, ret:%d\n", nRet);
	}
	else
	{
		_conn_timeout = strTo<unsigned int>(node.get_text());
		if(_conn_timeout == 0)
		{
			_conn_timeout = 3;
		}
	}

	//rw_timeout
	nRet = _parser.get_node("protocols/system/rw_timeout", node);
	if(nRet != 0)
	{
		printf("get rw_timeout failed, ret:%d\n", nRet);
	}
	else
	{
		_rw_timeout = strTo<unsigned int>(node.get_text());
		if(_rw_timeout == 0)
		{
			_rw_timeout = 3;
		}		
	}
	
	//protocols
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("protocols/protocol", vecNode);
	if(nRet != 0)
	{
		printf("get routers failed, ret:%d\n", nRet);
		return nRet;
	}

	if(vecNode.empty())
	{
		printf("protocol is empty\n");
		return -1;
	}

	//获取protocol 信息
	StProtocol stProtocol;
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		//cmd
		stProtocol._cmd= vecNode[i].get_attr_str("cmd");
		if(stProtocol._cmd == "")
		{
			printf("protocol cmd is empty\n");
			return -1;
		}


		//mode
		stProtocol._mode = vecNode[i].get_attr_str("mode");
		if(stProtocol._mode == "")
		{
			printf("protocol mode is empty\n");
			return -1;
		}
		else if(stProtocol._mode != "")
		{
			if((stProtocol._mode != "http_get") && (stProtocol._mode != "https_get"))
			{
				printf("invalid protocol mode:%s\n", stProtocol._mode.c_str());
				return -1;
			}
		}


		//url
		stProtocol._url = vecNode[i].get_attr_str("url");
		if(stProtocol._url == "")
		{
			printf("protocol url is empty\n");
			return -1;
		}


		//key
		stProtocol._key = vecNode[i].get_attr_str("key");
		if(stProtocol._key == "")
		{
			printf("protocol key is empty\n");
			if(_key == "")
			{
				printf("key is empty also\n");
				return -1;
			}
			else
			{
				//可以替换全局key
				stProtocol._key = _key;
			}
		}
		

		//sign
		std::string sign = vecNode[i].get_attr_str("sign");
		if(sign == "")
		{
			printf("sign is empty\n");
			return -1;
		}
		
		std::vector<std::string> vecStr;
		split(sign, ", \t", vecStr);
		stProtocol._sign = vecStr;
		_protocols.insert(std::make_pair(stProtocol._cmd, stProtocol));
		
	}
	
	if(_protocols.empty())
	{
		printf("_protocols is empty\n");
		return -1;
	}

	return nRet;

}



int Conf_Mgt::init_white_list(const std::string &cfg)
{
	int nRet = 0;

	_cfg_white_list = cfg;
	
	XML_Parser _parser;
	nRet = _parser.parse_file(cfg);
	if(nRet != 0)
	{
		printf("init white list failed, ret:%d, cfg:%s\n", nRet, cfg.c_str());
		return 0;
	}

	//logsys
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("white_list/svr", vecNode);
	if(nRet != 0)
	{
		printf("get white_list svr failed, ret:%d\n", nRet);
		return 0;
	}

	if(vecNode.empty())
	{
		printf("logsys is empty\n");
		return 0;
	}

	std::string ip = "";
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		ip = vecNode[i].get_attr_str("ip");
		trim(ip);
		if(ip == "")
		{
			printf("ip is empty\n");
			continue;
		}
		
		_white_list.insert(ip);
	}
	
	if(_white_list.empty())
	{
		printf("_white_list is empty\n");
	}
		
	return nRet;
	
}




int Conf_Mgt::init_notification_cfg(const std::string &cfg)
{
	int nRet = 0;

	_cfg_noticication = cfg;
	
	XML_Parser _parser;
	nRet = _parser.parse_file(cfg);
	if(nRet != 0)
	{
		printf("init notification cfg failed, ret:%d, cfg:%s\n", nRet, cfg.c_str());
		return nRet;
	}

	//conn_timeout
	XML_Node node;
	nRet = _parser.get_node("notification/system/conn_timeout", node);
	if(nRet != 0)
	{
		printf("get conn_timeout failed, ret:%d\n", nRet);
	}
	else
	{
		_conn_timeout = strTo<unsigned int>(node.get_text());
		if(_conn_timeout == 0)
		{
			_conn_timeout = 3;
		}
	}

	//rw_timeout
	nRet = _parser.get_node("notification/system/rw_timeout", node);
	if(nRet != 0)
	{
		printf("get rw_timeout failed, ret:%d\n", nRet);
	}
	else
	{
		_rw_timeout = strTo<unsigned int>(node.get_text());
		if(_rw_timeout == 0)
		{
			_rw_timeout = 3;
		}		
	}
	
	//protocols
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("notification/protocol", vecNode);
	if(nRet != 0)
	{
		printf("get routers failed, ret:%d\n", nRet);
		return nRet;
	}

	if(vecNode.empty())
	{
		printf("protocol is empty\n");
		return -1;
	}

	//获取protocol 信息
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		//mode
		std::string mode = vecNode[i].get_attr_str("mode");
		if(mode == "")
		{
			printf("protocol mode is empty\n");
			return -1;
		}
		else if(mode != "")
		{
			if((mode != "email") && (mode != "EQQ"))
			{
				printf("invalid protocol mode:%s\n", mode.c_str());
				return -1;
			}
		}

		if(mode == "email")
		{
			_stEmail._host = vecNode[i].get_attr_str("host");
			if(_stEmail._host == "")
			{
				printf("protocol host is empty\n");
				return -1;
			}

			_stEmail._port = strTo<unsigned short>(vecNode[i].get_attr_str("port"));
			if(_stEmail._port == 0)
			{
				printf("protocol port is 0\n");
			}

			_stEmail._user = vecNode[i].get_attr_str("user");
			if(_stEmail._user == "")
			{
				printf("protocol user is empty\n");
				return -1;
			}

			_stEmail._pwd = vecNode[i].get_attr_str("pwd");
			if(_stEmail._pwd == "")
			{
				printf("protocol pwd is empty\n");
				return -1;
			}
			
		}
		else if(mode == "EQQ")
		{		
			_stEQQ._appid = vecNode[i].get_attr_str("app_id");
			if(_stEQQ._appid == "")
			{
				printf("protocol appid is empty\n");
				return -1;
			}

			_stEQQ._company_id = vecNode[i].get_attr_str("company_id");
			if(_stEQQ._company_id == "")
			{
				printf("protocol company_id is empty\n");
				return -1;
			}

			_stEQQ._company_token = vecNode[i].get_attr_str("company_token");
			if(_stEQQ._company_token == "")
			{
				printf("protocol company_token is empty\n");
				return -1;
			}
			

		}
		else
		{
			//no-todo
		}


		
	}

	if(_stEmail._host.empty() && _stEQQ._appid.empty())
	{
		printf("no protocol.\n");
		return -1;		
	}

	return nRet;

}


//--------------------------------------


int Conf_Mgt::refresh_router_logsys()
{
	return refresh_logsys("dip_router");
}


int Conf_Mgt::refresh_worker_logsys()
{
	return refresh_logsys("dip_worker");
}


int Conf_Mgt::refresh_logsys(const std::string &tag)
{	
	int nRet = 0;

	XML_Parser _parser;
	nRet = _parser.parse_file(_cfg_router);
	if(nRet != 0)
	{
		printf("refresh logsys failed, ret:%d, cfg:%s\n", nRet, _cfg_router.c_str());
		return -1;
	}

	//logsys
	XML_Node node;
	nRet = _parser.get_node(tag+std::string("/logsys"), node);
	if(nRet != 0)
	{
		printf("get logsys failed, ret:%d\n", nRet);
		return -2;
	}

	//logsys.category
	std::string category = "";
	category = node.get_attr_str("category");
	if(category == "")
	{
		printf("logsys category is empty\n");
		return -3;
	}

	//logsys.regional
	std::string regional = "";
	regional = node.get_attr_str("regional");
	if(regional == "")
	{
		printf("logsys regional is empty\n");
		regional = "LOCAL";
	}

	//logsys.svr
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes(tag+std::string("/logsys/svr"), vecNode);
	if(nRet != 0)
	{
		printf("get logsys svr failed, ret:%d\n", nRet);
		return -4;
	}

	if(vecNode.empty())
	{
		printf("logsys is empty\n");
		return -5;
	}

	std::vector<StServer> logsys;
	StServer stServer;
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		stServer._ip= vecNode[i].get_attr_str("ip");
		trim(stServer._ip);
		if(stServer._ip == "")
		{
			printf("logsys ip is empty\n");
			return -6;
		}
		
		stServer._port = strTo<unsigned short>(vecNode[i].get_attr_str("port"));
		if(stServer._port == 0)
		{
			printf("logsys port is empty\n");
			return -7;
		}
		logsys.push_back(stServer);
	}
	
	if(logsys.empty())
	{
		printf("logsys is empty\n");
		return -8;
	}

	//最小粒度锁保护
	Thread_Mutex_Guard guard(_mutex);
	_category = category;
	_regional = regional;
	_logsys = logsys;
	
	return nRet;

}


int Conf_Mgt::refresh_routers()
{
	int nRet = 0;

	XML_Parser _parser;
	nRet = _parser.parse_file(_cfg_worker);
	if(nRet != 0)
	{
		printf("refresh router failed, ret:%d, cfg:%s\n", nRet, _cfg_worker.c_str());
		return -1;
	}

	//routers
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("dip_worker/router/svr", vecNode);
	if(nRet != 0)
	{
		printf("get router failed, ret:%d\n", nRet);
		return -2;
	}

	if(vecNode.empty())
	{
		printf("routers is empty\n");
		return -3;
	}

	std::vector<StServer> routers;
	StServer stServer;
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		stServer._ip= vecNode[i].get_attr_str("ip");
		trim(stServer._ip);
		if(stServer._ip == "")
		{
			printf("router ip is empty\n");
			return -4;
		}
		
		stServer._port = strTo<unsigned short>(vecNode[i].get_attr_str("port"));
		if(stServer._port == 0)
		{
			printf("router port is empty\n");
			return -5;
		}
		routers.push_back(stServer);
	}
	
	if(routers.empty())
	{
		printf("router is empty\n");
		return -6;
	}

	Thread_Mutex_Guard guard(_mutex);
	_routers = routers;

	return nRet;

}




int Conf_Mgt::refresh_service_protocol()
{
	int nRet = 0;

	XML_Parser _parser;
	nRet = _parser.parse_file(_cfg_service_protocol);
	if(nRet != 0)
	{
		printf("refresh service protocol failed, ret:%d, cfg:%s\n", nRet, _cfg_service_protocol.c_str());
		return -1;
	}

	//key
	std::string key = "";
	XML_Node node;
	nRet = _parser.get_node("protocols/system/key", node);
	if(nRet != 0)
	{
		printf("get key failed, ret:%d\n", nRet);
	}
	else
	{
		key = node.get_text();
	}

	//conn_timeout
	unsigned int conn_timeout = 0;
	nRet = _parser.get_node("protocols/system/conn_timeout", node);
	if(nRet != 0)
	{
		printf("get conn_timeout failed, ret:%d\n", nRet);
	}
	else
	{
		conn_timeout = strTo<unsigned int>(node.get_text());
		if(conn_timeout == 0)
		{
			conn_timeout = 3;
		}
	}

	//rw_timeout
	unsigned int rw_timeout = 0;
	nRet = _parser.get_node("protocols/system/rw_timeout", node);
	if(nRet != 0)
	{
		printf("get rw_timeout failed, ret:%d\n", nRet);
	}
	else
	{
		rw_timeout = strTo<unsigned int>(node.get_text());
		if(rw_timeout == 0)
		{
			rw_timeout = 3;
		}		
	}
	
	//protocols
	std::map<std::string, StProtocol> protocols;
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("protocols/protocol", vecNode);
	if(nRet != 0)
	{
		printf("get routers failed, ret:%d\n", nRet);
		return -2;
	}

	if(vecNode.empty())
	{
		printf("protocol is empty\n");
		return -3;
	}

	//获取protocol 信息
	StProtocol stProtocol;
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		//cmd
		stProtocol._cmd= vecNode[i].get_attr_str("cmd");
		if(stProtocol._cmd == "")
		{
			printf("protocol cmd is empty\n");
			return -4;
		}


		//mode
		stProtocol._mode = vecNode[i].get_attr_str("mode");
		if(stProtocol._mode == "")
		{
			printf("protocol mode is empty\n");
			return -5;
		}
		else if(stProtocol._mode != "")
		{
			if((stProtocol._mode != "http_get") && (stProtocol._mode != "https_get"))
			{
				printf("invalid protocol mode:%s\n", stProtocol._mode.c_str());
				return -6;
			}
		}


		//url
		stProtocol._url = vecNode[i].get_attr_str("url");
		if(stProtocol._url == "")
		{
			printf("protocol url is empty\n");
			return -7;
		}


		//key
		stProtocol._key = vecNode[i].get_attr_str("key");
		if(stProtocol._key == "")
		{
			printf("protocol key is empty\n");
			if(key == "")
			{
				printf("key is empty also\n");
				return -8;
			}
			else
			{
				//可以替换全局key
				stProtocol._key = key;
			}
		}
		

		//sign
		std::string sign = vecNode[i].get_attr_str("sign");
		if(sign == "")
		{
			printf("sign is empty\n");
			return -9;
		}
		
		std::vector<std::string> vecStr;
		split(sign, ", \t", vecStr);
		stProtocol._sign = vecStr;
		protocols.insert(std::make_pair(stProtocol._cmd, stProtocol));
		
	}
	
	if(protocols.empty())
	{
		printf("protocols is empty\n");
		return -10;
	}

	Thread_Mutex_Guard guard(_mutex);
	_key = key;
	_conn_timeout = conn_timeout;
	_rw_timeout = rw_timeout;
	_protocols = protocols;

	return nRet;

}




int Conf_Mgt::refresh_white_list()
{
	int nRet = 0;

	XML_Parser _parser;
	nRet = _parser.parse_file(_cfg_white_list);
	if(nRet != 0)
	{
		printf("refresh white list, ret:%d, cfg:%s\n", nRet, _cfg_white_list.c_str());
		return -1;
	}

	//logsys
	std::vector<XML_Node> vecNode;
	nRet = _parser.get_nodes("white_list/svr", vecNode);
	if(nRet != 0)
	{
		printf("get white_list svr failed, ret:%d\n", nRet);
		return -2;
	}

	if(vecNode.empty())
	{
		printf("white_list is empty\n");
		return -3;
	}

	std::set<std::string> white_list;
	std::string ip = "";
	for(unsigned int i=0; i<vecNode.size(); ++i)
	{
		ip = vecNode[i].get_attr_str("ip");
		trim(ip);
		if(ip == "")
		{
			printf("ip is empty\n");
			continue;
		}
		
		white_list.insert(ip);
	}
	
	if(white_list.empty())
	{
		printf("white_list is empty\n");
		return -4;
	}

	Thread_Mutex_Guard guard(_mutex);
	_white_list = white_list;
		
	return nRet;

}


std::vector<StServer> Conf_Mgt::get_logsys()
{
	Thread_Mutex_Guard guard(_mutex);
	return _logsys;
}

std::vector<StServer> Conf_Mgt::get_routers()
{
	Thread_Mutex_Guard guard(_mutex);
	return _routers;
}

std::map<std::string, StProtocol> Conf_Mgt::get_service_protocol()
{
	Thread_Mutex_Guard guard(_mutex);
	return _protocols;
}

std::set<std::string> Conf_Mgt::get_white_list()
{
	Thread_Mutex_Guard guard(_mutex);
	return _white_list;
}


