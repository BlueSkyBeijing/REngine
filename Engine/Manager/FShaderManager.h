#pragma once
#include "Utility.h"
#include "TSingleton.h"

class FRHIShader;

class FShaderManager : TSingleton<FShaderManager>
{
public:
	FShaderManager();
	~FShaderManager();

    void Init();
    void UnInit();

    FRHIShader* GetOrCreateShader(std::string& fileName, std::string& enterPoint, std::string& target);

private:
    std::map<std::string, FRHIShader*> mShader;

};
