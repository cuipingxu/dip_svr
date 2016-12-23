#include "base/base_common.h"
#include "base/base_args_parser.h"
#include "base/base_reactor.h"
#include "base/base_simple_udp_reactor.h"
#include "base/base_signal.h"
#include "base/base_logger.h"
#include "base/base_time.h"
#include "base/base_os.h"

#include "libcomm/conf_mgt.h"

#include "admin.h"
#include "processor.h"
#include "req_event_handler.h"
#include "conn_mgt.h"
#include "service_adapter.h"

#include "logclient/debugger.h"

USING_NS_BASE;

Args_Parser g_args_parser;          //进程参数解析器
Logger g_logger_debug;              //内部日志打印器
Debugger g_debugger;
Service_Adapter service_adapter;
std::string g_id = "DIP_Worker";    //id: Log_Router_IP_seq


void usage()
{
    fprintf(stderr, 
        "Usage: ./dip_worker.bin -s cgf\n\n"
        "    -s    cfg path\n\n"
        "  build time: %s %s\n\n"
        "  Bug reports, feedback, etc, to: 89617663@qq.com\n"
        "\n",
        __DATE__, __TIME__
    );
};



int main(int argc, char * argv[])
{
	int nRet = 0;

	//usage
    if (argc == 1) 
	{
        usage();
        return 0;
    }
	
	//信号屏蔽
	sigset_t sigset;
	add_signal_in_set(sigset, 3, SIGPIPE, SIGUSR2, SIGTERM);
	nRet = block_process_signal(sigset);
	if(nRet != 0)
	{
		printf("block_process_signal failed, ret:%d\n", nRet);
		return 0;
	}

	//解析程序运行参数
	printf("--- prepare to start parse arg ---\n");
	g_args_parser.parse_args(argc, argv);
	g_args_parser.show();

	std::string cfg = "";
	if(!g_args_parser.get_opt("s", &cfg))
	{
		printf("get_opt cfg failed!\n");
		return 0;
	}	
	printf("=== complete to start parse arg ===\n");


	//初始化配置模块
	printf("--- prepare to init conf mgt ---\n");
	nRet = PSGT_Conf_Mgt->init_worker(std::string("../conf/")+cfg);
	if(nRet != 0)
	{
		printf("init conf mgt failed, ret:%d\n", nRet);
		return 0;
	}
	g_id = PSGT_Conf_Mgt->_id;
	printf("===== complete to init conf mgt =====\n");


	//初始化logger_debug
	nRet = g_logger_debug.init("./../log/", g_id, MAX_LOG_SIZE, 3600);
	if(nRet != 0)
	{
		printf("init debug logger failed, ret:%d\n", nRet);
		return nRet;
	}

	//初始化debugger
	std::vector<StServer> logsys = PSGT_Conf_Mgt->get_logsys();
	if(!logsys.empty())
	{
		nRet = g_debugger.init(logsys, PSGT_Conf_Mgt->_category, PSGT_Conf_Mgt->_regional, 
			g_id, "127.0.0.1", PSGT_Conf_Mgt->_req_port);
		if(nRet != 0)
		{
			printf("init debugger failed, ret:%d\n", nRet);
			return nRet;
		}	
	}
	

	XCP_LOGGER_INFO(&g_logger_debug, "=============================================\n");
	XCP_LOGGER_INFO(&g_logger_debug, "        date:%s\n", FormatDateTimeStr().c_str());
	XCP_LOGGER_INFO(&g_logger_debug, "        process:%d, thread:%llu \n", get_pid(), get_thread_id());
	XCP_LOGGER_INFO(&g_logger_debug, "        prepare to start dip worker! ...\n");
	XCP_LOGGER_INFO(&g_logger_debug, "=============================================\n");


	//初始化Service obj
	XCP_LOGGER_INFO(&g_logger_debug, "--- prepare to load service so ---\n");
	std::string plug_in = PSGT_Conf_Mgt->_plug_in;
	std::string dll = std::string("../libservice/")+plug_in;
	nRet = service_adapter.load(dll);
	if(nRet != 0)
	{
		printf("load service so(%s) failed, ret:%d\n", dll.c_str(), nRet);
		return 0;
	}

	service_args args;
	args.logger_debug = &g_logger_debug;
	args.conf_mgt = PSGT_Conf_Mgt;
	args.argc_ext = 0;
	nRet = service_adapter.init(&args);
	if(nRet != 0)
	{
		printf("init service so(%s) failed, ret:%d\n", dll.c_str(), nRet);
		return 0;
	}
	XCP_LOGGER_INFO(&g_logger_debug, "--- complete to load service so ---\n");


	//启动工作线程池
	XCP_LOGGER_INFO(&g_logger_debug, "--- prepare to start processor ---\n");
	Processor processor;
	unsigned short thr_num = PSGT_Conf_Mgt->_thr_num;	
	nRet = processor.init(NULL, thr_num);
	if(nRet != 0)
	{
		printf("processor init failed, ret:%d\n", nRet);
		return nRet;
	}
		
	nRet = processor.run();
	if(nRet != 0)
	{
		printf("processor run failed, ret:%d\n", nRet);
		return nRet;
	}
	XCP_LOGGER_INFO(&g_logger_debug, "=== complete to start processor ===\n");


	//启动Admin
	XCP_LOGGER_INFO(&g_logger_debug, "--- prepare to start Admin ---\n");
	Admin admin;
	nRet = admin.init(NULL);
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "Admin init failed, ret:%d\n", nRet);
		return nRet;
	}
		
	nRet = admin.run();
	if(nRet != 0)
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "Admin run failed, ret:%d\n", nRet);
		return nRet;
	}
	XCP_LOGGER_INFO(&g_logger_debug, "=== complete to start Admin ===\n");


	//启动TCP 连接管理器
	XCP_LOGGER_INFO(&g_logger_debug, "--- prepare to init conn mgt ---\n");
	nRet = PSGT_XConn_Mgt->init();
	if(nRet != 0)
	{
		printf("init conn mgt failed, ret:%d\n", nRet);
		return 0;
	}
	XCP_LOGGER_INFO(&g_logger_debug, "===== complete to init conn mgt =====\n");

	XCP_LOGGER_INFO(&g_logger_debug, "===== complete to start dip worker! =====\n");
		
	while(true)
	{
		::sleep(5);
	}
	
	return 0;
	
}


