

#include "conn.h"
#include "base/base_string.h"
#include "base/base_time.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"
#include "base/base_uid.h"
#include "req_event_handler.h"
#include "libcomm/conf_mgt.h"
#include "libcomm/protocol.h"
#include "logclient/debugger.h"


extern Logger g_logger_debug;
extern Debugger g_debugger;
extern std::string g_id;

XConn::XConn(const std::string ip, unsigned short port): _ip(ip), _port(port), 
	_stmp_create(0), _stmp_close(0), _stmp_hb(0), _client(NULL), _registered(false)
{
	Req_Event_Handler *handler = new Req_Event_Handler;
	_client = new TCP_Client_Epoll(handler, true);
	//这样作就是防止对端关闭连接后，_client 线程被自动释放了，别的线程再对_client 操作导致crash
	_client->inc_ref();
}


XConn::~XConn()
{
	XCP_LOGGER_INFO(&g_logger_debug, "---- prepare to release conn ----\n");

	close();

	while(Thread::ST_EXIT != _client->thr_status())
	{	
		//异步的TCP Client 一定要保证线程退出后才能释放对象。
		sleep(1000000, true); //停顿100毫秒
	}

	//释放TCP Client 对象
	DELETE_POINTER(_client);

	XCP_LOGGER_INFO(&g_logger_debug, "---- complete to release conn ----\n");

}


int XConn::connect()
{
	int nRet = 0;

	if(!is_close())
	{
		return 0;
	}

	_registered = false;

	nRet = _client->open(_ip, _port);
	if(nRet == 0)
	{
		_stmp_create = getTimestamp();
		XCP_LOGGER_INFO(&g_logger_debug, "connect to dip router(%s:%u) success\n", _ip.c_str(), _port);
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "connect to dip router(%s:%u) success", _ip.c_str(), _port);
	}
	else
	{
		XCP_LOGGER_INFO(&g_logger_debug, "connect to dip router(%s:%u) failed, ret:%d\n", _ip.c_str(), _port, nRet);
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "connect to dip router(%s:%u) failed, ret:%d", _ip.c_str(), _port, nRet);
	}
		
	return nRet;
	
}


void XConn::close()
{
	_client->close();
	_registered = false;
}


bool XConn::is_close()
{
	return _client->is_close();
}


void XConn::log()
{
	XCP_LOGGER_INFO(&g_logger_debug, "conn: ip=%s, port=%u, stmp_create=%llu, stmp_close:%llu\n", 
		_ip.c_str(), _port, _stmp_create, _stmp_close);
}


std::string XConn::to_string()
{
	return format("conn: ip=%s, port=%u, stmp_create=%llu, stmp_close:%llu", 
		_ip.c_str(), _port, _stmp_create, _stmp_close);
}


int XConn::register_worker()
{
	int nRet = 0;

	std::string msg = XProtocol::set_register(UID::uid_inc(g_id), PSGT_Conf_Mgt->_id, PSGT_Conf_Mgt->_group_id, 
		PSGT_Conf_Mgt->_service_id, PSGT_Conf_Mgt->_server_id, PSGT_Conf_Mgt->_cmd);

	unsigned int len = msg.size();
	nRet = _client->send_msg(msg.c_str(), len);
	if(nRet != 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "send register to dip router(%s) failed, ret:%d\n", to_string().c_str(), nRet);
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "send register to dip router(%s) failed, ret:%d", to_string().c_str(), nRet);
	}
	else
	{
		XCP_LOGGER_INFO(&g_logger_debug, "send register to dip router(%s) success.\n", to_string().c_str());	
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "send register to dip router(%s) success.", to_string().c_str());
	}

	return nRet;
}



int XConn::unregister_worker()
{
	int nRet = 0;

	std::string msg = XProtocol::set_unregister(UID::uid_inc(g_id), PSGT_Conf_Mgt->_id);
	unsigned int len = msg.size();
	nRet = _client->send_msg(msg.c_str(), len);
	if(nRet != 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "send unregister to dip router(%s) failed, ret:%d\n", to_string().c_str(), nRet);
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "send unregister to dip router(%s) failed, ret:%d", to_string().c_str(), nRet);
	}
	else
	{ 
		XCP_LOGGER_INFO(&g_logger_debug, "send unregister to dip router(%s) success.\n", to_string().c_str()); 
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "send unregister to dip router(%s) success.", to_string().c_str());
	}

	return nRet;
}



int XConn::hb()
{
	int nRet = 0;

	std::string msg = XProtocol::set_hb(UID::uid_inc(g_id));
	unsigned int len = msg.size();
	nRet = _client->send_msg(msg.c_str(), len);
	if(nRet != 0)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "send hb to dip router(%s) failed, ret:%d\n", to_string().c_str(), nRet);
		DEBUGGER_INFO(&g_debugger, "", "", _client->fd(), "send hb to dip router(%s) failed, ret:%d", to_string().c_str(), nRet);
	}
	else
	{
		//XCP_LOGGER_INFO(&g_logger_debug, "send hb to dip router success.\n");
	}
	
	return nRet;
}


