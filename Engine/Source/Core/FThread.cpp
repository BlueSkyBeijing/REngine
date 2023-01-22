#include "PrecompiledHeader.h"
#include "WindowsUtility.h"

#include "FThread.h"

FThread::FThread(FString Name, std::function<void(void)> Func): mFunc(Func), mThread(nullptr)
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	SetThreadName(-1, "RenderThread");
#endif
}

FThread::~FThread()
{
	release();
}

void FThread::Join()
{
	release();

	mThread = new std::thread(mFunc);
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
