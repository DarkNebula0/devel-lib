#pragma once
#include "MySQLPreparedQuery.h"

#include "DatabaseORMHelperFunctions.h"
#include "DatabaseORMGlobal.h"
#include "DatabaseORMTable.h"

namespace MySQL
{
	namespace ORM
	{
		template <typename TModel>
		class CQueryBuilder
		{
		private:
			typedef CTable<TModel> DBTable;

			enum EQuery : byte
			{
				QSelect,
				QInsert,
				QUpdate,
				QDelete
			};

		public:
			CQueryBuilder(DBTable *i_pTable, const bool i_fIsPreparedQueryForced = false)
				: m_pTable(i_pTable), m_fIsPreparedQueryForced(i_fIsPreparedQueryForced), m_sColumns("*"), m_nLimit(0)
			{}

		public:
			// Care! Default value is taken from the DatabaseORMTable!
			void setForcePrepared(const bool i_fPrepared)
			{
				this->m_fIsPreparedQueryForced = i_fPrepared;
			}

			bool isPreparedForced() const
			{
				return this->m_fIsPreparedQueryForced;
			}

		private:
			template<typename TOtherModel = TModel>
			void setCache(std::shared_ptr<TOtherModel> &i_pPassedModel)
			{
				i_pPassedModel->setCachedModel(std::reinterpret_pointer_cast<CModel>(std::make_shared<TOtherModel>(*i_pPassedModel)));
			}

			std::string build(const EQuery i_eType, std::vector<std::string *> &i_asConditionBinds)
			{
				std::string sQuery;

				if (i_eType == EQuery::QSelect)
				{
					sQuery = "SELECT";
					sQuery += this->buildColumnString();
					sQuery += " FROM ";
					sQuery += this->m_pTable->name();
					sQuery += this->buildWhereString(i_asConditionBinds);
					sQuery += this->buildOrderByString();
					sQuery += this->buildLimitString();
				}
				//else if (i_eType == EQuery::QUpdate)
				//{
				//	// ToDo: Actually add it?
				//	sQuery = "UPDATE ";
				//	sQuery += this->m_pTable->name();
				//	// SET...
				//	sQuery += this->buildWhereString(i_asConditionBinds);
				//	sQuery += this->buildOrderByString();
				//	sQuery += this->buildLimitString();
				//}
				else if (i_eType == EQuery::QDelete)
				{
					sQuery = "DELETE FROM ";
					sQuery += this->m_pTable->name();
					sQuery += this->buildWhereString(i_asConditionBinds);
					sQuery += this->buildOrderByString();
					sQuery += this->buildLimitString();
				}

				return sQuery;
			}
			template <typename TOtherModel>
			bool executeQuery(const EQuery i_eQuery, std::function<void(std::shared_ptr<TOtherModel>)> i_pModelCallback)
			{
				// Cannot execute any SELECT queries after an initialized cached table! (All models are loaded in the vector in table class!)
				if (this->m_pTable->isCachedTable() && this->m_pTable->isInitialized()) throw CCachedTableException();

				std::vector<std::string *> asConditionBinds;
				const std::string sQuery = this->build(i_eQuery, asConditionBinds);


				bool fReturn = false;

				if (this->m_fIsPreparedQueryForced || !asConditionBinds.empty())
				{
					CMySQLPreparedQuery oQuery(this->database());
					if (oQuery.prepare(sQuery) == MySQL::EError::ESuccess)
					{
						TOtherModel oModel;
						MySQL::SPreparedBind *apResults = (i_eQuery != EQuery::QSelect ? nullptr : Serialize::BindQueryResults(&oModel, this->columnCount()));
						MySQL::SPreparedBind *apParams = (asConditionBinds.empty() ? nullptr : Serialize::BindQueryConditions(asConditionBinds));

						if (oQuery.exec(apResults, apParams) == MySQL::EError::ESuccess)
						{
							if (i_eQuery == EQuery::QSelect)
							{
								while (oQuery.next())
								{
									std::shared_ptr<TOtherModel> pModel = this->m_pTable->newModel<TOtherModel>(std::move(oModel));
									pModel->setIsNew(false);

									this->setCache(pModel);

									i_pModelCallback(pModel);
								}
							}

							fReturn = true;
						}

						delete[] apResults;
						delete[] apParams;
					}
				}
				else
				{
					CMySQLQuery oQuery(this->database());
					if (oQuery.exec(sQuery) == MySQL::EError::ESuccess)
					{
						if (i_eQuery == EQuery::QSelect)
						{
							// Parse Data
							while (oQuery.next())
							{
								std::shared_ptr<TOtherModel> pModel = this->m_pTable->newModel<TOtherModel>();

								if (!Serialize::DeserializeQuery(pModel.get(), oQuery))
								{
									continue;
								}

								pModel->setIsNew(false);

								this->setCache(pModel);

								i_pModelCallback(std::move(pModel));
							}
						}

						fReturn = true;
					}
				}

				return fReturn;
			}

