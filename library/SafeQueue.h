#pragma once
#include <queue>

#include "LockGuard.h"
#include "Typedef.h"
#include "Global.h"

#define SafeQueueLockGuard(x)	RecursiveLockGuard(x.mutex())
#define SafeQueueLock(x)		x.mutex().lock()
#define SafeQueueUnlock(x)		x.mutex().unlock()

template <class T>
class CSafeQueue
{
public:
	const CMutex &mutex() const
	{
		return this->m_oMutex;
	}

	size_t size() const
	{
		return this->m_aQueue.size();
	}

	bool isEmpty() const
	{
		return this->m_aQueue.empty();
	}

public:
	void enqueue(const T &i_tValue)
	{
		RecursiveLockGuard(this->m_oMutex);
		this->m_aQueue.push(i_tValue);
	}

	void enqueue(T &&i_tValue)
	{
		RecursiveLockGuard(this->m_oMutex);
		this->m_aQueue.push(std::move(i_tValue));
	}

public:
	T dequeue(const bool i_fMove = true)
	{
		RecursiveLockGuard(this->m_oMutex);
		
		T tValue = this->front(i_fMove);
		this->pop();

		return tValue;
	}

public:
	T front(const bool i_fMove = false)
	{
		RecursiveLockGuard(this->m_oMutex);

		if (this->isEmpty())
		{
			throw CNoEntryFoundException();
		}

		return (i_fMove ? std::move(this->m_aQueue.front()) : this->m_aQueue.front());
	}

	void pop()
	{
		RecursiveLockGuard(this->m_oMutex);
		
		if (this->isEmpty())
		{
			throw CNoEntryFoundException();
		}

		this->m_aQueue.pop();
	}

	void clear()
	{
		RecursiveLockGuard(this->m_oMutex);

		try {
			while (true)
			{
				this->pop();
			}
		} catch(const CNoEntryFoundException &)
		{ }
	}

private:
	std::queue<T>	m_aQueue;
	CMutex			m_oMutex;
};