#pragma once
#include <windows.h>
#include <string>
#include <cassert>
#include <stdexcept>

class HResultException : public std::runtime_error
{
public:
	HResultException(HRESULT hresult) : std::runtime_error(HResultToString(hresult)), mHResult(hresult)
	{
	}

	HRESULT GetError() const 
	{
		return mHResult;
	}

protected:
	inline std::string HResultToString(HRESULT hresult)
	{
		char infos[64] = {};
		sprintf_s(infos, "HRESULT 0x%08X", static_cast<UINT>(hresult));

		return std::string(infos);
	}
private:
	const HRESULT mHResult;
};

#define THROW_IF_FAILED(hresult)						   \
{													       \
	if (FAILED(hresult))								   \
	{													   \
		throw HResultException(hresult);				   \
	}													   \
}
