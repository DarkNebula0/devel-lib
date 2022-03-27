#pragma once
#include <type_traits>

namespace MySQL
{
	namespace ORM
	{
		class CModel;
		template <typename TModel = CModel>
		class CQueryBuilder;
		template <typename TModel = CModel, typename std::enable_if_t<std::is_base_of_v<CModel, TModel>> * = nullptr>
		class CTable;
		class CORM;

		namespace Serialize { }
	}
}

namespace MySQLORM = MySQL::ORM;
namespace MySQLORMSerialize = MySQLORM::Serialize;