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

/*
enum EMaterialShadingModel
{
    MSM_Unlit,
    MSM_DefaultLit,
    MSM_Subsurface,
    MSM_PreintegratedSkin,
    MSM_ClearCoat,
    MSM_SubsurfaceProfile,
    MSM_TwoSidedFoliage,
    MSM_Hair,
    MSM_Cloth,
    MSM_Eye,
    MSM_SingleLayerWater,
    MSM_ThinTranslucent,
    MSM_Strata,
    MSM_NUM,
};
*/

class FDefaultLitShadingModel : public FShadingModel
{
public:
    virtual ~FDefaultLitShadingModel();

};

class FSubsurfaceShadingModel : public FShadingModel
{
public:
	virtual ~FSubsurfaceShadingModel();

};

class FPreintegratedSkinShadingModel : public FShadingModel
{
public:
	virtual ~FPreintegratedSkinShadingModel();

};

class FClearCoatShadingModel : public FShadingModel
{
public:
    virtual ~FClearCoatShadingModel();

};

class FTwoSidedFoliageShadingModel : public FShadingModel
{
public:
    virtual ~FTwoSidedFoliageShadingModel();

};

class FClothShadingModel : public FShadingModel
{
public:
    virtual ~FClothShadingModel();

};

class FEyeShadingModel : public FShadingModel
{
public:
    virtual ~FEyeShadingModel();

};

