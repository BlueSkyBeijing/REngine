#include "PrecompiledHeader.h"

#include "FModuleManager.h"
#include "TSingleton.h"
#include "IModuleInterface.h"

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
}

IModuleInterface* FModuleManager::GetOrCreate(const FString& InModuleName)
{
	return nullptr;
}