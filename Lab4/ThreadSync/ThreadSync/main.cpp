#include <iostream>
#include <Windows.h>
//#include <vector>
#include <string>
#include "ThreadArguments.h"

#define ARRAY_EMPTY_VALUE 0
#define MESSAGE_FOR_ARRAY_SIZE "Please, enter array size (an integer value >0): "
#define MESSAGE_FOR_NUM_OF_MARKER_THREADS "Please, enter an integer number of marker threads (> 0): "

enum class ThreadStatuses {
    TERMINATED,
    INITIALIZED
};

bool is_thread_terminated(ThreadArguments* parameters);
int get_integer_from_console(std::string message_to_user = "");
template <typename ARR_TYPE>
ARR_TYPE* get_new_filled_array(int arr_size, ARR_TYPE fill_value);
DWORD WINAPI marker(LPVOID param);

void create_threads(ThreadArguments* arguments, HANDLE hBeginWorkEVent,
                    HANDLE* hStopWorkEvents,
                    HANDLE* hThread, int num_of_markers,
                    int arr_size, int* arr, DWORD* IDThread);
void delete_array(int* array_to_delete);
void print_array(int* mas, int n);
void continue_thread_work(ThreadArguments* parameters);
void reset_elements_of_array_with_thread_index(ThreadArguments* parameters);
template <typename FILL_VALUE_TYPE>
void full_fill_array(FILL_VALUE_TYPE* target_array, int target_array_size, FILL_VALUE_TYPE fill_value);
void terminate_thread(ThreadStatuses* terminated_threads, ThreadArguments* parameters, HANDLE* hThread, int threadIndexToTerminate, bool& is_thread_deleted);
void signal_to_continue_work_threads(ThreadArguments* parameters, int number_of_markers, ThreadStatuses* terminated_threads);


CRITICAL_SECTION critical_section;

int main()
{
    int arr_size, num_of_marker_threads;
    int* arr;
    HANDLE hBeginWorkEvent;
    HANDLE* hStopWorkEvents;
    HANDLE* hThread;
    DWORD* IDTHread;
    ThreadArguments* parameters;
    ThreadStatuses* terminated_threads;
    // open critical section block with initializing
    InitializeCriticalSection(&critical_section);

    // ask the user for an integer which sets an array size value
    arr_size = get_integer_from_console(MESSAGE_FOR_ARRAY_SIZE);
    
    arr = get_new_filled_array<int>(arr_size, ARRAY_EMPTY_VALUE);

    // ask the user for number of marker threads to create
    num_of_marker_threads = get_integer_from_console(MESSAGE_FOR_NUM_OF_MARKER_THREADS);

    // allocate memory in vectors
    hThread = new HANDLE[num_of_marker_threads];
    hStopWorkEvents = get_new_filled_array<HANDLE>(arr_size, CreateEvent(NULL, TRUE, FALSE, NULL));
    parameters = new ThreadArguments[num_of_marker_threads];
    terminated_threads = get_new_filled_array<ThreadStatuses>(arr_size, ThreadStatuses::INITIALIZED);
    IDTHread = new DWORD[num_of_marker_threads];

    hBeginWorkEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    create_threads(parameters, hBeginWorkEvent, hStopWorkEvents, hThread, num_of_marker_threads, arr_size, arr, IDTHread);

    if (hBeginWorkEvent)
        SetEvent(hBeginWorkEvent);
    else
        return 0;

    for (int i = 0; i < num_of_marker_threads && i < arr_size; i++)
    {
        WaitForMultipleObjects(num_of_marker_threads, hStopWorkEvents, TRUE, INFINITE);
        print_array(arr, arr_size);

        bool is_thread_deleted = false;

        while (!is_thread_deleted)
        {
            int threadIndexToTerminate;

            std::cout << "\nEnter the index of thread which you want to terminate (it starts from 1): ";
            std::cin >> threadIndexToTerminate;
            threadIndexToTerminate--;

            if (threadIndexToTerminate >= num_of_marker_threads || threadIndexToTerminate < 0)
            {
                std::cout << "This thread isn`t exist";
                continue;
            }
            terminate_thread(terminated_threads, parameters, hThread, threadIndexToTerminate, is_thread_deleted);
        }
        print_array(arr, arr_size);

        signal_to_continue_work_threads(parameters, num_of_marker_threads, terminated_threads);
    }

    std::cout << "\nAll threads finished work\n";

    for (int i = 0; i < num_of_marker_threads; i++)
    {
        if (hThread[i])
            CloseHandle(hThread[i]);
    }



    delete_array(arr);
    delete[] (hStopWorkEvents);
    delete[](hThread);
    delete[](IDTHread);
    delete[](parameters);
    delete[](terminated_threads);
    DeleteCriticalSection(&critical_section);
}

