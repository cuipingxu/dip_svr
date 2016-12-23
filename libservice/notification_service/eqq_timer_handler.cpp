
#include "eqq_timer_handler.h"
#include "eqq_mgt.h"

EQQ_Timer_handler::EQQ_Timer_handler()
{

}

EQQ_Timer_handler::~EQQ_Timer_handler()
{

}

int EQQ_Timer_handler::handle_timeout(void *args)
{
	PSGT_EQQ_Mgt->update_openid_list();
	PSGT_EQQ_Mgt->update_eqq_group_list();
	
	return 0;
}

