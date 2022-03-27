#pragma once
 
#include "LockGuard.h"
#include "Typedef.h"
#include "Global.h"

#include <string>
#include <map>

#define LockPlugin(x)    RecursiveLockGuard(x->mutex())

namespace Plugin
{
	class CPlugin
	{
		friend class Plugin::CManager;
	public:
		CPlugin()
			: m_hPlugin(nullptr), m_nId(0)
		{}

		virtual  ~CPlugin() = default;

	public:
		virtual void onLoad() { }
		virtual void onLoaded() { }

		virtual void onClose() { }
		virtual void onClosed() { }

	private:
		void load();
		void close();
		void clear();

	public:
		const CMutex &mutex() const
		{
			return this->m_oMutex;
		}

		uint64 id() const
		{
			return this->m_nId;
		}

		const std::string &name() const
		{
			return this->m_sName;
		}

		const std::string &path() const
		{
			return this->m_sPath;
		}

		bool isOpen() const
		{
			return (this->m_hPlugin != nullptr);
		}

	public:
		template<typename Fn = void *>
		Fn functionByName(const std::string &i_sName)
		{
			return static_cast<Fn>(this->loadFunctionByName(i_sName));
		}

	private:
		void *loadFunctionByName(const std::string &i_sName);

	private:
		CMutex	m_oMutex;
		HMODULE m_hPlugin;
		uint64	m_nId;

		std::string m_sName;
		std::string m_sPath;

		std::map<std::string, void *> m_mapFunctions;
	};
}