#pragma once
class UObject;

enum EResourceType
{
    RT_StaticMesh,
    RT_Texture,
    RT_Material,
    RT_Map,
    RT_MaxNum
};

class FResourceManager
{
public:
	FResourceManager();
	~FResourceManager();

    void Init();
    void UnInit();

    UObject* GetOrLoad();

private:

};