			template <typename TOtherModel>
			bool executeQuery(std::shared_ptr<TOtherModel> i_pModel)
			{
				std::string sColumns;
				size_t nColumnCounter = 0, nIdPosition = (~0);
				::Serialize::IterateSerializableStruct<Serialize::IData<>>(i_pModel.get(), [&](Serialize::IData<> *i_pData)
				{
					if (i_pData->parseFromDatabase())
					{
						if (const std::shared_ptr<SColumnMetaData> pColumn = this->m_pTable->column(i_pData->columnIndex() + 1))
						{
							// Just needed for update => "SET id=?"
							if (i_pModel->isNew() || pColumn->sName != this->m_pTable->idColumnName())
							{
								sColumns += pColumn->sName;
								sColumns += (i_pModel->isNew() ? "," : "=?,");
							}

							if (nIdPosition == (~0) && pColumn->sName == this->m_pTable->idColumnName())
							{
								nIdPosition = nColumnCounter;
							}

							nColumnCounter++;
						}
						else
						{
							throw CColumnOutOfRangeException();
						}
					}

					return true;
				});
				if (nColumnCounter == 0)
				{
					return false;
				}
				sColumns.erase(sColumns.end() - 1);

				bool fReturn = false;

				if (i_pModel->isNew())
				{
					std::string sQuery = "INSERT INTO " + this->m_pTable->name() + "(" + sColumns + ") VALUES(" + Utils::RepeatString("?,", nColumnCounter);
					*(--sQuery.end()) = ')';

					MySQL::SPreparedBind *apParams = Serialize::BindQueryParams(i_pModel.get(), nColumnCounter);

					MySQL::CPreparedQuery oQuery(this->database());
					if (oQuery.prepare(sQuery) == MySQL::EError::ESuccess)
					{
						if (oQuery.exec(nullptr, apParams) == MySQL::EError::ESuccess)
						{
							i_pModel->setId(oQuery.lastInsertID());
							this->setCache(i_pModel);

							if (this->m_pTable->isCachedTable())
							{
								this->m_pTable->addModel(i_pModel);
							}
						}

						fReturn = true;
					}

					delete[] apParams;
				}
				else
				{
					std::shared_ptr<TOtherModel> pCachedModel = i_pModel->cachedModel<TOtherModel>();

					if (!pCachedModel || !::Serialize::CompareSerializableStruct(i_pModel.get(), pCachedModel.get(), false))
					{
						std::string sQuery = "UPDATE " + this->m_pTable->name() + " SET " + sColumns;
						if (nIdPosition != ~0)
						{
							sQuery += " WHERE ";
							sQuery += this->m_pTable->idColumnName();
							sQuery += "=";
							sQuery += std::to_string(i_pModel->id());

							MySQL::SPreparedBind *apParams = Serialize::BindQueryParams(i_pModel.get(), nColumnCounter, nIdPosition);

							CMySQLPreparedQuery oQuery(this->database());
							if (oQuery.prepare(sQuery) != MySQL::EError::ESuccess)
							{
								throw CCouldNotPrepareQueryException();
							}
							else if (oQuery.exec(nullptr, apParams) != MySQL::EError::ESuccess)
							{
								throw CCouldNotExecuteQueryException();
							}

							*pCachedModel = *i_pModel;

							delete[] apParams;
							fReturn = true;
						}
						else
						{
							throw CIdNotFoundException();
						}
					}
					else
					{
						fReturn = true;
					}
				}

				return fReturn;
			}

		public:
			template <typename TOtherModel = TModel>
			bool save(std::shared_ptr<TOtherModel> i_pModel)
			{
				return this->executeQuery(i_pModel);
			}

			template <typename TOtherModel = TModel>
			bool remove()
			{
				return this->executeQuery<TOtherModel>(EQuery::QDelete, nullptr);
			}

			template <typename TOtherModel = TModel>
			void remove(const uint64 i_nId)
			{
				return this->where(this->m_pTable->idColumnName(), "=", i_nId)->executeQuery<TOtherModel>(EQuery::QDelete, nullptr);
			}

		public:
			template <typename TOtherModel = TModel>
			std::vector<std::shared_ptr<TOtherModel>> all()
			{
				std::vector<typename DBTable::Model> apModels;

				this->executeQuery<TOtherModel>(EQuery::QSelect, [&apModels](std::shared_ptr<TOtherModel> i_pModel)
				{
					apModels.push_back(std::move(i_pModel));
				});

				return apModels;
			}

