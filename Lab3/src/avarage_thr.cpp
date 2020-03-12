#include "avarage_thr.h"
#include <process.h>
#include <Windows.h>
#include <iostream>

void avarage(void* avaragearg)
{
    avaragearg_t* arg = (avaragearg_t*)avaragearg;

    float count = 0;
    float sum = 0;
    for (int i = 0; i < arg->arr_size; i++) {
        sum += arg->arr[i];
        count++;
        Sleep(12);
    }
    arg->avarage = sum / count;
    std::cout << "Thread average:\n";
    std::cout << "\t" << arg->avarage << "\n";
    _endthread();
}


avaragearg_t* get_new_avarage_structure(int* arr, int arr_size, int avarages)
{
    avaragearg_t* avarage_structure = new avaragearg_t;

    avarage_structure->arr = arr;
    avarage_structure->arr_size = arr_size;
    avarage_structure->avarage = avarages;

    return avarage_structure;
}