
#include "worker_mgt.h"
#include "libcomm/error_code.h"
#include "base/base_logger.h"
#include "logclient/debugger.h"

extern Debugger g_debugger;
extern Logger g_logger_debug;

Workers::Workers(): _group_id(""), _index(0)
{

}


Workers::~Workers()
{

}


void Workers::register_worker(Worker_Ptr worker)
{
	XCP_LOGGER_INFO(&g_logger_debug, "===register worker(%s)\n", worker->to_string().c_str());
	DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "===register worker(%s)", worker->to_string().c_str());

	_workers.push_back(worker);
}



void Workers::unregister_worker(Worker_Ptr worker)
{
	std::vector<Worker_Ptr>::iterator itr = _workers.begin();
	for(; itr != _workers.end(); ++itr)
	{
		if(worker->_fd == (*itr)->_fd)
		{
			XCP_LOGGER_INFO(&g_logger_debug, "===unregister worker(%s)\n", (*itr)->to_string().c_str());
			DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "===unregister worker(%s)", (*itr)->to_string().c_str());

			_workers.erase(itr);

			break;
		}
	}
	
}



bool Workers::get_worker(Worker_Ptr &worker)
{
	int bRet = true;
	
	if(_workers.empty())
	{
		XCP_LOGGER_ERROR(&g_logger_debug, "no worker is found.\n");
		DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "no worker is found");
		return false;
	}

	if(_index >= _workers.size())
	{
		_index = 0;
	}

	//均分
	worker = _workers[_index++];
	
	//worker->log();
	
	return bRet;
	
}




void Workers::set_group_id(const std::string &group_id)
{
	_group_id = group_id;
}



bool Workers::empty()
{
	return _workers.empty();
}



void Workers::log()
{
	XCP_LOGGER_INFO(&g_logger_debug, "--- workers(group id:%s, index:%u) ---\n", _group_id.c_str(), _index);
	std::vector<Worker_Ptr>::iterator itr = _workers.begin();
	for(; itr != _workers.end(); itr++)
	{
		(*itr)->log();
	}
}


//------------------------------------------	

Worker_Mgt::Worker_Mgt(): _index(0)
{

}

Worker_Mgt::~Worker_Mgt()
{

}


int Worker_Mgt::register_worker(Worker_Ptr worker)
{
	int nRet = 0;

	Thread_Mutex_Guard guard(_mutex);
	
	//更新_workers
	std::set<std::string>::iterator itr_worker =  _workers.find(worker->_id);
	if(itr_worker != _workers.end())
	{
		//id 已经存在
		XCP_LOGGER_ERROR(&g_logger_debug, "the worker id is exist(%s)\n", worker->to_string().c_str());
		DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "the worker id is exist(%s)", worker->to_string().c_str());
		return ERR_WORKER_REGISTERED;		
	}
	else
	{
		//id 不存在
		
		//更新_fds
		std::map<int, Worker_Ptr>::iterator itr_fd = _fds.find(worker->_fd);	
		if(itr_fd != _fds.end())
		{
			//fd 已经存在
			XCP_LOGGER_ERROR(&g_logger_debug, "the worker fd is exist(%s)\n", worker->to_string().c_str());
			DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "the worker fd is exist(%s)", worker->to_string().c_str());
			return -2;		
		}
		else
		{
			//fd 不存在
			worker->_index = ++_index;
			_fds.insert(std::make_pair(worker->_fd, worker));
			_workers.insert(worker->_id);
		}
		
	}
	

	//更新_groups
	std::map<std::string, Workers>::iterator itr_group = _groups.find(worker->_group_id);
	if(itr_group != _groups.end())
	{
		//组id 已经存在
		itr_group->second.register_worker(worker);
	}
	else
	{
		//组id 不存在
		Workers workers;
		workers.set_group_id(worker->_group_id);
		workers.register_worker(worker);
		_groups.insert(std::make_pair(worker->_group_id, workers));
	}
	
	return nRet;
	
}




void Worker_Mgt::unregister_worker(Worker_Ptr worker)
{
	Thread_Mutex_Guard guard(_mutex);

	//去注册worker 只需要一个 worker fd 就可以了
	std::map<int, Worker_Ptr>::iterator itr_fd = _fds.find(worker->_fd);	
	if(itr_fd != _fds.end())
	{
		//fd 已经存在
		
		//删除_groups
		std::map<std::string, Workers>::iterator itr_group = _groups.find(itr_fd->second->_group_id);
		if(itr_group != _groups.end())
		{
			//groupid存在
			itr_group->second.unregister_worker(itr_fd->second);
			if(itr_group->second.empty())
			{
				_groups.erase(itr_group);
			}
		}
		else
		{
			//groupid 不存在
			XCP_LOGGER_ERROR(&g_logger_debug, "group id isn't exist, id:%s, fd:%d, group id:%s\n", 
			(itr_fd->second->_id).c_str(), worker->_fd, (itr_fd->second->_group_id).c_str());
			DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "group id isn't exist, id:%s, fd:%d, group id:%s", 
			(itr_fd->second->_id).c_str(), worker->_fd, (itr_fd->second->_group_id).c_str());
		}

		//删除_workers
		std::set<std::string>::iterator itr_worker = _workers.find(itr_fd->second->_id);
		if(itr_worker != _workers.end())
		{
			//worker存在
			_workers.erase(itr_worker);
		}
		else
		{
			//worker 不存在
			XCP_LOGGER_ERROR(&g_logger_debug, "worker isn't exist, id:%s, fd:%d, group id:%s\n", 
			(itr_fd->second->_id).c_str(), worker->_fd, (itr_fd->second->_group_id).c_str());
			DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "worker isn't exist, id:%s, fd:%d, group id:%s", 
			(itr_fd->second->_id).c_str(), worker->_fd, (itr_fd->second->_group_id).c_str());
		}
		
		_fds.erase(worker->_fd);
		
	}
	else
	{
		//fd 不存在
		XCP_LOGGER_ERROR(&g_logger_debug, "fd(%d) isn't exist\n", worker->_fd);
		DEBUGGER_INFO(&g_debugger, "", "", worker->_fd, "fd(%d) isn't exist", worker->_fd);		
	}
	
	
}




bool Worker_Mgt::get_worker(const std::string &group_id, Worker_Ptr &worker)
{
	bool bRet = true;

	Thread_Mutex_Guard guard(_mutex);
	
	std::map<std::string, Workers>::iterator itr_group = _groups.find(group_id);
	if(itr_group != _groups.end())
	{
		//组id 已经存在
		itr_group->second.get_worker(worker);
	}
	else
	{
		return false;
	}
	
	return bRet;
	
}



bool Worker_Mgt::get_worker(int fd, Worker_Ptr &worker)
{
	bool bRet = true;

	Thread_Mutex_Guard guard(_mutex);
	
	std::map<int, Worker_Ptr>::iterator itr = _fds.find(fd);
	if(itr != _fds.end())
	{
		worker = itr->second;
	}
	else
	{
		return false;
	}
	
	return bRet;

}



void Worker_Mgt::log()
{
	std::map<std::string, Workers>::iterator itr = _groups.begin();
	for(; itr != _groups.end(); itr++)
	{
		XCP_LOGGER_INFO(&g_logger_debug, "===== group id:%s =====\n", itr->first.c_str());
		itr->second.log();
	}
}



