#include "thread_management.h"
#include "marker_arguments.h"
#include "Windows.h"
#include <iostream>

CRITICAL_SECTION critical_section;

void set_critical_section(CRITICAL_SECTION& main_critical_section) {
	critical_section = main_critical_section;
}

void reset_elements_of_array_with_thread_index(marker_argument_st* args)
{
	EnterCriticalSection(&critical_section);

	for (int i = 0; i < args->arr_size; i++)
	{
		if (args->arr[i] == (args->id + 1))
			args->arr[i] = 0;
	}

	LeaveCriticalSection(&critical_section);
}


bool is_thread_terminated(marker_argument_st* marker_thread_args)
{
	return WaitForSingleObject(marker_thread_args->h_event[1], 0) == WAIT_OBJECT_0;
}

void continue_thread_work(marker_argument_st* args)
{
	ResetEvent(args->h_event[0]);
}

DWORD WINAPI thread_mark_job(LPVOID param)
{
	marker_argument_st* args = (marker_argument_st*)param;
	int editsCount = 0;
	bool finished = false;
	srand(args->id);

	WaitForSingleObject(args->h_start_event, INFINITE); // Expect a signal from main.

	do {
		int random_index = rand() % args->arr_size;
		EnterCriticalSection(&critical_section);

		if (args->arr[random_index] == 0)
		{
			Sleep(5);
			args->arr[random_index] = args->id + 1;

			LeaveCriticalSection(&critical_section);

			editsCount++;
			Sleep(5);
		}

		else {
			std::cout << "\nMarker thread #" << args->id + 1 << "; editsCount = " << editsCount << "; "
				<< "elements, Array value at [" << random_index + 1 << "]\n";

			LeaveCriticalSection(&critical_section);

			// Wait for a response to continue or exit from the thread.
			SetEvent(args->h_finish_event);
			WaitForMultipleObjects(2, args->h_event, FALSE, INFINITE);

			if (is_thread_terminated(args)) {
				finished = true;
			}
			else {
				continue_thread_work(args);
			}
		}
	} while (!finished);

	reset_elements_of_array_with_thread_index(args);

	return 0;
}

void initialize_threads_with(marker_argument_st* args, HANDLE h_start_single_event, HANDLE* h_stop_events,
	HANDLE* h_thread, int number_of_markers, int* arr, int arr_size, DWORD* thread_base)
{
	for (int i = 0; i < number_of_markers; i++)
	{
		h_stop_events[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		args[i] = get_marker_argument_of(i, arr, arr_size, h_start_single_event, h_stop_events[i]);

		for (int j = 0; j < NUM_OF_EVENTS; j++)
		{
			args[i].h_event[j] = CreateEvent(NULL, TRUE, FALSE, NULL);
		}

		h_thread[i] = CreateThread(NULL, 0, thread_mark_job, (void*)&args[i], 0, &thread_base[i]);
	}

}

//returns true if success
bool success_terminate_thread(ThreadStatus* terminated_threads, marker_argument_st* args, HANDLE* hThread, int threadIndexToTerminate)
{
	if (terminated_threads[threadIndexToTerminate] == ThreadStatus::INITIALIZED)
	{
		SetEvent(args[threadIndexToTerminate].h_event[1]);
		WaitForSingleObject(hThread[threadIndexToTerminate], INFINITE);

		terminated_threads[threadIndexToTerminate] = ThreadStatus::TERMINATED;

		return true;
	}

	return false;
}

void continue_thread_job(marker_argument_st* args, int number_of_markers, ThreadStatus* terminated_threads)
{
	for (int i = 0; i < number_of_markers; i++)
	{
		if (terminated_threads[i] == ThreadStatus::INITIALIZED)
		{
			ResetEvent(args[i].h_finish_event);
			SetEvent(args[i].h_event[0]);
		}
	}
}

void close_connection_with_threads(HANDLE* h_threads, int arr_size)
{
	for (int i = 0; i < arr_size; i++)
	{
		if (h_threads[i]) {
			CloseHandle(h_threads[i]);
		}
	}
}
