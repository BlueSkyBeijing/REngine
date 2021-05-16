#include "PrecompiledHeader.h"

#include "Utility.h"
#include "CityHash.h"

uint64 HashMemory(const void* Data, int32 NumBytes)
{
    return CityHash64((const char*)Data, NumBytes);
}