			template <typename TOtherModel = TModel>
			std::vector<std::shared_ptr<TOtherModel>> all(const std::string &i_sColumns)
			{
				return this->columns(i_sColumns)->all<TOtherModel>();
			}

			template <typename TOtherModel = TModel>
			std::vector<std::shared_ptr<TOtherModel>> all(const std::vector<std::string> &i_asColumns)
			{
				return this->columns(i_asColumns)->all<TOtherModel>();
			}


			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> find(const uint64 i_nId)
			{
				std::shared_ptr<TOtherModel> pModel;

				this->clearConditions();
				this->where(this->m_pTable->idColumnName(), "=", i_nId)->executeQuery<TOtherModel>(EQuery::QSelect, [&pModel](std::shared_ptr<TOtherModel> i_pModel)
				{
					pModel = std::move(i_pModel);
				});

				return pModel;
			}

			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> find(const uint64 i_nId, const std::string &i_sColumns)
			{
				return this->columns(i_sColumns)->find<TOtherModel>(i_nId);
			}

			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> find(const uint64 i_nId, const std::vector<std::string> &i_asColumns)
			{
				return this->columns(i_asColumns)->find<TOtherModel>(i_nId);
			}


			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> first()
			{
				std::shared_ptr<TOtherModel> pModel;

				this->limit(1)->executeQuery<TOtherModel>(EQuery::QSelect, [&pModel](std::shared_ptr<TOtherModel> i_pModel)
				{
					pModel = std::move(i_pModel);
				});

				return pModel;
			}

			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> first(const std::string &i_sColumns)
			{
				return this->columns(i_sColumns)->first<TOtherModel>();
			}

			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> first(const std::vector<std::string> &i_asColumns)
			{
				return this->columns(i_asColumns)->first<TOtherModel>();
			}


			// Settings

#pragma region Columns
		public:
			CQueryBuilder * columns(const std::string &i_sColumns)
			{
				this->m_sColumns = i_sColumns;
				return this;
			}

			CQueryBuilder *columns(const std::vector<std::string> &i_asColumns)
			{
				return this->columns(Utils::Join(i_asColumns, ','));
			}

			size_t columnCount() const
			{
				if (this->m_sColumns == "*")
				{
					return this->m_pTable->metaData()->columnCount();
				}

				return Utils::CountCharacters(this->m_sColumns, ',') + 1;
			}

		private:
			bool areColumnsSet()
			{
				return !this->m_sColumns.empty();
			}

			std::string buildColumnString()
			{
				if (this->areColumnsSet())
				{
					return " " + this->m_sColumns;
				}

				return "";
			}
#pragma endregion

#pragma region Where
		private:
			void clearConditions()
			{
				this->m_aoConditions.clear();
			}

			void buildAndPushCondition(const std::string &i_sColumn, const std::string &i_sValue, const bool i_fIsInsecure, const bool i_fIsString)
			{
				SDatabaseCondition oCondition;
				oCondition.sColumn = i_sColumn;
				oCondition.sValue = i_sValue;
				oCondition.fIsInsecure = i_fIsInsecure;
				oCondition.fIsString = i_fIsString;
				this->m_aoConditions.push_back(std::move(oCondition));
			}

		public:
			// Operator: =, >, <, >=, <=, !=

			template <typename TArithmetic, typename std::enable_if_t<std::is_arithmetic_v<TArithmetic>> * = nullptr>
			CQueryBuilder *where(const std::string &i_sColumn, const std::string &i_sOperator, const TArithmetic i_nValue)
			{
				this->buildAndPushCondition(i_sColumn + i_sOperator, std::to_string(i_nValue), false, false);
				return this;
			}

			CQueryBuilder *where(const std::string &i_sColumn, const std::string &i_sOperator, const std::string &i_sValue, const bool i_fIsInsecure = true)
			{
				this->buildAndPushCondition(i_sColumn + i_sOperator, i_sValue, i_fIsInsecure, true);
				return this;
			}

			template <size_t Size>
			CQueryBuilder *where(const std::string &i_sColumn, const std::string &i_sOperator, const CCharArray<Size> &i_sValue, const bool i_fIsInsecure = true)
			{
				this->buildAndPushCondition(i_sColumn + i_sOperator, i_sValue.toStdString(), i_fIsInsecure, true);
				return this;
			}


			template <typename TArithmetic, typename std::enable_if_t<std::is_arithmetic_v<TArithmetic>> * = nullptr>
			CQueryBuilder *whereAnd(const std::string &i_sColumn, const std::string &i_sOperator, const TArithmetic i_nValue)
			{
				this->buildAndPushCondition("AND " + i_sColumn + i_sOperator, std::to_string(i_nValue), false, false);
				return this;
			}

