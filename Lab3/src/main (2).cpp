#include <process.h>
#include <Windows.h>
#include <iostream>
#include "lab_argument.h"
#include "minmax.h"
#include "average.h"

void read_size_to(int& size);
void read_array_to(int* to_arr, int arr_size);
void begin_minmax_thread(const minmaxarg_t* minmaxarg);
void begin_average_thread(const averagearg_t* averagearg);
bool array_size_correct(int size);

int main()
{
    int array_size = 0;
    int* arr;

    read_size_to(array_size);
    arr = new int[array_size];
    read_array_to(arr, array_size);

    minmaxarg_t* minmaxarg = get_new_minmax_structure(arr, array_size, 0, 0, 0, 0);
    begin_minmax_thread(minmaxarg);

    averagearg_t* averagearg = get_new_average_structure(minmaxarg->arr, array_size, 0);
    begin_average_thread(averagearg);

    minmaxarg->arr[minmaxarg->min_i] = averagearg->average;
    minmaxarg->arr[minmaxarg->max_i] = averagearg->average;


    std::cout << "Result: \n";
    for (int i = 0; i < array_size; i++) {
        std::cout << minmaxarg->arr[i] << " ";
    }
    std::cout << std::endl;

    delete minmaxarg;
    delete averagearg;
    delete[] arr;
}


void read_size_to(int& size)
{
    do {
        std::cout << "Please enter array size: ";
        std::cin >> size;
    } while (!array_size_correct(size));
}



void read_array_to(int* to_arr, int size)
{
    std::cout << "Please enter array elements: \n";
    for (int i = 0; i < size; i++) {
        std::cout << "Reading " << i + 1 << "/" << size << " element: ";
        std::cin >> to_arr[i];
    }
}

void begin_minmax_thread(const minmaxarg_t* minmaxarg)
{
    DWORD d_thread;
    std::cout << "Starting minmax thread.\nPlease, wait...";
    HANDLE h_minmax = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)min_max, (void*)minmaxarg, 0, &d_thread);
    if (!h_minmax) {
        std::cout << "Error while creating minmax thread: " << GetLastError();
    }
    if (WaitForSingleObject(h_minmax, INFINITE) == WAIT_FAILED)
    { 
        std::cout << "Error while waiting minmax thread: " << GetLastError();
    }
    std::cout << "\n------------------------\nMinmax thread has finished.\n";

    CloseHandle(h_minmax);
}


void begin_average_thread(const averagearg_t* averagearg)
{
    DWORD d_thread;
    std::cout << "Starting average thread.\nPlease, wait...";
    HANDLE h_avarage = (HANDLE)CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)average, (void*)averagearg, 0, &d_thread);
    if (!h_avarage) {
        std::cout << "Error while creating avarage thread: " << GetLastError();
    }
    if (WaitForSingleObject(h_avarage, INFINITE) == WAIT_FAILED)
    {
        std::cout << "Error while waiting minmax thread: " << GetLastError();
    }
    std::cout << "\n------------------------\naverage thread has finished.\n";
 
    CloseHandle(h_avarage);
}


bool array_size_correct(int size)
{
    return size > 0;
}
