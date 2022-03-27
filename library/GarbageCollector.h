#pragma once
#include <stack>

#include "LockGuard.h"

template<size_t TSize = 1>
class CGarbageCollector
{
public:
	CGarbageCollector() = default;
	virtual ~CGarbageCollector()
	{
		while (this->m_pStack.empty())
		{
			delete[] static_cast<char *>(this->get(false));
		}
	}

public:
	template<size_t TUseSize = TSize>
	void *allocate() const
	{
		return new char[TUseSize];
	}

	template<size_t TUseSize = TSize>
	void *get(const bool i_fCreateWhenEmpty = true)
	{
		RecursiveLockGuard(this->m_oMutex);
		if (this->m_pStack.empty())
		{
			return (i_fCreateWhenEmpty ? this->allocate<TUseSize>() : nullptr);
		}

		void *pMemory = this->m_pStack.top();
		this->m_pStack.pop();

		return pMemory;
	}

	void free(void *i_pMemory)
	{
		RecursiveLockGuard(this->m_oMutex);
		this->m_pStack.push(i_pMemory);
	}

private:
	CMutex				m_oMutex;
	std::stack<void *>	m_pStack;
};