
#include "admin.h"
#include "libcomm/error_code.h"
#include "libcomm/common.h"
#include "libcomm/conf_mgt.h"
#include "base/base_signal.h"
#include "base/base_logger.h"
#include "logclient/debugger.h"
#include "conn_mgt.h"
#include "req_mgt.h"

extern Logger g_logger_debug;
extern Debugger g_debugger;
extern std::string g_id;
	
Admin::Admin()
{

}


Admin::~Admin()
{

}


int Admin::do_init(void *args)
{
	int nRet = 0;

	return nRet;
}


int Admin::svc()
{
	int nRet = 0;

	sigset_t sigset;
	add_signal_in_set(sigset, 2, SIGUSR2, SIGTERM);

	int signo = 0;	
	while(true) 
	{
		nRet = sigwait(&sigset, &signo);
		if (EINTR == nRet) 
		{
		    continue;
		}
		else if (nRet != 0)
		{
			XCP_LOGGER_INFO(&g_logger_debug, "sigwait failed. errno:%d, errmsg:%s\n", errno, strerror(errno));
		}
		else
		{
			if(signo == SIGUSR2)
			{
				XCP_LOGGER_INFO(&g_logger_debug, "--- prepare to Admin refresh ----\n");
					
				//ˢ��ҵ��Э��
				nRet = PSGT_Conf_Mgt->refresh_service_protocol();
				if(nRet != 0)
				{
					XCP_LOGGER_INFO(&g_logger_debug, "refresh_service_protocol failed, ret:%d\n", nRet);
				}
				else
				{
					XCP_LOGGER_INFO(&g_logger_debug, "refresh_service_protocol success\n");
				}

				//ˢ��routers
				nRet = PSGT_Conf_Mgt->refresh_routers();
				if(nRet != 0)
				{
					XCP_LOGGER_INFO(&g_logger_debug, "refresh_routers failed, ret:%d\n", nRet);
				}
				else
				{
					XCP_LOGGER_INFO(&g_logger_debug, "refresh_routers success\n");
				}

				//ˢ��logsys
				nRet = PSGT_Conf_Mgt->refresh_worker_logsys();
				if(nRet != 0)
				{
					XCP_LOGGER_INFO(&g_logger_debug, "refresh_worker_logsys failed, ret:%d\n", nRet);
				}
				else
				{
					XCP_LOGGER_INFO(&g_logger_debug, "refresh_worker_logsys success\n");
				}			

				//ˢ��debugger
				std::vector<StServer> logsys = PSGT_Conf_Mgt->get_logsys();
				if(!logsys.empty())
				{
					nRet = g_debugger.refresh(PSGT_Conf_Mgt->_logsys, PSGT_Conf_Mgt->_category, PSGT_Conf_Mgt->_regional, 
						g_id, "127.0.0.1", PSGT_Conf_Mgt->_req_port);
					if(nRet != 0)
					{
						XCP_LOGGER_INFO(&g_logger_debug, "refresh debugger failed, ret:%d\n", nRet);
					}
					else
					{
						XCP_LOGGER_INFO(&g_logger_debug, "refresh debugger success\n");
					}
					
				}

				XCP_LOGGER_INFO(&g_logger_debug, "--- complete to Admin refresh ----\n");
				
			}
			else if(signo == SIGTERM)
			{
				XCP_LOGGER_INFO(&g_logger_debug, "--- start to terminate router ----\n");

				//ֹͣconn mgt �Ķ�ʱcheck����ֹȥע�����������ע����
				PSGT_XConn_Mgt->stop_check();

				::sleep(3);
				
				//ȥע������worker
				PSGT_XConn_Mgt->unregister_workers();

				unsigned int loop = 0;
				while((loop++) < 10)
				{
					if(PSGT_Req_Mgt->count() == 0)
					{
						//����������û�������ٵ�5�����˳�����
						XCP_LOGGER_INFO(&g_logger_debug, "req queue is empty. exit now ...\n");
						::sleep(5);  //�ٵ�5�뽫����������󷵻ظ�Router
						::exit(0);
					}
					else
					{
						::sleep(1);
					}
				}

				//ѭ��10�Σ������Ϣ����û�д������ǿ���˳�����
				XCP_LOGGER_INFO(&g_logger_debug, "force to exit now ...\n");
				::sleep(3);   //��3�뽫��־���
				::exit(0);
				

			}
			else
			{

			}
	
		}

		break;
	
	}
	
	return 0;

	
}


