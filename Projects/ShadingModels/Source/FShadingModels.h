#pragma once
#include "IModuleInterface.h"


class FShadingModels: public IModuleInterface
{
public:
	virtual ~FShadingModels();

	virtual void Init();

	virtual void UnInit();
};