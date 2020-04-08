#ifndef AVERAGE_H
#define AVERAGE_H
#include "lab_argument.h"

void average(void* averagearg);

averagearg_t* get_new_average_structure(int* arr, int arr_size, int averages);

#endif //!AVERAGE_THR_H