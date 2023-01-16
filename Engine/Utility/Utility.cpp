#include "PrecompiledHeader.h"

#include "Utility.h"
#include "CityHash.h"

uint64 HashMemory(const char* Data, int32 NumBytes)
{
    return CityHash64(Data, NumBytes);
}

void ReadUnrealString(std::ifstream& file, FString& outString)
{
    int32 stringSize;
    file.read((char*)&stringSize, sizeof(int32));
    file.read((char*)outString.data(), stringSize);

    outString = FString(outString.c_str());
}

void ReplaceSubString(FString& str, const FString& before, const FString& after)
{
    for (FString::size_type pos(0); pos != FString::npos; pos += after.length())
    {
        pos = str.find(before, pos);
        if (pos != FString::npos)
            str.replace(pos, before.length(), after);
        else
            break;
    }
}

