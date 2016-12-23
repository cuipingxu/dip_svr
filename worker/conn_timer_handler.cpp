
#include "conn_timer_handler.h"
#include "base/base_logger.h"
#include "conn_mgt.h"

XConn_Timer_handler::XConn_Timer_handler()
{

}

XConn_Timer_handler::~XConn_Timer_handler()
{

}

int XConn_Timer_handler::handle_timeout(void *args)
{
	PSGT_XConn_Mgt->check();
	
	return 0;
}

