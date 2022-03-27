#pragma once
#include <string>
#include <map>

#include "LockGuard.h"
#include "Typedef.h"
#include "Global.h"

#define LockStringMatrix(x)	RecursiveLockGuard(x.mutex())

template<typename T>
class CStringMatrix {
public:
	CStringMatrix() {
		this->m_oStringMatrix.oData = T();
	}

public:
	struct SData {
		std::map<byte, SData *>		mapNext;
		T							oData;
	};

public:
	bool addString(const std::string &i_sString, const T &i_tData, bool i_fOverwrite = true)
	{
		RecursiveLockGuard(this->m_oMutex);
		return _addName(i_sString, i_tData, 0, nullptr, i_fOverwrite);
	}

	bool removeString(const std::string &i_sString)
	{
		RecursiveLockGuard(this->m_oMutex);
		return _removeString(i_sString);
	}

	// For pointer
	template<typename T1 = T>
	typename std::enable_if_t<std::is_pointer_v<T1>, T> findString(const std::string &i_sString) {
		RecursiveLockGuard(this->m_oMutex);

		T *pResult = _findString(i_sString);

		return (pResult ? *pResult : nullptr);
	}

	// For no-pointer
	// When no entry was found it throws CNoEntryFoundException
	template<typename T1 = T> 
	typename std::enable_if_t<!std::is_pointer_v<T1>, T> &findString(const std::string &i_sString) {
		RecursiveLockGuard(this->m_oMutex);

		T *pResult = _findString(i_sString);
		if (!pResult) {
			throw CNoEntryFoundException();
		}
		
		return *pResult;
	}

	const CMutex &mutex() const
	{
		return this->m_oMutex;
	}

private:
	bool _addName(const std::string &i_sString, const T &i_tData, size_t i_nPos = 0, SData *i_pNext = nullptr, const bool i_fOverwrite = true) {
		if (i_sString.size() == 0 || i_nPos >= i_sString.size()) return false;
		if (i_pNext == nullptr) i_pNext = &m_oStringMatrix;

		i_pNext = setData(i_pNext, i_sString[i_nPos]);

		if (i_nPos + 1 == i_sString.size()) {
			if (i_pNext->oData != T() && !i_fOverwrite)
				return false;

			i_pNext->oData = i_tData;
			return true;
		}
		else
			return _addName(i_sString, i_tData, i_nPos + 1, i_pNext, i_fOverwrite);
	}
	bool _removeString(const std::string &i_sString, size_t i_nPos = 0, SData *i_pNext = nullptr) {
		if (i_sString.size() == 0 || i_nPos >= i_sString.size()) return false;
		if (i_pNext == nullptr) i_pNext = &this->m_oStringMatrix;

		auto oData = i_pNext->mapNext.find(i_sString[i_nPos]);
		if (oData == i_pNext->mapNext.end())
			return false;
		if (i_nPos + 1 == i_sString.size()) {
			oData->second->oData = T();
			return true;
		}

		return _removeString(i_sString, i_nPos + 1, oData->second);
	}
	T *_findString(const std::string &i_sString, const size_t i_nPos = 0, SData *i_pNext = nullptr) {
		if (i_sString.size() == 0 || i_nPos >= i_sString.size()) return nullptr;
		if (i_pNext == nullptr) i_pNext = &this->m_oStringMatrix;

		auto oData = i_pNext->mapNext.find(i_sString[i_nPos]);
		if (oData == i_pNext->mapNext.end())
			return nullptr;
		if (i_nPos + 1 == i_sString.size())
			return &oData->second->oData;

		return _findString(i_sString, i_nPos + 1, oData->second);
	}

private:
	SData *setData(SData *i_pData, byte i_bChar) {
		auto oData = i_pData->mapNext.find(i_bChar);
		SData *pData;

		if (oData == i_pData->mapNext.end()) {
			pData = new SData();
			i_pData->mapNext.insert(std::pair<byte, SData *>(i_bChar, pData));
		}
		else
			pData = oData->second;

		return pData;
	}

private:
	CMutex	m_oMutex;
	SData	m_oStringMatrix;
};