#pragma once

#include "MutexVector.h"
#include "JsonObject.h"

namespace Json
{
	class CArray : public CMutexVector<CObject>
	{
	public:
		CArray() = default;
		CArray(std::vector<CObject> &i_oOther) { CMutexVector::operator=(i_oOther); }
		CArray(CMutexVector<CObject> &i_oOther) { CMutexVector::operator=(i_oOther); }
		CArray(const CArray &i_oOther) { this->operator=(i_oOther); }
		CArray(CArray &&i_oOther) noexcept { this->operator=(std::move(i_oOther)); }
		virtual ~CArray() = default;
	public:
		void allocate(const size_t i_nSize) { CMutexVector::reserve(i_nSize); }
	public:
		_inline const CMutexVector<CObject> &data() const { return *this; }
	public:
		_inline CArray &operator=(const CArray &i_oOther)
		{
			CMutexVector::operator=(i_oOther);
			return *this;
		}

		_inline CArray &operator=(CArray &&i_oOther) noexcept
		{
			CMutexVector::operator=(std::move(i_oOther));
			return *this;
		}
	};
}