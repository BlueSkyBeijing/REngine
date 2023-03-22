#pragma once
#include "Prerequisite.h"

inline UINT CalcConstantBufferByteSize(UINT byteSize)
{
	return (byteSize + 255) & ~255;
}

static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const FString& entrypoint,
	const FString& target);


class FShaderInclude : public ID3DInclude {
public:
	FShaderInclude(const char* shaderDir, const char* systemDir) : mShaderDir(shaderDir), mSystemDir(systemDir) {}

	HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes);
	HRESULT __stdcall Close(LPCVOID pData);
private:
	std::string mShaderDir;
	std::string mSystemDir;
};