
#include "protocol.h"
#include "base/base_time.h"
#include "common.h"
#include "cmd.h"
#include "json-c/json.h"


int XProtocol::get_req_head(const std::string &req, std::string &cmd, unsigned long long &time, std::string &req_id, 
	std::string &channel, std::string &peer, std::string &service_id, std::string &server_id)
{	
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	{"head":{"cmd":"register", "time":111111, "req_id":"xxxxxx", "channel":"bx", "peer":"127.0.0.1:5500", "service_id":"game1", "server_id":"100"}, "body":{"user_id":"xcp"}}
	*/
	//head
	json_object *head = NULL;
	if(!json_object_object_get_ex(root, "head", &head))
	{
		printf("it is invalid req, no head, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}

	//cmd
	json_object *_cmd = NULL;
	if(!json_object_object_get_ex(head, "cmd", &_cmd))
	{
		printf("it is invalid req, no cmd, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	cmd = json_object_get_string(_cmd);


	//time
	json_object *_time = NULL;
	if(!json_object_object_get_ex(head, "time", &_time))
	{
		//printf("it is invalid req, no time, req(%u):%s\n", req.size(), req.c_str());
		time = 0;
	}
	else
	{
		time = json_object_get_int64(_time);
	}
	

	//req_id
	json_object *_req_id = NULL;
	if(!json_object_object_get_ex(head, "req_id", &_req_id))
	{
		//printf("it is invalid req, no req id, req(%u):%s\n", req.size(), req.c_str());
		req_id = "";
	}
	else
	{
		req_id = json_object_get_string(_req_id);
	}

	//channel
	json_object *_channel = NULL;
	if(!json_object_object_get_ex(head, "channel", &_channel))
	{
		//printf("it is invalid req, no channel, req(%u):%s\n", req.size(), req.c_str());
		channel = "";
	}
	else
	{
		channel = json_object_get_string(_channel);
	}
	

	//peer
	json_object *_peer = NULL;
	if(!json_object_object_get_ex(head, "peer", &_peer))
	{
		//printf("it is invalid req, no peer, req(%u):%s\n", req.size(), req.c_str());
		peer = "";
	}
	else
	{
		peer = json_object_get_string(_peer);
	}


	//service_id
	json_object *_service_id = NULL;
	if(!json_object_object_get_ex(head, "service_id", &_service_id))
	{
		printf("it is invalid req, no service id, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	service_id = json_object_get_string(_service_id);

	/*
	//server_id
	json_object *_server_id = NULL;
	if(!json_object_object_get_ex(head, "server_id", &_server_id))
	{
		//printf("it is invalid req, no server id, req(%u):%s\n", req.size(), req.c_str());
	}
	else
	{
		server_id = json_object_get_string(_server_id);
	}
	*/

	//释放内存
	json_object_put(root);
	
	return 0;

}



int XProtocol::get_admin_head(const std::string &req, std::string &cmd, unsigned long long &time, 
	std::string &msg_id, std::string &req_id, int &err, std::string &err_info)
{
		
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	{"head":{"cmd":"xxxx", "time":111111, "req_id":"xxxxxx",  "msg_id":"yyyyy",  "err":0, "err_info":"xxxx"}, "body":xxxxxxx}
	*/
	//head
	json_object *head = NULL;
	if(!json_object_object_get_ex(root, "head", &head))
	{
		printf("it is invalid req, no head, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}

	//cmd
	json_object *_cmd = NULL;
	if(!json_object_object_get_ex(head, "cmd", &_cmd))
	{
		printf("it is invalid req, no cmd, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	cmd = json_object_get_string(_cmd);


	//time
	json_object *_time = NULL;
	if(!json_object_object_get_ex(head, "time", &_time))
	{
		//printf("it is invalid req, no time, req(%u):%s\n", req.size(), req.c_str());
		time = 0;
	}
	else
	{
		time = json_object_get_int64(_time);
	}
	

	//msg_id
	json_object *_msg_id = NULL;
	if(!json_object_object_get_ex(head, "msg_id", &_msg_id))
	{
		printf("it is invalid req, no msg id, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	msg_id = json_object_get_string(_msg_id);


	//req_id
	json_object *_req_id = NULL;
	if(!json_object_object_get_ex(head, "req_id", &_req_id))
	{
		//printf("it is invalid req, no req id, req(%u):%s\n", req.size(), req.c_str());
		req_id = "";
	}
	else
	{
		req_id = json_object_get_string(_req_id);
	}
	

	//err
	json_object *_err = NULL;
	if(!json_object_object_get_ex(head, "err", &_err))
	{
		//printf("it is invalid req, no err, req(%u):%s\n", req.size(), req.c_str());
		err = 0;
	}
	else
	{
		err = json_object_get_int(_err);
	}

	
	//err_info
	json_object *_err_info = NULL;
	if(!json_object_object_get_ex(head, "err_info", &_err_info))
	{
		//printf("it is invalid req, no err_info, req(%u):%s\n", req.size(), req.c_str());
		err_info = "";
	}
	else
	{
		err_info = json_object_get_string(_err_info);
	}

	//释放内存
	json_object_put(root);
	
	return 0;


}




int XProtocol::get_worker_info(const std::string &req, std::string &worker_id, 
		std::string &group_id, std::string &service_id, std::string &server_id, std::string &cmd)
{
		
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	{"head":{"cmd":"register", "time":111111, "msg_id":"xxxxxx"}, "body":{"id":"Log_Worker_127.0.0.1", "group_id":" BFWZ", " service_id":" BFWZ",  " server_id":" *|1-200|1,2,3-50,80-100",  "cmd":" *|1-200|1,2,3-50,80-100"}}
	*/
	//body
	json_object *body = NULL;
	if(!json_object_object_get_ex(root, "body", &body))
	{
		printf("it is invalid req, no body, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}

	//worker id
	json_object *_worker_id = NULL;
	if(!json_object_object_get_ex(body, "id", &_worker_id))
	{
		printf("it is invalid req, no id, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	worker_id = json_object_get_string(_worker_id);
	//printf("worker_id(%u):%s\n", worker_id.size(), worker_id.c_str());


	//group id
	json_object *_group_id = NULL;
	if(!json_object_object_get_ex(body, "group_id", &_group_id))
	{
		printf("it is invalid req, no group, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	group_id = json_object_get_string(_group_id);
	//printf("category(%u):%s\n", category.size(), category.c_str());


	//service id
	json_object *_service_id = NULL;
	if(!json_object_object_get_ex(body, "service_id", &_service_id))
	{
		printf("it is invalid req, no service id, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	service_id = json_object_get_string(_service_id);
	//printf("category(%u):%s\n", category.size(), category.c_str());


	//server id
	json_object *_server_id = NULL;
	if(!json_object_object_get_ex(body, "server_id", &_server_id))
	{
		printf("it is invalid req, no server id, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	server_id = json_object_get_string(_server_id);
	//printf("category(%u):%s\n", category.size(), category.c_str());


	//cmd
	json_object *_cmd = NULL;
	if(!json_object_object_get_ex(body, "cmd", &_cmd))
	{
		printf("it is invalid req, no cmd, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	cmd = json_object_get_string(_cmd);
	//printf("category(%u):%s\n", category.size(), category.c_str());	

	//释放内存
	json_object_put(root);
	
	return 0;

}





int XProtocol::get_rsp_staus(const std::string &req, std::string &cmd, unsigned long long &time, 
	std::string &msg_id, int &err, std::string &err_info)
{
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	{"head":{"cmd":"xxxx", "time":111111, "msg_id":"yyyyy",  "err":0, "err_info":"xxxx"}, "body":xxxxxxx}
	*/
	//head
	json_object *head = NULL;
	if(!json_object_object_get_ex(root, "head", &head))
	{
		printf("it is invalid req, no head, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}

	//cmd
	json_object *_cmd = NULL;
	if(!json_object_object_get_ex(head, "cmd", &_cmd))
	{
		printf("it is invalid req, no cmd, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	cmd = json_object_get_string(_cmd);


	//time
	json_object *_time = NULL;
	if(!json_object_object_get_ex(head, "time", &_time))
	{
		printf("it is invalid req, no time, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	time = json_object_get_int64(_time);


	//msg_id
	json_object *_msg_id = NULL;
	if(!json_object_object_get_ex(head, "msg_id", &_msg_id))
	{
		printf("it is invalid req, no msg id, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	msg_id = json_object_get_string(_msg_id);


	//err
	json_object *_err = NULL;
	if(!json_object_object_get_ex(head, "err", &_err))
	{
		printf("it is invalid req, no err, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	err = json_object_get_int(_err);

	
	//err_info
	json_object *_err_info = NULL;
	if(!json_object_object_get_ex(head, "err_info", &_err_info))
	{
		printf("it is invalid req, no err_info, req(%u):%s\n", req.size(), req.c_str());
		err_info = "";
	}
	else
	{
		err_info = json_object_get_string(_err_info);
	}

	//释放内存
	json_object_put(root);
	
	return 0;

}




int XProtocol::get_body(const std::string &req, std::string &cmd, std::string &body)
{
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	{"head":{"cmd":"log", "time":11111, "category":"oss", "key":"oss"}, "body":"xxxxxxxxxxxxx"}
	*/
	json_object *head = NULL;
	if(!json_object_object_get_ex(root, "head", &head))
	{
		printf("it is invalid req, no head, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}

	//cmd
	json_object *_cmd = NULL;
	if(!json_object_object_get_ex(head, "cmd", &_cmd))
	{
		printf("it is invalid req, no cmd, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	cmd = json_object_get_string(_cmd);
	
	//body
	json_object *_body = NULL;
	if(!json_object_object_get_ex(root, "body", &_body))
	{
		printf("it is invalid req, no body, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	body = json_object_get_string(_body);
	printf("cmd:%s, body(%u):%s\n", cmd.c_str(), body.size(), body.c_str());

	//释放内存
	json_object_put(root);
	
	return 0;

}



int XProtocol::get_notification(const std::string &req, std::string &cmd, 
	std::string &title, std::string &content, std::string &sender, 
	std::vector<std::string> &receiver, std::vector<std::string> &groups, std::vector<std::string> &CC, std::string &err_msg)
{
	int nRet = 0;
	
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		err_msg = "it is invalid json req!";
		return -1;
	}

	/*
	{"head":{"cmd":"notification_mail", "service_id":"notification", "server_id":"100"}, 
	"body":{"title":"it is title!", "content":"it is content!", "sender":"xcp", "receiver":["sky@r2games.com", "carl@r2games.com"], 
	"group":["\u8fd0\u8425\u90e8-Operation", "NPT"], "CC":["sky@r2games.com", "carl@r2games.com"]}}
	*/
	json_object *head = NULL;
	if(!json_object_object_get_ex(root, "head", &head))
	{
		printf("it is invalid req, no head, req(%u):%s\n", req.size(), req.c_str());
		err_msg = "no head is found!";
		json_object_put(root);
		return -1;
	}

	//cmd
	json_object *_cmd = NULL;
	if(!json_object_object_get_ex(head, "cmd", &_cmd))
	{
		err_msg = "no cmd is found!";
		printf("it is invalid req, no cmd, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	cmd = json_object_get_string(_cmd);
	if((cmd != "notification_EQQ") && (cmd != "notification_mail"))
	{
		printf("it is invalid req, cmd is invalid, req(%u):%s\n", req.size(), req.c_str());
		err_msg = "it is invalid req, cmd is invalid.";
		json_object_put(root);
		return -1;	
	}

	
	//body
	json_object *_body = NULL;
	if(!json_object_object_get_ex(root, "body", &_body))
	{
		err_msg = "no body is found!";
		printf("it is invalid req, no body, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	
	
	//title
	json_object *_title = NULL;
	if(!json_object_object_get_ex(_body, "title", &_title))
	{
		err_msg = "no title is found!";
		printf("it is invalid req, no title, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return -1;
	}
	
	json_type type = json_object_get_type(_title);
	if(type != json_type_string) 
	{
  		printf("it is invalid req, title isn't string, req(%u):%s\n", req.size(), req.c_str());
		err_msg = "it is invalid req, title isn't string.";
		json_object_put(root);
		return -2;      
    }
	title = json_object_get_string(_title);
	
	
	//content
	json_object *_content = NULL;
	if(!json_object_object_get_ex(_body, "content", &_content))
	{
		printf("it is invalid req, no content, req(%u):%s\n", req.size(), req.c_str());
		content = "";
	}
	else
	{
		json_type type = json_object_get_type(_content);
		if(type != json_type_string) 
		{
	  		printf("it is invalid req, content isn't string, req(%u):%s\n", req.size(), req.c_str());
			err_msg = "it is invalid req, content isn't string.";
			json_object_put(root);
			return -2;      
	    }
		content = json_object_get_string(_content);
	}

	//sender
	json_object *_sender = NULL;
	if(!json_object_object_get_ex(_body, "sender", &_sender))
	{
		printf("it is invalid req, no sender, req(%u):%s\n", req.size(), req.c_str());
		sender = "";
	}
	else
	{
		json_type type = json_object_get_type(_sender);
		if(type != json_type_string) 
		{
	  		printf("it is invalid req, sender isn't string, req(%u):%s\n", req.size(), req.c_str());
			err_msg = "it is invalid req, sender isn't string.";
			json_object_put(root);
			return -2;      
	    }
		sender = json_object_get_string(_sender);
	}


	//receiver
	json_object *_receiver = NULL;
	if(!json_object_object_get_ex(_body, "receiver", &_receiver))
	{
		printf("it is invalid req, no receiver, req(%u):%s\n", req.size(), req.c_str());
	}
	else
	{
		//判断receiver 是否是数组类型
		type = json_object_get_type(_receiver);
		if(type != json_type_array) 
		{
			printf("it is invalid req, receiver isn't array, req(%u):%s\n", req.size(), req.c_str());
			err_msg = "it is invalid req, receiver isn't array.";
			json_object_put(root);
			return -2;		
		}
		
		std::string receiver_one = "";
		int size = json_object_array_length(_receiver);
		for(int i=0; i<size; i++) 
		{
			json_object *_receiver_one = json_object_array_get_idx(_receiver, i);
			json_type type = json_object_get_type(_receiver_one);
			if(type != json_type_string) 
			{
				printf("it is invalid req, receiver isn't string, req(%u):%s\n", req.size(), req.c_str());
				err_msg = "it is invalid req, receiver isn't string.";
				json_object_put(root);
				return -2;		
			}	
			receiver_one = json_object_get_string(_receiver_one);
			receiver.push_back(receiver_one);
		}
		
		if(receiver.empty())
		{
			err_msg = "receiver is empty.";
			printf("receiver is empty.\n");	
		}

	}
	

	//group
	json_object *_group = NULL;
	if(!json_object_object_get_ex(_body, "group", &_group))
	{
		printf("it is invalid req, no group, req(%u):%s\n", req.size(), req.c_str());		
	}
	else
	{
		//判断receiver 是否是数组类型
		type = json_object_get_type(_group);
		if(type != json_type_array) 
		{
			printf("it is invalid req, group isn't array, req(%u):%s\n", req.size(), req.c_str());
			err_msg = "it is invalid req, group isn't array.";
			json_object_put(root);
			return -2;		
		}
		
		std::string group_one = "";
		int size = json_object_array_length(_group);
		for(int i=0; i<size; i++) 
		{
			json_object *_group_one = json_object_array_get_idx(_group, i);
			json_type type = json_object_get_type(_group_one);
			if(type != json_type_string) 
			{
				printf("it is invalid req, group isn't string, req(%u):%s\n", req.size(), req.c_str());
				err_msg = "it is invalid req, group isn't string.";
				json_object_put(root);
				return -2;		
			}	
			group_one = json_object_get_string(_group_one);
			groups.push_back(group_one);
		}
		
		if(groups.empty())
		{
			err_msg = "groups is empty.";
			printf("groups is empty.\n");	
		}

	}


	//CC
	json_object *_CC = NULL;
	if(!json_object_object_get_ex(_body, "CC", &_CC))
	{
		printf("it is invalid req, no CC, req(%u):%s\n", req.size(), req.c_str());		
	}
	else
	{
		//判断CC 是否是数组类型
		type = json_object_get_type(_CC);
		if(type != json_type_array) 
		{
			printf("it is invalid req, CC isn't array, req(%u):%s\n", req.size(), req.c_str());
			err_msg = "it is invalid req, CC isn't array.";
			json_object_put(root);
			return -2;		
		}
		
		std::string CC_one = "";
		int size = json_object_array_length(_CC);
		for(int i=0; i<size; i++) 
		{
			json_object *_CC_one = json_object_array_get_idx(_CC, i);
			json_type type = json_object_get_type(_CC_one);
			if(type != json_type_string) 
			{
				printf("it is invalid req, CC isn't string, req(%u):%s\n", req.size(), req.c_str());
				err_msg = "it is invalid req, CC isn't string.";
				json_object_put(root);
				return -2;		
			}	
			CC_one = json_object_get_string(_CC_one);
			CC.push_back(CC_one);
		}
		
		if(CC.empty())
		{
			err_msg = "CC is empty.";
			printf("CC is empty.\n");	
		}

	}

	if((cmd == "notification_EQQ" && receiver.empty() && groups.empty()) ||
	   (cmd == "notification_mail" && receiver.empty()))
	{
		printf("it is invalid req, invalid receiver, req(%u):%s\n", req.size(), req.c_str());
		err_msg = "it is invalid req, invalid receiver.";
		json_object_put(root);
		return -1;	

	}

	//释放内存
	json_object_put(root);
	
	return 0;

}




int XProtocol::get_openid_list(const std::string &rsp, std::map<std::string, std::string> &openid_list)
{
	int nRet = 0;
	
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(rsp.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, rsp:%s, err:%s\n", 
			rsp.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	rsp(48633):{"ret":0,"msg":"\u8c03\u7528\u6210\u529f","data":{"timestamp":1418709345,"items":[{"open_id":"3c69c7274ba7cdd5f2de75da147e546a","gender":1,"account":"Owen","realname":"\u603b\u88c1\u529e.\u7f57\u707f","title":null]} }
	*/
	json_object *_ret = NULL;
	if(!json_object_object_get_ex(root, "ret", &_ret))
	{
		printf("it is invalid req, no ret\n");
		json_object_put(root);
		return -1;
	}

	//ret
	nRet = json_object_get_int(_ret);
	if(nRet != 0)
	{
		printf("ret isn't 0.\n");
		json_object_put(root);
		return nRet;
	}

	//data
	json_object *_data = NULL;
	if(!json_object_object_get_ex(root, "data", &_data))
	{
		printf("it is invalid req, no data.\n");
		json_object_put(root);
		return -1;
	}
	
	//items
	json_object *_items = NULL;
	if(!json_object_object_get_ex(_data, "items", &_items))
	{
		json_object_put(root);
		return -1;
	}

	std::string open_id = "";
	std::string account = "";
    int size = json_object_array_length(_items);
    for(int i=0; i<size; i++) 
	{
        json_object *_item = json_object_array_get_idx(_items, i);
		
		//openid
		json_object *_open_id = NULL;
		if(!json_object_object_get_ex(_item, "open_id", &_open_id))
		{
			printf("it is invalid req, no open id\n");
			continue;
		}
		open_id = json_object_get_string(_open_id);

		//account
		json_object *_account = NULL;
		if(!json_object_object_get_ex(_item, "account", &_account))
		{
			printf("it is invalid req, no account\n");
			continue;
		}
		account = json_object_get_string(_account);

		openid_list.insert(std::make_pair(account, open_id)); 
	

    }

	if(openid_list.empty())
	{
		printf("openid_list is empty.\n");
		json_object_put(root);
		return -1;		
	}	

	//释放内存
	json_object_put(root);
	
	return 0;

}



int XProtocol::get_eqq_group_list(const std::string &rsp, std::map<std::string, std::string> &group_list)
{
	int nRet = 0;
	
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(rsp.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, rsp:%s, err:%s\n", 
			rsp.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	rsp(48633):{"ret":0,"msg":"\u8c03\u7528\u6210\u529f","data":{"timestamp":1418709345,"items":[{"open_id":"3c69c7274ba7cdd5f2de75da147e546a","gender":1,"account":"Owen","realname":"\u603b\u88c1\u529e.\u7f57\u707f","title":null]} }
	*/
	json_object *_ret = NULL;
	if(!json_object_object_get_ex(root, "ret", &_ret))
	{
		printf("it is invalid req, no ret\n");
		json_object_put(root);
		return -1;
	}

	//ret
	nRet = json_object_get_int(_ret);
	if(nRet != 0)
	{
		printf("ret isn't 0.\n");
		json_object_put(root);
		return nRet;
	}

	//data
	json_object *_data = NULL;
	if(!json_object_object_get_ex(root, "data", &_data))
	{
		printf("it is invalid req, no data.\n");
		json_object_put(root);
		return -1;
	}
	
	//items
	json_object *_items = NULL;
	if(!json_object_object_get_ex(_data, "items", &_items))
	{
		json_object_put(root);
		return -1;
	}

	std::string dept_id = "";
	std::string dept_name = "";
    int size = json_object_array_length(_items);
    for(int i=0; i<size; i++) 
	{
        json_object *_item = json_object_array_get_idx(_items, i);
		
		//dept_id
		json_object *_dept_id = NULL;
		if(!json_object_object_get_ex(_item, "dept_id", &_dept_id))
		{
			printf("it is invalid req, no dept id\n");
			continue;
		}
		dept_id = json_object_get_string(_dept_id);

		//dept_name
		json_object *_dept_name = NULL;
		if(!json_object_object_get_ex(_item, "dept_name", &_dept_name))
		{
			printf("it is invalid req, no dept name\n");
			continue;
		}
		dept_name = json_object_get_string(_dept_name);
		//printf("dept_name:%s\n", dept_name.c_str());

		group_list.insert(std::make_pair(dept_name, dept_id)); 
	

    }

	if(group_list.empty())
	{
		printf("group_list is empty.\n");
		json_object_put(root);
		return -1;		
	}	

	//释放内存
	json_object_put(root);
	
	return 0;

}



int XProtocol::get_eqq_rsp_status(const std::string &rsp, std::string &err_msg)
{
	int nRet = 0;
	
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(rsp.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, rsp:%s, err:%s\n", 
			rsp.c_str(), json_tokener_error_desc(jerr));
		return -1;
	}

	/*
	{"ret":0,"msg":"\u8c03\u7528\u6210\u529f"}
	*/
	json_object *_ret = NULL;
	if(!json_object_object_get_ex(root, "ret", &_ret))
	{
		printf("it is invalid req, no ret\n");
		json_object_put(root);
		return -1;
	}

	json_object *_msg = NULL;
	if(!json_object_object_get_ex(root, "msg", &_msg))
	{
		printf("it is invalid req, no msg\n");
	}
	else
	{
		err_msg = json_object_get_string(_msg);
	}

	//ret
	nRet = json_object_get_int(_ret);
	
	//释放内存
	json_object_put(root);
	
	return nRet;

}



std::string XProtocol::set_hb(const std::string &msg_id)
{
	char msg[512] = {0};
	snprintf(msg, 512, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"msg_id\":\"%s\"}}\n", 
		CMD_HB.c_str(), getTimestamp(), msg_id.c_str());

	return msg;
}


std::string XProtocol::set_register(const std::string &msg_id, const std::string &worker_id, 
		std::string &group_id, std::string &service_id, std::string &server_id, std::string &cmd)
{
	char msg[512] = {0};
	snprintf(msg, 512, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"msg_id\":\"%s\"}, \"body\":{\"id\":\"%s\", \"group_id\":\"%s\", \"service_id\":\"%s\", \"server_id\":\"%s\", \"cmd\":\"%s\"}}\n", 
		CMD_REGISTER.c_str(), getTimestamp(), msg_id.c_str(), 
		worker_id.c_str(), group_id.c_str(), service_id.c_str(), server_id.c_str(), cmd.c_str());

	return msg;
}



std::string XProtocol::set_unregister(const std::string &msg_id, const std::string &worker_id)
{
	char msg[512] = {0};
	snprintf(msg, 512, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"msg_id\":\"%s\"}, \"body\":{\"id\":\"%s\"}}\n", 
		CMD_UNREGISTER.c_str(), getTimestamp(), msg_id.c_str(), worker_id.c_str());
	
	return msg;
}


std::string XProtocol::set_rsp_msg(const std::string &cmd, const std::string &req_id, const std::string &msg_id,  
	const int err, const std::string &error_info, const std::string &body, bool is_object)
{
	std::string rsp = "";
	
	if(body == "")
	{
		char msg[512] = {0};
		if(req_id == "")
		{
			snprintf(msg, 512, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"msg_id\":\"%s\", \"err\":%d, \"err_info\":\"%s\"}}\n", 
				cmd.c_str(), getTimestamp(), msg_id.c_str(), err, error_info.c_str());		
		}
		else
		{
			snprintf(msg, 512, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"req_id\":\"%s\", \"msg_id\":\"%s\", \"err\":%d, \"err_info\":\"%s\"}}\n", 
				cmd.c_str(), getTimestamp(), req_id.c_str(), msg_id.c_str(), err, error_info.c_str());
		}
		rsp = msg;
	}
	else
	{
		char msg[8092] = {0};
		if(req_id == "")
		{
			if(is_object)
			{
				snprintf(msg, 8092, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"msg_id\":\"%s\", \"err\":%d, \"err_info\":\"%s\"}, \"body\":%s}\n", 
					cmd.c_str(), getTimestamp(), msg_id.c_str(), err, error_info.c_str(), body.c_str());			
			}
			else
			{
				snprintf(msg, 8092, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"msg_id\":\"%s\", \"err\":%d, \"err_info\":\"%s\"}, \"body\":\"%s\"}\n", 
					cmd.c_str(), getTimestamp(), msg_id.c_str(), err, error_info.c_str(), body.c_str());
			}
		}
		else
		{
			if(is_object)
			{
				snprintf(msg, 8092, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"req_id\":\"%s\", \"msg_id\":\"%s\", \"err\":%d, \"err_info\":\"%s\"}, \"body\":%s}\n", 
					cmd.c_str(), getTimestamp(), req_id.c_str(), msg_id.c_str(), err, error_info.c_str(), body.c_str());
			}
			else
			{
				snprintf(msg, 8092, "{\"head\":{\"cmd\":\"%s\", \"time\":%llu, \"req_id\":\"%s\", \"msg_id\":\"%s\", \"err\":%d, \"err_info\":\"%s\"}, \"body\":\"%s\"}\n", 
					cmd.c_str(), getTimestamp(), req_id.c_str(), msg_id.c_str(), err, error_info.c_str(), body.c_str());
			}
		}
		rsp = msg;
		
	}

	return rsp;

	
}




std::string XProtocol::set_new_req(const std::string &req, const std::string &msg_id)
{

	std::string new_req = "";
	
	enum json_tokener_error jerr;
	json_object *root = json_tokener_parse_verbose(req.c_str(), &jerr);
	if(jerr != json_tokener_success)
	{
		printf("json_tokener_parse_verbose failed, req:%s, err:%s\n", 
			req.c_str(), json_tokener_error_desc(jerr));
		return new_req;
	}

	/*
	{"head":{"cmd":"register", "time":111111, "req_id":"xxxxxx", "channel":"bx", "peer":"127.0.0.1:5500", "service_id":"game1", "server_id":"100"}, "body":{"user_id":"xcp"}}
	*/
	//head
	json_object *head = NULL;
	if(!json_object_object_get_ex(root, "head", &head))
	{
		printf("it is invalid req, no head, req(%u):%s\n", req.size(), req.c_str());
		json_object_put(root);
		return new_req;
	}

	json_object_object_add(head, "msg_id", json_object_new_string(msg_id.c_str()));

	new_req = json_object_get_string(root);
	//printf("new_req(%u):%s\n", new_req.size(), new_req.c_str());

	new_req += std::string("\n");

	//释放内存
	json_object_put(root);

	return new_req;

}


