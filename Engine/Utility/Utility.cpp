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

void ReplaceSubString(std::string& str, const std::string& before, const std::string& after)
{
    for (std::string::size_type pos(0); pos != std::string::npos; pos += after.length())
    {
        pos = str.find(before, pos);
        if (pos != std::string::npos)
            str.replace(pos, before.length(), after);
        else
            break;
    }
}

