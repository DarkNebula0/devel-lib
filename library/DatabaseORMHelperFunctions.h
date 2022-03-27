#pragma once
#include "MySQLPreparedBind.h"
#include "MySQLQuery.h"

#include "DatabaseSerializable.h"

namespace MySQL
{
	namespace ORM
	{
		namespace Serialize
		{
			template <typename TModel>
			bool DeserializeQuery(TModel *i_tModel, CMySQLQuery &i_oQuery)
			{
				::Serialize::IterateSerializableStruct<IData<>>(i_tModel, [&](IData<> *i_pData) -> bool
				{
					if (i_pData->parseFromDatabase())
					{
						char *szValue = i_oQuery.toRaw(i_pData->columnIndex());
						if (szValue)
						{
							i_pData->doDeserialize(szValue);
						}
						else
						{
							throw CColumnOutOfRangeException();
						}
					}

					return true;
				});

				return true;
			}

			template <typename TModel>
			MySQL::SPreparedBind *BindQuery(TModel *i_tModel, const size_t i_nColumns, const MySQL::EPreparedBind i_eBindType, const size_t i_nIgnoreColumnIndex = ~0)
			{
				if (i_nColumns > 0)
				{
					MakeMySQLBind_Pointer(pResults, i_nColumns);

					size_t nIndex = 0;
					::Serialize::IterateSerializableStruct<IData<>>(i_tModel, [&](IData<> *i_pData) -> bool
					{
						if (nIndex >= i_nColumns)
						{
							return false;
						}
						else if (i_pData->parseFromDatabase())
						{
							if (i_nIgnoreColumnIndex != nIndex)
							{
								i_pData->bind(pResults, nIndex - (nIndex > i_nIgnoreColumnIndex), i_eBindType);
							}

							nIndex++;
						}

						return true;
					});

					if (nIndex != i_nColumns)
					{
						throw CNotEnoughDataForBindAll();
					}

					return pResults;
				}

				return nullptr;
			}

			template <typename TModel>
			MySQL::SPreparedBind *BindQueryResults(TModel *i_tModel, const size_t i_nColumns, const size_t i_nIgnoreColumnIndex = ~0)
			{
				return BindQuery<TModel>(i_tModel, i_nColumns, MySQL::EPreparedBind::PBResult, i_nIgnoreColumnIndex);
			}

			template <typename TModel>
			MySQL::SPreparedBind *BindQueryParams(TModel *i_tModel, const size_t i_nColumns, const size_t i_nIgnoreColumnIndex = ~0)
			{
				return BindQuery<TModel>(i_tModel, i_nColumns, MySQL::EPreparedBind::PBParam, i_nIgnoreColumnIndex);
			}

			inline MySQL::SPreparedBind *BindQueryConditions(std::vector<std::string *> &i_apParams)
			{
				MakeMySQLBind_Pointer(apParams, i_apParams.size());
				for (size_t i = 0, nSize = i_apParams.size(); i < nSize; i++)
				{
					SetMySQLBindSTDString(apParams, i, (*i_apParams[i]));
				}

				return apParams;
			}
		}
	}
}