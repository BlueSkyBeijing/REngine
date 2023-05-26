#include "PrecompiledHeader.h"

#include "DX12Utility.h"

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
	const std::wstring& filename,
	const D3D_SHADER_MACRO* defines,
	const FString& entrypoint,
	const FString& target)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr = S_OK;

	Microsoft::WRL::ComPtr<ID3DBlob> byteCode = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errors;
	hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	return byteCode;
}

HRESULT __stdcall FShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes) {
    try {
        std::string finalPath;
        switch (IncludeType) {
        case D3D_INCLUDE_LOCAL:
            finalPath = mShaderDir + "\\" + pFileName;
            break;
        case D3D_INCLUDE_SYSTEM:
            finalPath = mSystemDir + "\\" + pFileName;
            break;
        default:
            assert(0);
        }

        std::ifstream includeFile(finalPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        if (includeFile.is_open()) {
            long long fileSize = includeFile.tellg();
            char* buf = new char[fileSize];
            includeFile.seekg(0, std::ios::beg);
            includeFile.read(buf, fileSize);
            includeFile.close();
            *ppData = buf;
            *pBytes = (UINT)fileSize;
        }
        else {
            return E_FAIL;
        }
        return S_OK;
    }
    catch (std::exception& ) {
        return E_FAIL;
    }
}

HRESULT __stdcall FShaderInclude::Close(LPCVOID pData) {
    char* buf = (char*)pData;
    delete[] buf;
    return S_OK;
}
