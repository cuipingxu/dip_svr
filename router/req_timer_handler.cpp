
#include "req_timer_handler.h"
#include "base/base_logger.h"
#include "req_mgt.h"

extern Logger g_logger_debug;


Req_Timer_handler::Req_Timer_handler()
{

}

Req_Timer_handler::~Req_Timer_handler()
{

}

int Req_Timer_handler::handle_timeout(void *args)
{
	PSGT_Req_Mgt->check_asyn_req();	
	return 0;
}


