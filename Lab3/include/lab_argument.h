#ifndef LAB_ARGUMENT_H
#define LAB_ARGUMENT_H

struct minmaxarg_t 
{
	int* arr = nullptr;
	int arr_size, min, max, min_i, max_i;
};

struct averagearg_t
{
	int* arr = nullptr;
	int arr_size, average;
};

#endif //!LAB_ARGUMENT_H