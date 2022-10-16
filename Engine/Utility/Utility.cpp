#include "PrecompiledHeader.h"

#include "Utility.h"
#include "CityHash.h"

uint64 HashMemory(const char* Data, int32 NumBytes)
{
    return CityHash64(Data, NumBytes);
}

void ReadUnrealString(std::ifstream& file, std::string& outString)
{
    int32 stringSize;
    file.read((char*)&stringSize, sizeof(int32));
    file.read((char*)outString.data(), stringSize);

    outString = std::string(outString.c_str());
}

