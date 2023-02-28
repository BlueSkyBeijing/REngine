#pragma once
#include "Prerequisite.h"

DLL_API std::wstring StringToWString(const FString & str);

DLL_API FString WStringToString(const std::wstring & wstr);

DLL_API uint64 HashMemory(const char* Data, int32 NumBytes);

DLL_API void ReadUnrealString(std::ifstream& file, FString& outString);

DLL_API void ReplaceSubString(FString& str, const FString& before, const FString& after);

DLL_API void StringSplit(const FString& str, const FString& splitStr, TArray<FString>& res);

DLL_API void FullFileNameSplit(const FString& fullFileName, FString& filePath, FString& fileName, FString& name, FString& suffix);
