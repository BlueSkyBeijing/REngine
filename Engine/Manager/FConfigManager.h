#pragma once
#include "TSingleton.h"

class FConfigManager : TSingleton<FConfigManager>
{
public:
	FConfigManager();
	~FConfigManager();

	static std::string ContentPath;

private:

};
