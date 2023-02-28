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

class FCustomShadingModel : public FShadingModel
{
public:
    FCustomShadingModel(int32 inValue);
    virtual ~FCustomShadingModel();

    virtual void Init() override;

    virtual void UnInit();

};

class FDefaultLitShadingModel : public FCustomShadingModel
{
public:
    virtual ~FDefaultLitShadingModel();

    virtual void Init();

    virtual void UnInit();

};

class FSubsurfaceShadingModel : public FCustomShadingModel
{
public:
	virtual ~FSubsurfaceShadingModel();

};

class FPreintegratedSkinShadingModel : public FCustomShadingModel
{
public:
	virtual ~FPreintegratedSkinShadingModel();

};

class FClearCoatShadingModel : public FCustomShadingModel
{
public:
    virtual ~FClearCoatShadingModel();

};

class FTwoSidedFoliageShadingModel : public FCustomShadingModel
{
public:
    virtual ~FTwoSidedFoliageShadingModel();

};

class FClothShadingModel : public FCustomShadingModel
{
public:
    virtual ~FClothShadingModel();

};

class FEyeShadingModel : public FCustomShadingModel
{
public:
    virtual ~FEyeShadingModel();

};

