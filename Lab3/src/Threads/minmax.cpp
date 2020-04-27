#include "lab_argument.h"
#include <iostream>
#include <Windows.h>
#include <process.h>
#include "Threads\minmax.h"
#define SLEEP_MINMAX_TIME 7

void min_max(void* minmaxarg)
{
    minmaxarg_t* arg = (minmaxarg_t*)minmaxarg;
    int max = -INT_MAX;
    int min = INT_MAX;

    for (int i = 0; i < arg->arr_size; i++)
    {
        if (max < arg->arr[i]) {
            max = arg->arr[i];
            arg->maxId = i;
        }
        if (min > arg->arr[i]) {
            min = arg->arr[i];
            arg->minId = i;
        }
        Sleep(SLEEP_MINMAX_TIME);
    }
    int min_global = min;
    int max_global = max;
    std::cout << "Thread min_max: \n";
    std::cout << "\tMax: " << max << "\n\tMin: " << min << "\n";
    abort();
}


minmaxarg_t* get_new_minmax_structure(int* arr, int arr_size, int min, int max, int minid, int maxid)
{
    minmaxarg_t *args = new minmaxarg_t;
    args->arr = arr;
    args->arr_size = arr_size;
    args->min = min;
    args->max = max;
    args->minId = minid;
    args->maxId = maxid;

    return args;
}