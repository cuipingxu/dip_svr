
#include "req_mgt.h"
#include "base/base_args_parser.h"
#include "base/base_logger.h"
#include "libcomm/conf_mgt.h"

Req_Mgt::Req_Mgt()
{
	_queue = new X_Queue<Request_Ptr>(PSGT_Conf_Mgt->_max_queue_size, PSGT_Conf_Mgt->_rate_limiting);	
}


Req_Mgt::~Req_Mgt()
{


}

int Req_Mgt::push_req(Request_Ptr req)
{
	return _queue->push(req);
}


int Req_Mgt::get_req(Request_Ptr &req)
{
	return _queue->pop(req);	
}

bool Req_Mgt::full()
{
	return _queue->full();
}


unsigned int Req_Mgt::count()
{
	return _queue->size();
}


