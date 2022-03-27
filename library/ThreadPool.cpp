#include "ThreadPool.h"
#include "MutexVector.h"
#include <Windows.h>


CThreadPool::EError CThreadPool::execute()
{
	if (!this->m_fIsExecuted) {
		this->m_aoWorker.reserve(this->m_nWorkerCount);
		this->m_fIsExecuted = true;

		for (size_t i = 0; i < this->m_nWorkerCount; i++) {
			this->m_aoWorker.push_back(std::thread(&CThreadPool::handleWorker, this));
		}
		
		return CThreadPool::EError::ESuccess;
	}

	return CThreadPool::EError::EAlreadyExecuted;
}

void CThreadPool::stop(bool i_fClearTasks)
{
	if (this->m_fIsExecuted) {
		this->m_fIsExecuted = false;

		for (std::thread &oWorker : this->m_aoWorker) {
			if (oWorker.joinable()) {
				oWorker.join();
			}
		}

		this->m_aoWorker.clear();
	}
}

void CThreadPool::handleWorker()	
{
	while (this->m_fIsExecuted) {
		
		if (this->m_aoTasks.isEmpty())
		{
			THREAD_SLEEP();
			continue;
		}

		ThreadPoolTaskFn fnTask = nullptr;

		try
		{
			fnTask = this->m_aoTasks.dequeue();
		} catch (const CNoEntryFoundException &)
		{ }

		if (fnTask)
		{
			fnTask();
		}
	}
}