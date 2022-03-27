#include "PluginManager.h"
#include "File.h"


std::atomic<uint64> Plugin::CManager::s_nPluginCounter = 0;


SharedPlugin Plugin::CManager::plugin(const uint64 i_nPluginId)
{
	ForEachMV(const SharedPlugin &pPlugin, this->m_apPlugins)
	{
		if (pPlugin->id() == i_nPluginId)
		{
			return pPlugin;
		}
	}

	return nullptr;
}

SharedPlugin Plugin::CManager::plugin(const std::string &i_sName)
{
	ForEachMV(const SharedPlugin &pPlugin, this->m_apPlugins)
	{
		if (pPlugin->name() == i_sName)
		{
			return pPlugin;
		}
	}

	return nullptr;
}


SharedPlugin Plugin::CManager::loadPlugin(const std::string &i_sPath)
{
	if (CFile::exists(i_sPath))
	{
		if (SharedPlugin pPlugin = std::reinterpret_pointer_cast<Plugin::CPlugin>(this->m_oObjectData.constructShared()))
		{
			pPlugin->m_sPath = i_sPath;
			pPlugin->m_sName = CFile::name(i_sPath);
			pPlugin->m_nId = ++Plugin::CManager::s_nPluginCounter;
			pPlugin->load();

			if (pPlugin->isOpen())
			{
				this->m_apPlugins.push_back(pPlugin);
				return pPlugin;
			}
		}
		else
		{
			throw CCouldNotCreatePluginInstanceException();
		}
	}

	return nullptr;
}


bool Plugin::CManager::closePlugin(const SharedPlugin &i_pPlugin)
{
	if (i_pPlugin)
	{
		i_pPlugin->close();
		this->m_apPlugins.remove(i_pPlugin);
		return true;
	}

	return false;
}

void Plugin::CManager::closeAll()
{
	ForEachMV(const SharedPlugin &pPlugin, this->m_apPlugins)
	{
		pPlugin->close();
	}

	this->m_apPlugins.clear();
}