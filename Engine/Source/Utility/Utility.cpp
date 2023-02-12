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
    outString.resize(stringSize);
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

void StringSplit(const FString& str, const FString& splitStr, TArray<FString>& res)
{
    if (str.size() == 0)
    {
        return;
    }

    FString strs = str + splitStr;
    size_t pos = strs.find(splitStr);
    size_t step = splitStr.size();

    while (pos != strs.npos)
    {
        FString temp = strs.substr(0, pos);
        res.push_back(temp);
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splitStr);
    }
}

void FullFileNameSplit(const FString& fullFileName, FString& filePath, FString& fileName, FString& name, FString& suffix)
{
    size_t iPos = fullFileName.find_last_of('\\');
    filePath = fullFileName.substr(0, iPos + 1);
    fileName = fullFileName.substr(iPos + 1, fullFileName.length() - iPos);
    name = fileName.substr(0, fileName.rfind("."));
    suffix = fileName.substr(fileName.find_last_of('.') + 1);
}