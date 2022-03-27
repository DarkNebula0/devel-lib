#pragma once
#include "DatabaseSerializableORMTable.h"
#include "DatabaseSerializableBool.h"

namespace MySQL
{
	namespace ORM
	{
#pragma pack(push, 1)
		class CModel;
	
		namespace Serialize
		{
			template <typename T = std::vector<std::string>>
			class IColumnArray : public IData<-1, T, true>
			{
			public:
				void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) override
				{
					throw CShouldNotExecuteException();
				}

			public:
				using IData<-1, T, true>::IData;
				CreateSerializeOperatorsCustomDerive(T);
			};

			template<typename T = std::shared_ptr<CModel>>
			class ICachedModel : public IData<-1, T, true>
			{
			public:
				void bind(MySQL::SPreparedBind *i_pBind, const size_t i_nIndex, const MySQL::EPreparedBind i_eBindType) override
				{
					throw CShouldNotExecuteException();
				}

			public:
				using IData<-1, T, true>::IData;
				CreateSerializeOperatorsCustomDerive(T);
			};
		}

		class CModel
		{
			template <typename TModel, std::enable_if_t<std::is_base_of_v<CModel, TModel>> *> friend class ORM::CTable;
			template <typename TModel> friend class CQueryBuilder;

		public:
			typedef Serialize::IColumnArray<> ColumnArray;

		public:
			CModel() = default;
			virtual ~CModel() = default;

		public:
			virtual void setId(const uint64 i_nId)	{ throw COverwriteMethodException(); }
			virtual uint64 id()						{ throw COverwriteMethodException(); }

		private:
			void setTable(const std::weak_ptr<ORM::CTable<>> i_pTable)
			{
				this->m_pTable = i_pTable;
			}

			void setIsNew(const bool i_fIsNew)
			{
				this->m_fIsNew = i_fIsNew;
			}

			void setCachedModel(const std::shared_ptr<CModel> &i_pModel)
			{
				this->m_pCachedModel = i_pModel;
			}

		private:
			template<typename TModel = CModel>
			std::shared_ptr<TModel> cachedModel() const
			{
				return std::reinterpret_pointer_cast<TModel>(this->m_pCachedModel.value());
			}

		public:
			template <typename TTable = ORM::CTable<>>
			std::shared_ptr<TTable> table() const
			{
				const std::shared_ptr<ORM::CTable<>> pTable = this->m_pTable.value().lock();
				if (pTable)
				{
					return std::reinterpret_pointer_cast<TTable>(pTable);
				}

				return nullptr;
			}

			bool isNew() const
			{
				return this->m_fIsNew;
			}

		private:
			Serialize::ICachedModel<>		m_pCachedModel;
			Serialize::ITable<>				m_pTable;
			Serialize::IBoolean<-1, true>	m_fIsNew;
		};
#pragma pack(pop)
	}
}