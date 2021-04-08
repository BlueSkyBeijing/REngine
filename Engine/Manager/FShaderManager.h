#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FRHIShader;

class FShaderManager : TSingleton<FShaderManager>
{
public:
    FShaderManager();
    ~FShaderManager();

    void Init();
    void UnInit();

    FRHIShader* CreateShader(std::string& fileName, std::string& enterPoint, std::string& target);
    FRHIShader* GetShader(std::string& fileName, std::string& enterPoint, std::string& target);

private:
    std::map<std::string, FRHIShader*> mShader;

};
