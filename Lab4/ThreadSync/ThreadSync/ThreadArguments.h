#ifndef THREADARGUMENTS_H
#define THREADARGUMENTS_H

#include <Windows.h>
#include <vector>


struct params
{
	int n;
	int* mas;
	int index;
	HANDLE hBeginWorkEvent;
	HANDLE hStopWorkEvent;
	HANDLE* hContinueOrTerminateWork;
};


#endif // !THREADARGUMENTS_H
