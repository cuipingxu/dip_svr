
#include "conn.h"
#include "conn_mgt.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"
#include "base/base_string.h"
#include "libcomm/error_code.h"
#include "libcomm/conf_mgt.h"
#include "conn_timer_handler.h"
#include "logclient/debugger.h"

extern Args_Parser g_args_parser;
extern Logger g_logger_debug;
extern Debugger g_debugger;


XConn_Mgt::XConn_Mgt(): timer_req(NULL)
{

}

XConn_Mgt::~XConn_Mgt()
{

}


//初始化长连接池
int XConn_Mgt::init()
{
	int nRet = 0;
	
	refresh();
	
	//启动conn mgt  timer
	XCP_LOGGER_INFO(&g_logger_debug, "--- prepare to start conn mgt timer ---\n");
	timer_req = new Select_Timer;
	XConn_Timer_handler *conn_thandler = new XConn_Timer_handler;
	nRet = timer_req->register_timer_handler(conn_thandler, 5000000);
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "register conn mgt timer handler falied, ret:%d\n", nRet);
		return nRet;
	}
		
	nRet = timer_req->init();
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "int conn mgt timer failed, ret:%d\n", nRet);
		return nRet;
	}

	nRet = timer_req->run();
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "conn mgt timer run failed, ret:%d\n", nRet);
		return nRet;
	}
	XCP_LOGGER_INFO(&g_logger_debug, "=== complete to start conn mgt timer ===\n");

	return nRet;

	
}




void XConn_Mgt::refresh()
{
	Thread_Mutex_Guard guard(_mutex);

	std::vector<StServer> servers;
	servers = PSGT_Conf_Mgt->get_routers();

	printf("----- XConn_Mgt::refresh, size of servers:%u -----\n", servers.size());
	

	//释放无效链接
	std::map<std::string, XConn_Ptr>::iterator itr = _conns.begin();
	for(; itr != _conns.end();)
	{
		bool found = false;
		for(unsigned int i=0; i<servers.size(); i++)
		{
			if((servers[i]._ip == itr->second->_ip) &&
			   (servers[i]._port == itr->second->_port))
			{
				found = true;
				break;
			}
		}

		//对于无效的conn关闭和释放
		if(!found)
		{
			std::string router_id = format("%s_%u", itr->second->_ip.c_str(), itr->second->_port);
			XCP_LOGGER_INFO(&g_logger_debug, "**** release useless conn(%s) ****\n", router_id.c_str());
			
			itr->second->close();
			_conns.erase(itr++);  //调用Conn析构函数
		}
		else
		{
			++itr;
		}

	}


	//创建新链接
	std::string router_id = "";
	for(unsigned int i=0; i<servers.size(); i++)
	{
		router_id = format("%s_%u", servers[i]._ip.c_str(), servers[i]._port);
		std::map<std::string, XConn_Ptr>::iterator itr = _conns.find(router_id);
		if(itr == _conns.end())
		{
			XCP_LOGGER_INFO(&g_logger_debug, "**** add new conn(%s) ****\n", router_id.c_str());
			
			XConn_Ptr conn = new XConn(servers[i]._ip, servers[i]._port);
			conn->connect();
			_conns.insert(std::make_pair(router_id, conn));

		}		
	}

}



//通过hb 检查所有长连接
void XConn_Mgt::check()
{
	int nRet = 0;
	
	Thread_Mutex_Guard guard(_mutex);
	
	std::map<std::string, XConn_Ptr>::iterator itr = _conns.begin();
	for(; itr != _conns.end(); itr++)
	{
		if(itr->second->is_close())
		{
			//连接
			itr->second->connect();		
		}
		else
		{
			//如果已经注册就发hb
			if(itr->second->_registered)
			{
				//心跳
				nRet = itr->second->hb();
				if(nRet != 0)
				{
					itr->second->close();
				}			
			}
			else
			{
				//注册worker
				nRet = itr->second->register_worker();
				if(nRet != 0)
				{
					//注册失败关闭连接
					itr->second->close();
				}

			}
			
		}
		
	}

}



void XConn_Mgt::stop_check()
{
	timer_req->stop();
}


bool XConn_Mgt::get_conn(const std::string &router_id, XConn_Ptr &conn)
{
	Thread_Mutex_Guard guard(_mutex);
	
	std::map<std::string, XConn_Ptr>::iterator itr = _conns.find(router_id);
	if(itr == _conns.end())
	{
		XCP_LOGGER_INFO(&g_logger_debug, "no conn is found, router id:%s\n", router_id.c_str());
		DEBUGGER_INFO(&g_debugger, "", "", -1, "no conn is found, router id:%s", router_id.c_str());
		return false;
	}

	conn = itr->second;
	
	return true;
}



void XConn_Mgt::unregister_workers()
{
	Thread_Mutex_Guard guard(_mutex);
	
	std::map<std::string, XConn_Ptr>::iterator itr = _conns.begin();
	for(; itr != _conns.end(); itr++)
	{
		itr->second->unregister_worker();
	}
	
}

