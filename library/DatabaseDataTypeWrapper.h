#pragma once
#include "Singleton.h"
#include <string>
#include <map>

#include "DatabaseMetaData.h"


namespace MySQL
{
	namespace ORM
	{
		class CColumnDataTypeWrapper : public CSingleton<CColumnDataTypeWrapper>
		{
			friend class CSingleton<CColumnDataTypeWrapper>;
		private:
			CColumnDataTypeWrapper();

		public:
			std::shared_ptr<SDataTypeWrapper> dataTypeWrapper(const std::string &i_sType)
			{
				const auto oIt = this->m_mapDataTypeWrappers.find(i_sType);
				if (oIt != this->m_mapDataTypeWrappers.end())
				{
					return oIt->second;
				}

				return nullptr;
			}

		private:
			std::map<std::string, std::shared_ptr<SDataTypeWrapper>>	m_mapDataTypeWrappers;
		};

	}
}

extern MySQL::ORM::CColumnDataTypeWrapper *ColumnDataTypeWrapperInstance;