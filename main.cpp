#include "stdio.h"
#include "stdlib.h"

#define NOTATION_NUMBER     10
#define COMPARATOR_MORE     1
#define COMPARATOR_LESS     -1
#define COMPARATOR_EQUALS   0



int get_size_from_console();

void save_array(int *arr, int arr_size);

void sort_array(int *arr, int arr_size);

void print_array(const int *arr, int arr_size);

int get_sum_of_digits(int number);

void save_qsort_parray(struct Pair* parray, int parr_size, int (*pair_comparator)(const void*, const void*));

int pair_comparator(const void* p1, const void* p2);



struct Pair{
    int first;
    int second;
};


int main(int argc, char const *argv[]) {

    int n;
    int *arr;

    n = get_size_from_console();

    arr = malloc(n * sizeof(int));
    save_array(arr, n);
    sort_array(arr, n);
    print_array(arr, n);

    free(arr);

    return 0;
}



int get_sum_of_digits(int number){
    int sum_of_digits = 0;
    while (number > 0){
        sum_of_digits += number % NOTATION_NUMBER;
        number /= NOTATION_NUMBER;
    }
    return sum_of_digits;
}


int pair_comparator(const void* p1, const void* p2){
    int sum1 = ((struct Pair *)p1)->second;
    int sum2 = ((struct Pair *)p2)->second;

    if (sum1 > sum2){
        return COMPARATOR_MORE;
    }
    else if (sum1 < sum2){
        return COMPARATOR_LESS;
    }
    else{
        return COMPARATOR_EQUALS;
    }
}


//main task function for sorting
void sort_array(int *arr, int arr_size){
    struct Pair* arr_with_sum_numbers = malloc(arr_size* sizeof(struct Pair));

    // save *arr elements as firsts in struct Pair*
    // and save sum of (arr[i]) element as second in struct Pair[i]

    //get array of saved numbers and sum of their digits
    for(int i = 0; i < arr_size; i++){
        struct Pair cur_pair = {.first = arr[i], .second = get_sum_of_digits(arr[i])};
        arr_with_sum_numbers[i] = cur_pair;
    }

    // main sorting step
    qsort((void*)arr_with_sum_numbers,
                arr_size,
                sizeof(arr_with_sum_numbers[0]),
                pair_comparator);

    //save changes after being sorted
    for(int i = 0; i < arr_size; i++){
        arr[i] = arr_with_sum_numbers[i].first;
    }

    free(arr_with_sum_numbers);
}


void print_array(const int* arr, int arr_size){
    printf("Sorted array:\n");
    for (int i = 0; i < arr_size; i++){
        printf("%d\n", arr[i]);
    }
}


void save_array(int *arr, int arr_size){
        for(int i = 0; i < arr_size; i++) {
            printf("Please, enter %d-th number: ", i);
            scanf("%d", &arr[i]);
        }
}


int get_size_from_console(){
    int array_size;

    printf("Please, enter an array size: ");
    scanf("%d", &array_size);

    return array_size;
}
