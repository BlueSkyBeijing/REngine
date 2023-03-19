#pragma once
#include "Prerequisite.h"
#include "IModuleInterface.h"
#include "FShadingModel.h"


class FShadingModels: public IModuleInterface
{
public:
	virtual ~FShadingModels();

	virtual void Init();

	virtual void UnInit();
};

class FCustomShadingModel : public FShadingModel
{
public:
    FCustomShadingModel(int32 inValue);
    virtual ~FCustomShadingModel();

    virtual void Init() override;

    virtual void UnInit();

};
