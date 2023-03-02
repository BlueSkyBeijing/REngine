#include "PrecompiledHeader.h"

#include "FModuleManager.h"
#include "TSingleton.h"
#include "IModuleInterface.h"
#include "FConfigManager.h"
#include "FLogManager.h"

FModuleManager::FModuleManager()
{
}

FModuleManager::~FModuleManager()
{
}

void FModuleManager::Init()
{

}

void FModuleManager::UnInit()
{
	for (auto iter = Modules.begin(); iter != Modules.end(); iter++)
	{
		IModuleInterface* module = iter->second;
		module->UnInit();
		delete module;
	}

	Modules.clear();
}

IModuleInterface* FModuleManager::GetOrCreate(const FString& InModuleName)
{
	return nullptr;
}

IModuleInterface* FModuleManager::FindModule(FString InModuleName)
{
	auto iter = Modules.find(InModuleName);
	if (iter != Modules.end())
	{
		return iter->second;
	}
	return nullptr;
}

bool FModuleManager::LoadProjectMoudules(const FString& projectPath, const FString& projectName)
{
	FString dllFileName = projectPath + "Binaries\\" + *FConfigManager::Platform + "\\" + *FConfigManager::Configuration + "\\" + projectName + ".dll";
	HINSTANCE handle = LoadLibraryA(dllFileName.c_str());
	typedef IModuleInterface* (*InitializeModuleFunPtr)();
	if (handle == nullptr)
	{
		FreeLibrary(handle);

		TSingleton<FLogManager>::GetInstance().LogMessage(LL_Info, "load module fail");
	}
	else
	{
		InitializeModuleFunPtr funPtr = (InitializeModuleFunPtr)GetProcAddress(handle, "InitializeModule");
		if (funPtr != nullptr)
		{
			IModuleInterface* module = funPtr();
			module->Init();

			Modules.insert(std::make_pair(projectName, module));
		}
		else
		{
			TSingleton<FLogManager>::GetInstance().LogMessage(LL_Info, "get process error");
		}
	}

	return true;
}
