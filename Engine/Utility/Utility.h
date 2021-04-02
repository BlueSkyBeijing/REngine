#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include <stdint.h>

typedef uint8_t	    uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef	int8_t		int8;
typedef int16_t	    int16;
typedef int32_t   	int32;
typedef int64_t	    int64;

typedef DirectX::XMFLOAT4 FVector4;
typedef DirectX::XMFLOAT3 FVector3;
typedef DirectX::XMFLOAT2 FVector2;
typedef DirectX::XMFLOAT2 FVector2;

typedef DirectX::XMFLOAT4X4 FMatrix4x4;

typedef D3D_PRIMITIVE_TOPOLOGY FPrimitiveTopology;

typedef D3D12_VIEWPORT FViewPort;
typedef D3D12_RECT FRect;
typedef D3D12_INPUT_ELEMENT_DESC FInputElementDesc;