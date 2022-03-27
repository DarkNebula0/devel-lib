#pragma once
#include <functional>
#include <memory>

typedef std::function<void *()>					Constructor;
typedef std::function<std::shared_ptr<void>()>	ConstructorShared;
typedef std::function<void(void *)>				Deconstructor;

class CObjectData
{
public:
	CObjectData() = default;
	CObjectData(const CObjectData &i_oObjectData)
	{
		this->m_fnClassConstructor			= i_oObjectData.m_fnClassConstructor;
		this->m_fnClassConstructorShared	= i_oObjectData.m_fnClassConstructorShared;
		this->m_fnClassDeconstructor		= i_oObjectData.m_fnClassDeconstructor;

		this->m_nClassSize					= i_oObjectData.m_nClassSize;
		this->m_fHasVirtual					= i_oObjectData.m_fHasVirtual;
	}
	virtual ~CObjectData() {}

public:
	template<typename T>
	static CObjectData get()
	{
		typedef std::remove_pointer_t<T> Type;

		CObjectData oData;

		// Get constructors
		oData.m_fnClassConstructor = []() { return new Type(); };

		// Get move constructor
		oData.m_fnClassConstructorShared = []() { return CastShared(void, std::make_shared<Type>()); };

		// Get deconstructor
		oData.m_fnClassDeconstructor = [](void *i_pData) { delete reinterpret_cast<Type *>(i_pData); };

		// Get meta data
		oData.m_nClassSize = sizeof(Type);
		oData.m_fHasVirtual = std::has_virtual_destructor_v<Type>;

		return oData;
	}

public:
	template<typename T = void>
	T *construct()
	{
		return (this->m_fnClassConstructor ? reinterpret_cast<T *>(this->m_fnClassConstructor()) : nullptr);
	}

	template<typename T = void>
	std::shared_ptr<T> constructShared()
	{
		return (this->m_fnClassConstructorShared ? CastShared(T, this->m_fnClassConstructorShared()) : nullptr);
	}

	template<typename T = void>
	void deconstruct(T *i_tData)
	{
		if (this->m_fnClassDeconstructor)
		{
			return this->m_fnClassDeconstructor(i_tData);
		}
	}

public:
	Constructor constructor() const
	{
		return this->m_fnClassConstructor;
	}

	ConstructorShared constructorShared() const
	{
		return this->m_fnClassConstructorShared;
	}

	Deconstructor deconstructor() const
	{
		return this->m_fnClassDeconstructor;
	}

	size_t size() const
	{
		return this->m_nClassSize;
	}

	bool hasVirtual() const
	{
		return this->m_fHasVirtual;
	}

private:
	Constructor			m_fnClassConstructor;
	ConstructorShared	m_fnClassConstructorShared;
	Deconstructor		m_fnClassDeconstructor;

	size_t				m_nClassSize;
	bool				m_fHasVirtual;
};
