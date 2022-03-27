#include "Plugin.h"

#include <Windows.h>
#include <functional>


typedef std::function<void()> fnStandard;


void Plugin::CPlugin::load()
{
	LockPlugin(this);
	if (this->isOpen())
	{
		throw CPluginInstanceAlreadyInUseException();
	}

	this->onLoad();
	this->m_hPlugin = LoadLibraryA(this->m_sPath.c_str());
	
	if (const fnStandard oFunction = reinterpret_cast<void(*)()>(this->functionByName("OnLoad")))
	{
		oFunction();
	}

	this->onLoaded();
}

void Plugin::CPlugin::close()
{
	LockPlugin(this);

	if (this->isOpen())
	{
		this->onClose();

		if (const fnStandard oFunction = reinterpret_cast<void(*)()>(this->functionByName("OnClose")))
		{
			oFunction();
		}

		FreeLibrary(this->m_hPlugin);
		this->clear();

		this->onClosed();
	}
}

void Plugin::CPlugin::clear()
{
	this->m_nId = 0;
	this->m_sName.clear();
	this->m_sPath.clear();
	this->m_mapFunctions.clear();
	this->m_hPlugin = nullptr;
}


void *Plugin::CPlugin::loadFunctionByName(const std::string &i_sName)
{
	LockPlugin(this);

	void *pFunction = nullptr;

	if (this->isOpen())
	{
		const auto oIt = this->m_mapFunctions.find(i_sName);
		if (oIt == this->m_mapFunctions.end())
		{
			this->m_mapFunctions[i_sName] = pFunction = reinterpret_cast<void *>(GetProcAddress(this->m_hPlugin, i_sName.c_str()));
		} else
		{
			pFunction = oIt->second;
		}
	}

	return pFunction;
}