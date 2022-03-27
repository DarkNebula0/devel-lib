#pragma once
#include <type_traits>

#define SerializableGetField(Type, Struct, Index)		reinterpret_cast<Type *>(reinterpret_cast<char *>(Struct) + Index)

#define MakeAssignmentOperatorsPrivate(Type)		\
		auto &operator=(const Type &i_tValue)		\
		{											\
			this->m_tValue = i_tValue;				\
			return *this;							\
		}											\
													\
		auto &operator=(Type &&i_tValue)			\
		{											\
			this->m_tValue = i_tValue;				\
			return *this;							\
		}

#define CreateSerializeOperators(T, IsHidden)		\
		using Serialize::IData<T, IsHidden>::IData;	\
		MakeAssignmentOperatorsPrivate(T);

#define CreateSerializeOperatorsCustomDerive(T)	\
		MakeAssignmentOperatorsPrivate(T);

#define SerializerEnableIfNotArithmetic(TReturn)	template<typename T1 = T> std::enable_if_t<!std::is_arithmetic_v<T1>, TReturn>