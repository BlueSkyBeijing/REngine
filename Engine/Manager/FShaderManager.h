#pragma once
#include <map>
#include <string>
#include "Utility.h"
#include "TSingleton.h"

class FShader;

class FShaderManager : TSingleton<FShaderManager>
{
public:
	FShaderManager();
	~FShaderManager();

    void Init();
    void UnInit();

    FShader* GetOrCreateShader(std::string& fileName, std::string& enterPoint, std::string& target);

private:
    std::map<std::string, FShader*> mShader;

};
