#pragma once
#include "TSingleton.h"

#include <string>

class FConfigManager : TSingleton<FConfigManager>
{
public:
	FConfigManager();
	~FConfigManager();

	static std::string ContentPath;

private:

};
