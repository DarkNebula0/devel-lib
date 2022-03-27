#pragma once
#include "Mutex.h"

class CLockGuard
{
public:
	CLockGuard(const CMutex &i_oMutex)
		: m_oMutex(i_oMutex)
	{
		this->m_oMutex.lock();
	}
	
	~CLockGuard()
	{
		this->m_oMutex.unlock();
	}

private:
	const CMutex &m_oMutex;
};