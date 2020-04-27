#ifndef MARKERARGUMENTS_H
#define MARKERARGUMENTS_H
#include <Windows.h>

struct params
{
	int n;
	int* mas;
	int index;
	HANDLE hBeginWorkEvent;
	HANDLE hStopWorkEvent;
	HANDLE* hContinueOrTerminateWork;
};


#endif // !MARKERARGUMENTS_H
