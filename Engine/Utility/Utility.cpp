#include "PrecompiledHeader.h"

#include "Utility.h"
#include "CityHash.h"

uint64 HashMemory(const char* Data, int32 NumBytes)
{
    return CityHash64(Data, NumBytes);
}
