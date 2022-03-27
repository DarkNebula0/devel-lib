#pragma once
#include <string>

#include "DatabaseORMGlobalException.h"
#include "DatabaseORMGlobalTypedef.h"
#include "Typedef.h"

#define VTABLE_SIZE	(sizeof(void *))

namespace MySQL
{
	namespace ORM
	{
		struct SDatabaseCondition
		{
			std::string sColumn;
			std::string sValue;
			bool fIsInsecure;
			bool fIsString;

			SDatabaseCondition() = default;
			SDatabaseCondition(SDatabaseCondition &&i_oMove)
			{
				this->sColumn = std::move(i_oMove.sColumn);
				this->sValue = std::move(i_oMove.sValue);
				this->fIsInsecure = i_oMove.fIsInsecure;
				this->fIsString = i_oMove.fIsString;
			}
		};
		enum EOrderBy : byte
		{
			OBDefault,
			OBDesc,
			OBAsc,
		};
	}
}