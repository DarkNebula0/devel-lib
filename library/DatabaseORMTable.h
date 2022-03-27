#pragma once
#include <type_traits>

#include "DatabaseORMModel.h"
#include "DatabaseMetaData.h"
#include "MutexVector.h"

namespace MySQL
{
	namespace ORM
	{
		enum ETableType : byte
		{
			TTLive,
			TTCached,
		};

		template <typename TModel, std::enable_if_t<std::is_base_of_v<CModel, TModel>> *>
		class CTable : public std::enable_shared_from_this<CTable<TModel>>
		{
			friend class CQueryBuilder<TModel>;
		public:
			typedef TModel RawModel;
			typedef std::shared_ptr<TModel> Model;

		public:
			CTable(const ETableType i_eTableType, const bool i_fForcePreparedQuery, const std::string &i_sIdColumnName = "id")

				: m_pOrm(nullptr), m_eTableType(i_eTableType), m_sIdColumnName(i_sIdColumnName), m_fIsPreparedQueryForced(i_fForcePreparedQuery), m_fIsInitialized(false)
			{}

			virtual ~CTable() = default;

		public:
			virtual void onAddModel(Model i_pModel) { }
			virtual void onRemoveModel(size_t i_nId) { }
			virtual void onSaveAll() { }

		private:
			template <typename TOtherModel = TModel>
			bool hasCorrectTable(std::shared_ptr<TOtherModel> i_pModel)
			{
				if (static_cast<void *>(i_pModel->table().get()) != this)
				{
					throw CModelHasWrongTableExcpetion();
				}

				return true;
			}

		public:
			bool saveAll()
			{
				if (this->isCachedTable())
				{
					this->onSaveAll();
					this->save(this->m_apModels);
				}

				return false;
			}

			template <typename TOtherModel = TModel>
			bool save(std::shared_ptr<TOtherModel> i_pModel)
			{
				if (this->hasCorrectTable(i_pModel))
				{
					return this->newQuery()->save(i_pModel);
				}

				return false;
			}

			template <typename TOtherModel = TModel>
			bool save(std::vector<std::shared_ptr<TOtherModel>> &i_apModel)
			{
				for (const auto &pModel : i_apModel)
				{
					if (!this->save(pModel))
					{
						return false;
					}
				}

				return true;
			}

			template <typename TOtherModel = TModel>
			bool save(CMutexVector<std::shared_ptr<TOtherModel>> &i_apModel)
			{
				ForEachMV(const auto &pModel, i_apModel)
				{
					if (!this->save(pModel))
					{
						return false;
					}
				}

				return true;
			}

			bool remove(const size_t i_nId)
			{
				this->onRemoveModel(i_nId);
				return this->newQuery()->remove(i_nId);
			}

			template <typename TOtherModel = TModel>
			bool remove(std::shared_ptr<TOtherModel> i_pModel)
			{
				if (this->hasCorrectTable(i_pModel))
				{
					return this->remove(i_pModel->id());
				}

				return false;
			}

		public:
			const std::string &name() const
			{
				return this->m_pMetaData->name();
			}

			std::shared_ptr<CQueryBuilder<TModel>> newQuery()
			{
				return std::make_shared<CQueryBuilder<TModel>>(this, this->m_fIsPreparedQueryForced);
			}

		private:
			template <typename TOtherModel = TModel>
			void initializeModel(std::shared_ptr<TOtherModel> &i_pModel)
			{
				i_pModel->setTable(reinterpret_cast<CTable<> *>(this)->shared_from_this());
				i_pModel->setIsNew(true);
			}

		public:
			void addModel(const Model &i_pModel)
			{
				if (!this->m_apModels.contains(i_pModel))
				{
					this->onAddModel(i_pModel);
					this->m_apModels.push_back(i_pModel);
				}
			}

			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> newModel()
			{
				std::shared_ptr<TOtherModel> pModel = std::make_shared<TOtherModel>();
				this->initializeModel(pModel);
				pModel->setId(0);
				return pModel;
			}

			template <typename TOtherModel = TModel>
			std::shared_ptr<TOtherModel> newModel(TOtherModel &&i_oModel)
			{
				std::shared_ptr<TOtherModel> pModel = std::make_shared<TOtherModel>(std::move(i_oModel));
				this->initializeModel(pModel);
				return pModel;
			}

		public:
			bool isLiveTable() const
			{
				return this->m_eTableType == ETableType::TTLive;
			}

			bool isCachedTable() const
			{
				return this->m_eTableType == ETableType::TTCached;
			}

			bool isInitialized() const
			{
				return this->m_fIsInitialized;
			}


		public:
			void setTableData(CORM *i_pOrm, std::shared_ptr<CTableMetaData> i_pMetaData)
			{
				this->m_pOrm = i_pOrm;
				this->m_pMetaData = std::move(i_pMetaData);

				if (this->m_eTableType == ETableType::TTCached)
				{
					this->m_apModels = this->newQuery()->all();
				}

				this->m_fIsInitialized = true;
			}

			std::shared_ptr<CTableMetaData> metaData() const
			{
				return this->m_pMetaData;
			}

			std::shared_ptr<SColumnMetaData> column(const std::string &i_sName) const
			{
				return this->m_pMetaData->columnMetaData(i_sName);
			}

			std::shared_ptr<SColumnMetaData> column(const size_t i_nIndex) const
			{
				return this->m_pMetaData->columnMetaData(i_nIndex);
			}


			const std::string &idColumnName() const
			{
				return this->m_sIdColumnName;
			}


			void setForcePreparedQuery(const bool i_fForcePreparedQuery)
			{
				this->m_fIsPreparedQueryForced = i_fForcePreparedQuery;
			}

			bool isPreparedQueryForced() const
			{
				return this->m_fIsPreparedQueryForced;
			}

		private:
			CORM *orm() const
			{
				return this->m_pOrm;
			}

		private:
			CORM								*m_pOrm;
			ETableType							m_eTableType;
			const std::string					m_sIdColumnName;
			std::shared_ptr<CTableMetaData>		m_pMetaData;
			bool								m_fIsPreparedQueryForced;
			bool								m_fIsInitialized;

		protected:
			// Real models
			CMutexVector<Model>					m_apModels;
		};
	}
}