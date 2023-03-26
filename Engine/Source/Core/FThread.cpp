#include "PrecompiledHeader.h"
#include "WindowsUtility.h"

#include "FThread.h"

FThread::FThread(FString Name, std::function<void(void)> Func): mFunc(Func), mThread(nullptr), mName(Name)
{
}

FThread::~FThread()
{
	release();
}

void FThread::Join()
{
	release();

	mThread = new std::thread(mFunc);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	DWORD threadId = ::GetThreadId(static_cast<HANDLE>(mThread->native_handle()));
	SetThreadName(threadId, mName.c_str());
#endif

	mThread->detach();
}

void FThread::release()
{
	if (mThread)
	{
		delete mThread;
		mThread = nullptr;
	}
}
