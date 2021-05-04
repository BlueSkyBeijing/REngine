#pragma once
#include "Prerequisite.h"

#include "TSingleton.h"

class FShaderManager
{
    friend class TSingleton<FShaderManager>;

public:
    void Init();
    void UnInit();

    FRHIShader* CreateShader(std::string& fileName, std::string& enterPoint, std::string& target);
    FRHIShader* GetShader(std::string& fileName, std::string& enterPoint, std::string& target);

private:
    FShaderManager();
    virtual ~FShaderManager();

private:
    std::map<std::string, FRHIShader*> mShader;

};
