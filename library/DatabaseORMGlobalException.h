#pragma once
#include "Global.h"

namespace MySQL
{
	namespace ORM
	{
		CreateException(CNoDatabasePassedException, "Set the database before initialize the ORM!");
		CreateException(CORMIsAlreadyInitializedException, "You can't change the database when the ORM is already initialized!");
		CreateException(CModelHasWrongTableExcpetion, "Model has the wrong table!");
		CreateException(CDataTypeNotFoundException, "Couldn't find DataType!");
		CreateException(CColumnOutOfRangeException, "Column index is out of range!");
		CreateException(CColumnNotFoundException, "Column not found!");
		CreateException(CNotEnoughDataForBindAll, "Not enough data available for bind every column!");
		CreateException(CIdNotFoundException, "ID in Model not found!");
		CreateException(CCouldNotPrepareQueryException, "Couldn't prepare query!");
		CreateException(CCouldNotExecuteQueryException, "Couldn't execute query!");
		CreateException(CCachedTableException, "Use in your table class the \"this->m_apModels\" vector!");
	}
}