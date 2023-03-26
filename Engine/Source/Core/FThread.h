#pragma once
#include "Prerequisite.h"

class FThread
{
public:
	FThread(FString Name, std::function<void(void)> Func);
	~FThread();

	void Join();

protected:
	void release();

private:
	std::function<void(void)> mFunc;
	std::thread* mThread;
	FString mName;
};