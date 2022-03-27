#pragma once
#include <vector>

#include "SerializableData.h"
#include "SerializerStream.h"

namespace Serialize
{
#pragma pack(push, 1)
	template<typename Type = int, bool IsHidden = false, typename TSize = uint, typename T = std::vector<Type>>
	class IVector : public IData<T, IsHidden>
	{
	private:
		template<typename TCopy = Type, typename std::enable_if_t<std::is_base_of_v<Serialize::IField, TCopy>> * = nullptr>
		void _serialize(CWriteStream &i_oStream)
		{
			for (TCopy &tType : this->m_tValue)
			{
				tType.doSerialize(i_oStream);
			}
		}

		template<typename TCopy = Type, typename std::enable_if_t<std::is_base_of_v<Serialize::IStruct, TCopy>> * = nullptr>
		void _serialize(CWriteStream &i_oStream)
		{
			for (TCopy &tType : this->m_tValue)
			{
				Serialize::SerializeStream(tType, i_oStream);
			}
		}

		template<typename TCopy = Type, 
		typename std::enable_if_t<!std::is_base_of_v<Serialize::IField, TCopy>> * = nullptr, 
		typename std::enable_if_t<!std::is_base_of_v<Serialize::IStruct, TCopy>> * = nullptr>
		void _serialize(CWriteStream &i_oStream)
		{
			for (const TCopy &tType : this->m_tValue)
			{
				i_oStream.push<TCopy>(tType);
			}
		}

		bool serialize(CWriteStream &i_oStream) override
		{
			i_oStream.push<TSize>(static_cast<TSize>(this->m_tValue.size()));
			this->_serialize(i_oStream);
			return true;
		}

	private:
		template<typename TCopy = Type, typename std::enable_if_t<std::is_base_of_v<Serialize::IField, TCopy>> * = nullptr>
		void _deserialize(CReadStream &i_oStream, size_t i_nSize)
		{
			for (; i_nSize > 0; i_nSize--)
			{
				this->m_tValue.emplace_back();
				(--this->m_tValue.end())->doDeserialize(i_oStream);
			}
		}

		template<typename TCopy = Type, typename std::enable_if_t<std::is_base_of_v<Serialize::IStruct, TCopy>> * = nullptr>
		void _deserialize(CReadStream &i_oStream, size_t i_nSize)
		{
			for (; i_nSize > 0; i_nSize--)
			{
				this->m_tValue.emplace_back();
				Serialize::DeserializeStream(*(--this->m_tValue.end()), i_oStream);
			}
		}

		template<typename TCopy = Type, 
		typename std::enable_if_t<!std::is_base_of_v<Serialize::IField, TCopy>> * = nullptr, 
		typename std::enable_if_t<!std::is_base_of_v<Serialize::IStruct, TCopy>> * = nullptr>
		void _deserialize(CReadStream &i_oStream, size_t i_nSize)
		{
			for (; i_nSize > 0; i_nSize--)
			{
				this->m_tValue.push_back(i_oStream.get<Type>());
			}
		}

		bool deserialize(CReadStream &i_oStream) override
		{
			const TSize nSize = i_oStream.get<TSize>();
			this->m_tValue.reserve(nSize > 30 ? 30 : nSize);

			this->_deserialize(i_oStream, nSize);
			return true;
		}

	public:
		CreateSerializeOperators(T, IsHidden);
	};
#pragma pack(pop)
}