void terminate_thread(ThreadStatuses* terminated_threads, ThreadArguments* parameters, HANDLE* hThread, int threadIndexToTerminate, bool& is_thread_deleted)
{
    if (terminated_threads[threadIndexToTerminate] == ThreadStatuses::INITIALIZED)
    {
        SetEvent(parameters[threadIndexToTerminate].hContinueOrTerminateWork[1]);
        WaitForSingleObject(hThread[threadIndexToTerminate], INFINITE);

        is_thread_deleted = true;
        terminated_threads[threadIndexToTerminate] = ThreadStatuses::TERMINATED;
    }
    else
        std::cout << "This thread has already finished his work.";
}

void signal_to_continue_work_threads(ThreadArguments* parameters, int number_of_markers, ThreadStatuses* terminated_threads)
{
    for (int i = 0; i < number_of_markers; i++)
    {
        if (terminated_threads[i] == ThreadStatuses::INITIALIZED)
        {
            ResetEvent(parameters[i].hStopWorkEvent);
            SetEvent(parameters[i].hContinueOrTerminateWork[0]);
        }
    }
}

void print_array(int* mas, int n) {
    std::cout << "Array : \n";

    for (int i = 0; i < n; i++) {
        std::cout << mas[i] << " ";
    }

    std::cout << "\n";
}

int get_integer_from_console(std::string message_to_user)
{
    int array_size;
    std::cout << message_to_user;
    std::cin >> array_size;

    return array_size;
}

template <typename ARR_TYPE>
ARR_TYPE* get_new_filled_array(int arr_size, ARR_TYPE fill_value)
{
    ARR_TYPE* new_filled_array = new ARR_TYPE[arr_size];

    full_fill_array<ARR_TYPE>(new_filled_array, arr_size, fill_value);

    return new_filled_array;
}

DWORD WINAPI marker(LPVOID param)
{
    ThreadArguments* parameters = (ThreadArguments*)param;
    int editsCount = 0;
    bool endWork = false;
    srand(parameters->index);

    WaitForSingleObject(parameters->hBeginWorkEvent, INFINITE); // Expect a signal from main.

    while (!endWork)
    {
        int random_index = rand() % parameters->n;
        EnterCriticalSection(&critical_section);

        if (parameters->arr[random_index] == 0)
        {
            Sleep(5);
            parameters->arr[random_index] = parameters->index + 1;
            LeaveCriticalSection(&critical_section);

            editsCount++;
            Sleep(5);
        }

        else
        {
            std::cout << "\nThread " << parameters->index + 1 << " marked total " << editsCount;
            std::cout << " elements, can`t mark element on " << random_index + 1 << " index\n";

            LeaveCriticalSection(&critical_section);

            // Wait for a response to continue or exit from the thread.
            SetEvent(parameters->hStopWorkEvent);
            WaitForMultipleObjects(2, parameters->hContinueOrTerminateWork, FALSE, INFINITE);

            if (is_thread_terminated(parameters))
                endWork = true;
            else
                continue_thread_work(parameters);
        }
    }

    reset_elements_of_array_with_thread_index(parameters);

    return 0;
}

void reset_elements_of_array_with_thread_index(ThreadArguments* parameters)
{
    EnterCriticalSection(&critical_section);

    for (int i = 0; i < parameters->n; i++)
    {
        if (parameters->arr[i] == (parameters->index + 1))
            parameters->arr[i] = 0;
    }

    LeaveCriticalSection(&critical_section);
}

bool is_thread_terminated(ThreadArguments* parameters)
{
    return WaitForSingleObject(parameters->hContinueOrTerminateWork[1], 0) == WAIT_OBJECT_0;
}

void continue_thread_work(ThreadArguments* parameters)
{
    ResetEvent(parameters->hContinueOrTerminateWork[0]);
}


void create_threads(ThreadArguments* arguments, HANDLE hBeginWorkEVent, 
                    HANDLE* hStopWorkEvents, 
                    HANDLE* hThread, int num_of_markers, 
                    int arr_size, int* arr, DWORD* IDThread)
{
    for (int i = 0; i < num_of_markers; i++) {
        HANDLE* pair_of_works = new HANDLE[2];
        pair_of_works[1] = pair_of_works[0] = CreateEvent(NULL, TRUE, FALSE, NULL);
        arguments[i] = get_thread_arguments(arr_size, arr, i, hBeginWorkEVent, hStopWorkEvents[i], pair_of_works);
 
        hThread[i] = CreateThread(NULL, 0, marker, (void*)&arguments[i], 0, &IDThread[i]);
    }
}

void delete_array(int* array_to_delete)
{
    delete[] array_to_delete;
}

template <typename FILL_VALUE_TYPE>
void full_fill_array(FILL_VALUE_TYPE* target_array, int target_array_size, FILL_VALUE_TYPE fill_value)
{
    for (int i = 0; i < target_array_size; i++) {
        target_array[i] = fill_value;
    }
}
