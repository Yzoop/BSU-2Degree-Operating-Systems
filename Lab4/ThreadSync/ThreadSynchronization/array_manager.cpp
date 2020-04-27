#include <iostream>
#include "array_manager.h"

void print_array(const int* arr_to_print, int arr_size) {
	int space_bound_id = arr_size - 1;

	for (int i = 0; i < space_bound_id; i++) {
		std::cout << arr_to_print[i] << " ";
	}
	std::cout << arr_to_print[space_bound_id];

	std::cout << std::endl;
}
