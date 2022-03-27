#pragma once
#include <thread>
#include <atomic>
#include <functional>

#include "SafeQueue.h"
#include "Typedef.h"

typedef std::function<void()> ThreadPoolTaskFn;

class CThreadPool
{
public:
	enum EError {
		ESuccess,
		EAlreadyExecuted,
	};

public:
	CThreadPool()
		: m_nWorkerCount(3), m_fIsExecuted(false)
	{
	}

	CThreadPool(const size_t i_nWorkerCount)
		: CThreadPool()
	{
		this->setWorkerCount(i_nWorkerCount);
	}

	~CThreadPool() {
		this->stop();
	}

	CThreadPool(CThreadPool &i_oPool) = delete;
	CThreadPool(CThreadPool *i_pPool) = delete;

public:
	EError execute();
	EError execute(const size_t i_nWorkerCount) {
		this->setWorkerCount(i_nWorkerCount);
		return this->execute();
	}
	void stop(bool i_fClearTasks = true);

private:
	void handleWorker();

public:
	__forceinline void addTask(const ThreadPoolTaskFn &i_oTask)		{ this->m_aoTasks.enqueue(i_oTask); }
	__forceinline void addTask(ThreadPoolTaskFn &&i_oTask)			{ this->m_aoTasks.enqueue(std::move(i_oTask)); }
	__forceinline void setWorkerCount(const size_t i_nWorkerCount)	{ this->m_nWorkerCount = (i_nWorkerCount == 0 ? 1 : i_nWorkerCount); }

public:
	size_t workerCount() const	{ return this->m_nWorkerCount; }
	bool isExecuted() const		{ return this->m_fIsExecuted; }

private:
	size_t								m_nWorkerCount;
	std::vector<std::thread>			m_aoWorker;
	CSafeQueue<ThreadPoolTaskFn>		m_aoTasks;

	bool								m_fIsExecuted;
};