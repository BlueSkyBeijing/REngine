#pragma once
#include "Prerequisite.h"

inline std::wstring StringToWString(const std::string& str)
{
    int32 len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);
    wchar_t* wide = new wchar_t[len + 1];
    memset(wide, '\0', sizeof(wchar_t) * (len + 1));
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, len);
    std::wstring w_str(wide);
    delete[] wide;
    return w_str;
}

inline std::string WStringToString(const std::wstring& wstr)
{
    int32 len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int32>(wstr.size()), nullptr, 0, nullptr, nullptr);
    char* buffer = new char[len + 1];
    memset(buffer, '\0', sizeof(char) * (len + 1));
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int32>(wstr.size()), buffer, len, nullptr, nullptr);
    std::string result(buffer);
    delete[] buffer;
    return result;
}

uint64 HashMemory(const char* Data, int32 NumBytes);


void ReadUnrealString(std::ifstream& file, std::string& outString);

void ReplaceSubString(std::string& str, const std::string& before, const std::string& after);