			CQueryBuilder *whereAnd(const std::string &i_sColumn, const std::string &i_sOperator, const std::string &i_sValue, const bool i_fIsInsecure = true)
			{
				this->buildAndPushCondition("AND " + i_sColumn + i_sOperator, i_sValue, i_fIsInsecure, true);
				return this;
			}

			template <size_t Size>
			CQueryBuilder *whereAnd(const std::string &i_sColumn, const std::string &i_sOperator, const CCharArray<Size> &i_sValue, const bool i_fIsInsecure = true)
			{
				this->buildAndPushCondition("AND " + i_sColumn + i_sOperator, i_sValue.toStdString(), i_fIsInsecure, true);
				return this;
			}


			template <typename TArithmetic, typename std::enable_if_t<std::is_arithmetic_v<TArithmetic>> * = nullptr>
			CQueryBuilder *whereOr(const std::string &i_sColumn, const std::string &i_sOperator, const TArithmetic i_nValue)
			{
				this->buildAndPushCondition("OR " + i_sColumn + i_sOperator, std::to_string(i_nValue), false, false);
				return this;
			}

			CQueryBuilder *whereOr(const std::string &i_sColumn, const std::string &i_sOperator, const std::string &i_sValue, const bool i_fIsInsecure = true)
			{
				this->buildAndPushCondition("OR " + i_sColumn + i_sOperator, i_sValue, i_fIsInsecure, true);
				return this;
			}

			template <size_t Size>
			CQueryBuilder *whereOr(const std::string &i_sColumn, const std::string &i_sOperator, const CCharArray<Size> &i_sValue, const bool i_fIsInsecure = true)
			{
				this->buildAndPushCondition("OR " + i_sColumn + i_sOperator, i_sValue.toStdString(), i_fIsInsecure, true);
				return this;
			}


		private:
			bool hasConditions()
			{
				return !this->m_aoConditions.empty();
			}

			std::string buildWhereString(std::vector<std::string *> &i_asBinds)
			{
				if (this->hasConditions())
				{
					std::string sConditions = " WHERE";
					for (SDatabaseCondition &oCondition : this->m_aoConditions)
					{
						sConditions += " " + oCondition.sColumn;
						if (oCondition.fIsInsecure)
						{
							sConditions += "?";
							i_asBinds.push_back(&oCondition.sValue);
						}
						else
						{
							sConditions += (oCondition.fIsString ? "'" + oCondition.sValue + "'" : oCondition.sValue);
						}
					}

					return sConditions;
				}

				return "";
			}
#pragma endregion

#pragma region Limit
		public:
			CQueryBuilder * limit(const size_t i_nLimit)
			{
				this->m_nLimit = i_nLimit;
				return this;
			}

		private:
			bool isLimitSet()
			{
				return this->m_nLimit > 0;
			}

			std::string buildLimitString()
			{
				if (this->isLimitSet())
				{
					return " LIMIT " + std::to_string(this->m_nLimit);
				}

				return "";
			}
#pragma endregion

#pragma region OrderBy
		public:
			CQueryBuilder * orderBy(const std::string &i_sColumn, const EOrderBy i_eBy = EOrderBy::OBDefault)
			{
				std::string sOrder;
				if (i_eBy == EOrderBy::OBDesc)
				{
					sOrder = " DESC";
				}
				else if (i_eBy == EOrderBy::OBAsc)
				{
					sOrder = " ASC";
				}

				if (i_eBy == EOrderBy::OBDefault || !sOrder.empty())
				{
					this->m_aoOrderBy.push_back(i_sColumn + sOrder);
					this->m_aoOrderBy.push_back(i_sColumn + sOrder);
				}

				return this;
			}

			CQueryBuilder *orderBy(const uint64 i_nRelativePosition, const EOrderBy i_eBy = EOrderBy::OBDefault)
			{
				return this->orderBy(std::to_string(i_nRelativePosition), i_eBy);
			}

		private:
			bool areOrdersSet()
			{
				return !this->m_aoOrderBy.empty();
			}

			std::string buildOrderByString()
			{
				if (this->areOrdersSet())
				{
					std::string sOrderBy;

					for (const std::string &sOrder : this->m_aoOrderBy)
					{
						if (!sOrderBy.empty())
						{
							sOrderBy += ",";
						}

						sOrderBy += sOrder;
					}

					return " ORDER BY " + sOrderBy;
				}

				return "";
			}
#pragma endregion

		private:
			std::shared_ptr<CMySQLDatabase> database()
			{
				return this->m_pTable->orm()->database();
			}

		private:
			DBTable * m_pTable;
			bool m_fIsPreparedQueryForced;

			std::string m_sColumns;
			size_t m_nLimit;
			std::vector<SDatabaseCondition> m_aoConditions;
			std::vector<std::string> m_aoOrderBy;
		};
	}
}