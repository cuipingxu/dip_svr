

#include "service_adapter.h"
#include "base/base_logger.h"
#include <dlfcn.h>


extern Logger g_logger_debug;

Service_Adapter::Service_Adapter(): _service(NULL)
{

}

Service_Adapter::~Service_Adapter()
{

}

int Service_Adapter::load(const std::string &dll)
{
	int nRet = 0;
	
	void* handle = dlopen(dll.c_str(), RTLD_LAZY | RTLD_GLOBAL);
	if (!handle) 
	{
		printf("dlopen %s failed, err:%s\n", dll.c_str(), dlerror());
		return -1;
	}

	service_func_t func = (service_func_t)dlsym(handle, "get_service_object");
	if (!func) 
	{
		printf("dlsym %s failed, err:%s\n", dll.c_str(), dlerror());
		return -2;
	}

	//get storage obj
	if (func(_service) != 0 || NULL == _service) 
	{
		printf("get service obj failed, err:%s\n", dlerror());
		return -3;
	}

	return nRet;

}


int Service_Adapter::init(service_args* args)
{
	return _service->init(args);
}


int Service_Adapter::process(const std::string& req, std::string& rsp, std::string& debug_info, bool& is_obj)
{
	return _service->process(req, rsp, debug_info, is_obj);
}


int Service_Adapter::release()
{
	return _service->release();
}


