//#define _CRT_SECURE_NO_WARNINGS
#include <process.h>
#include <Windows.h>
#include <iostream>
#include "labArgument.h"
#include "minmax_thr.h"
#include "avarage_thr.h"


using namespace std;


void read_size_to(int& size);
void read_array_to(int* to_arr, int arr_size);
void begin_minmax_thread(const minmaxarg_t* minmaxarg);
void begin_avarge_thread(const avaragearg_t* avaragearg);
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

    avaragearg_t* avaragearg = get_new_avarage_structure(minmaxarg->arr, array_size, 0);
    begin_avarge_thread(avaragearg);

    minmaxarg->arr[minmaxarg->minId] = avaragearg->avarage;
    minmaxarg->arr[minmaxarg->maxId] = avaragearg->avarage;


    cout << "Result: \n";
    for (int i = 0; i < array_size; i++) {
        cout << minmaxarg->arr[i] << " ";
    }
    cout << endl;
    
    delete minmaxarg;
    delete avaragearg;
    delete[] arr;
}


void read_size_to(int& size)
{
    do {
        cout << "Please enter array size: ";
        cin >> size;
    } while (!array_size_correct(size));
}



void read_array_to(int* to_arr, int size)
{
    cout << "Please enter array elements: \n";
    for (int i = 0; i < size; i++) {
        cout << "Reading " << i + 1 << "/" << size << " element: ";
        cin >> to_arr[i];
    }
}


void begin_minmax_thread(const minmaxarg_t* minmaxarg)
{
    cout << "Starting minmax thread.\nPlease, wait...";
    HANDLE h_minmax = (HANDLE)_beginthread(min_max, 0, (void*)minmaxarg);
    WaitForSingleObject(h_minmax, INFINITE);
    cout << "\n------------------------\nMinmax thread has finished.\n";

}


void begin_avarge_thread(const avaragearg_t* avaragearg)
{
    cout << "Starting avarage thread.\nPlease, wait...";
    HANDLE h_avarage = (HANDLE)_beginthread(avarage, 0, (void*)avaragearg);
    WaitForSingleObject(h_avarage, INFINITE);
    cout << "\n------------------------\nAvarage thread has finished.\n";
}


bool array_size_correct(int size)
{
    return size > 0;
}
