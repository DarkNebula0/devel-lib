#pragma once
#include <atomic>
#include <memory>

#include "MutexVector.h"
#include "ObjectData.h"
#include "Typedef.h"
#include "Plugin.h"


#define ForEachPM(Type, PluginManager)			RecursiveLockGuard(PluginManager.mutex());		\
												for (Type : PluginManager)

#define ForEachPMPtr(Type, PluginManager)		RecursiveLockGuard(PluginManager->mutex());		\
												for (Type : *PluginManager)

typedef std::shared_ptr<Plugin::CPlugin>	SharedPlugin;

namespace Plugin
{
	class CManager
	{
	public:
		CManager()
			: m_oObjectData(CObjectData::get<CPlugin>())
		{

		}
		virtual ~CManager()
		{
			this->closeAll();
		}

	public:
		auto begin() const
		{
			return this->m_apPlugins.begin();
		}

		auto end() const
		{
			return this->m_apPlugins.end();
		}

		const CMutex &mutex() const
		{
			return this->m_apPlugins.mutex();
		}

	public:
		template<typename T, std::enable_if_t<std::is_base_of_v<Plugin::CPlugin, T>> * = nullptr>
		void setClass()
		{
			this->m_oObjectData = CObjectData::get<T>();
		}

	public:
		SharedPlugin plugin(const uint64 i_nPluginId);
		SharedPlugin plugin(const std::string &i_sName);
		SharedPlugin loadPlugin(const std::string &i_stPath);

	public:
		void closeAll();

		bool closePlugin(const SharedPlugin &i_pPlugin);
		bool closePlugin(const uint64 i_nPluginId)
		{
			return this->closePlugin(this->plugin(i_nPluginId));
		}
		bool closePlugin(const std::string &i_sName)
		{
			return this->closePlugin(this->plugin(i_sName));
		}

	private:
		CMutexVector<SharedPlugin> m_apPlugins;

	private:
		static std::atomic<uint64>	s_nPluginCounter;
		CObjectData					m_oObjectData;
	};
}

typedef Plugin::CManager CPluginManager;