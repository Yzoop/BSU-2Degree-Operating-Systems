#ifndef ARRAY_MANAGER_H
#define ARRAY_MANAGER_H


void print_array(const int* arr_to_print, int arr_size);

template <class FILL_TYPE>
void fill_arr_with(FILL_TYPE* arr, int arr_size, FILL_TYPE fill_value)
{
	for (int i = 0; i < arr_size; i++) {
		arr[i] = fill_value;
	}
}


#endif // !ARRAY_MANAGER_H
