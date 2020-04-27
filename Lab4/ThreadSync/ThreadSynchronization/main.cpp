#include <ctime>
#include <iostream>
#include <Windows.h>
#include "marker_arguments.h"
#include "array_manager.h"
#define MESSAGE_FOR_ARRAY_SIZE "Please, enter an integer array size (>0): "
#define MESSAGE_FOR_NUMBER_OF_MARKERS "Please, enter an integer number of marker threads (>0): "
#define MESSAGE_FOR_THREAD_ID_TO_TERMINATE "Enter the index of thread which you want to terminate (> 0): "
#define INT_EMPTY_VALUE 0
#define THREAD_STATUS_INIT ThreadStatus::INITIALIZED

enum class ThreadStatus {
	INITIALIZED,
	TERMINATED
};

// a variable for explicit highlighting critical sections in the program
CRITICAL_SECTION critical_section;

void reset_elements_of_array_with_thread_index(marker_argument_st* args);

int get_int_from_console(const char* message_for_console = "");

bool is_thread_terminated(marker_argument_st* args);

void continue_thread_work(marker_argument_st* args);

DWORD WINAPI thread_mark_job(LPVOID param);

void init_threads(marker_argument_st* args, HANDLE hBeginWorkEvent, 
				  HANDLE* hStopWorkEvents, HANDLE* hThreads, 
				  int number_of_markers, int* mas, int n, DWORD* thread_base);

bool success_terminate_thread(ThreadStatus* terminated_threads, marker_argument_st* args,
					  HANDLE* hThread, int threadIndexToTerminate);

void continue_thread_job(marker_argument_st* args, int number_of_markers, ThreadStatus* terminated_threads);

void start_terminating_by_console(int* arr, int arr_size, int num_of_markers, HANDLE* handler_for_finish_job,
								 ThreadStatus* terminated_threads, marker_argument_st* args, HANDLE* handler_for_thread,
								 bool print_arr_to_console);

void close_connection_with_threads(HANDLE* handler_for_thread, int arr_size);

int main()
{
	int arr_size, num_of_marker_threads;

	//read appropriate data from user input (console)
	arr_size = get_int_from_console(MESSAGE_FOR_ARRAY_SIZE);
	num_of_marker_threads = get_int_from_console(MESSAGE_FOR_NUMBER_OF_MARKERS);

	int* arr = new int[arr_size];
	
	// allocate memory for special handlers for thread job
	HANDLE* handler_for_thread = new HANDLE[num_of_marker_threads];
	HANDLE* handler_for_finish_job = new HANDLE[num_of_marker_threads];
	HANDLE h_event = CreateEvent(NULL, TRUE, FALSE, NULL);
	
	DWORD* thread_base = new DWORD[num_of_marker_threads];
	marker_argument_st* args = new marker_argument_st[num_of_marker_threads];
	ThreadStatus* terminated_threads = new ThreadStatus[num_of_marker_threads];
	
	// next part of array management: filling arrays with default values
	fill_arr_with<int>(arr, arr_size, INT_EMPTY_VALUE);
	fill_arr_with<ThreadStatus>(terminated_threads, num_of_marker_threads, ThreadStatus::INITIALIZED);
	
	InitializeCriticalSection(&critical_section);
	{
		// critical section block start
		init_threads(args, h_event, handler_for_finish_job, handler_for_thread, num_of_marker_threads, arr, arr_size, thread_base);

		if (h_event) {
			SetEvent(h_event);

			start_terminating_by_console(arr, arr_size, num_of_marker_threads, handler_for_finish_job, terminated_threads, args, handler_for_thread, true);
			close_connection_with_threads(handler_for_thread, arr_size);
		
			std::cout << "Marker threads have succsesfully finished the job\n";
		}
		else {
			std::cout << "Ooops!... Something went wrong while initializing marker threads.";
		}
		// finish critical section block
	}
	DeleteCriticalSection(&critical_section);

	// delete all allocated memory
	delete[] handler_for_finish_job;
	delete[] handler_for_thread;
	delete[] terminated_threads;
	delete[] arr;
	delete[] thread_base;

	return 0;
}

void reset_elements_of_array_with_thread_index(marker_argument_st* args)
{
	EnterCriticalSection(&critical_section);

	for (int i = 0; i < args->arr_size; i++)
	{
		if (args->arr[i] == (args->id + 1)) {
			args->arr[i] = 0;
		}
	}

	LeaveCriticalSection(&critical_section);
}

int get_int_from_console(const char* message_for_console)
{
	int var_from_console;
	std::cout << message_for_console;
	std::cin >> var_from_console;

	return var_from_console;
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

		if (args->arr[random_index] == 0)
		{
			EnterCriticalSection(&critical_section);

			Sleep(5);
			args->arr[random_index] = args->id + 1;

			LeaveCriticalSection(&critical_section);

			editsCount++;
			Sleep(5);
		}

		else {
			EnterCriticalSection(&critical_section);

			std::cout << "\nMarker thread #" << args->id + 1 << "; editsCount = " << editsCount
				<< "; elements, Array value at [" << random_index + 1 << "]\n";

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

void init_threads(marker_argument_st* args, HANDLE h_start_single_event, HANDLE* h_stop_events,
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
		if (SetEvent(args[threadIndexToTerminate].h_event[1])) {
			WaitForSingleObject(hThread[threadIndexToTerminate], INFINITE);

			terminated_threads[threadIndexToTerminate] = ThreadStatus::TERMINATED;
			return true;
		}
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

void start_terminating_by_console(int* arr, int arr_size, int num_of_markers, HANDLE* handler_for_finish_job, 
								  ThreadStatus* terminated_threads, marker_argument_st* args, HANDLE* handler_for_thread,
								  bool print_arr_to_console)
{
	for (int i = 0; i < num_of_markers && i < arr_size; i++)
	{
		WaitForMultipleObjects(num_of_markers, handler_for_finish_job, TRUE, INFINITE);

		if (print_arr_to_console) {
			print_array(arr, arr_size);
		}

		//bool thread_terminated = false;
		int thread_id;
		bool read_correct_thread_id;

		do {
			thread_id = get_int_from_console(MESSAGE_FOR_THREAD_ID_TO_TERMINATE);
			if (thread_id < num_of_markers && thread_id > 0) {
				read_correct_thread_id = true;
			}
			else {
				std::cout << "Wrong thread id!\n";
				if (print_arr_to_console) {
					print_array(arr, arr_size);
				}
				read_correct_thread_id = false;
			}
		} while (!read_correct_thread_id);

		--thread_id;

		std::cout << "Ok.\n";
		if (success_terminate_thread(terminated_threads, args, handler_for_thread, thread_id)) {
			std::cout << "Successfully terminated thread #" << thread_id << "\n";
		}
		
		if (print_arr_to_console) {
			print_array(arr, arr_size);
		}

		continue_thread_job(args, num_of_markers, terminated_threads);
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
