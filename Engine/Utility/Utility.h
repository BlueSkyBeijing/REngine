#pragma once
#include <d3d12.h>
#include <stdint.h>
#include <Eigen/Dense>

typedef uint8_t	    uint8;
typedef uint16_t	uint16;
typedef uint32_t	uint32;
typedef uint64_t	uint64;
typedef	int8_t		int8;
typedef int16_t	    int16;
typedef int32_t   	int32;
typedef int64_t	    int64;

typedef Eigen::Vector4f FVector4;
typedef Eigen::Vector3f FVector3;
typedef Eigen::Vector2f FVector2;

typedef Eigen::Matrix4f FMatrix4x4;

typedef D3D12_VIEWPORT FViewPort;
typedef D3D12_RECT FRect;
typedef D3D12_INPUT_ELEMENT_DESC FInputElementDesc;
