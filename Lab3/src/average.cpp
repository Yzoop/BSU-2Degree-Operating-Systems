#include "average.h"
#include <process.h>
#include <Windows.h>
#include <iostream>
#define SLEEP_TASK_TIME 12

void average(void* averagearg)
{
    averagearg_t* arg = (averagearg_t*)averagearg;

    float count = 0;
    float sum = 0;
    for (int i = 0; i < arg->arr_size; i++) {
        sum += arg->arr[i];
        count++;
        Sleep(SLEEP_TASK_TIME);
    }
    arg->average = sum / count;
    std::cout << "Thread average:\n";
    std::cout << "\t" << arg->average << "\n";
    
}


averagearg_t* get_new_average_structure(int* arr, int arr_size, int averages)
{
    averagearg_t* average_structure = new averagearg_t;

    average_structure->arr = arr;
    average_structure->arr_size = arr_size;
    average_structure->average = averages;

    return average_structure;
}