#pragma once


#include "labArgument.h"

void min_max(void* minmaxarg);

minmaxarg_t* get_new_minmax_structure(int* arr, int arr_size, int min, int max, int minid, int maxid);