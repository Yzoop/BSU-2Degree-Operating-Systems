#ifndef MARKERARGUMENTS_H
#define MARKERARGUMENTS_H
#include <Windows.h>

#define NUM_OF_EVENTS 2

struct marker_argument_st
{
	int id;
	int arr_size;
	int* arr;
	HANDLE h_finish_event;
	HANDLE h_start_event;
	HANDLE h_event[NUM_OF_EVENTS];
};

marker_argument_st get_marker_argument_of(int id, int* arr, int arr_size, HANDLE h_start, HANDLE h_finish) {
	marker_argument_st marker_argument;
	marker_argument.id = id;
	marker_argument.arr = arr;
	marker_argument.arr_size = arr_size;
	marker_argument.h_start_event = h_start;
	marker_argument.h_finish_event = h_finish;

	return marker_argument;
}


#endif // !MARKERARGUMENTS_H
