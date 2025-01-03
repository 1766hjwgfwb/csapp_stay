/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "headers/common.h"
#include "headers/algorithm.h"


array_t *array_construct(int size) {
    // init
    array_t *arr = (array_t *)malloc(sizeof(array_t));
    arr->count = 0;
    arr->size = size;
    arr->table = (uint64_t *)malloc(size * sizeof(uint64_t));
    return arr;
}


void array_free(array_t *arr) {
    free(arr->table);
    free(arr);
}


int array_insert(array_t **arr, uint64_t val) {
    array_t *tmp_arr = *arr;

    if (tmp_arr == NULL)
        return 0;


    if (tmp_arr->count == tmp_arr->size) {
        // malloc to twice size
        uint64_t *old_table = tmp_arr->table;

        tmp_arr->size *= 2;
        tmp_arr->table = (uint64_t *)malloc(tmp_arr->size * sizeof(uint64_t));

        // copy old data to new table
        for (int i = 0; i < tmp_arr->count; i++) {
            tmp_arr->table[i] = old_table[i];
        }

        free(old_table);
    }

    

    // insert new value
    tmp_arr->table[tmp_arr->count] = val;
    // increase count
    tmp_arr->count++;

    return 1;
}


int array_delete(array_t *arr, int index) {
    if (arr == NULL || index < 0 || index > arr->count)
        return 0;

    /*// if 1/3 of size, merge with half size
    if (arr->count <= arr->size / 3) {
        // merge with half size
        uint64_t *old_table = arr->table;
        arr->size /= 3;
        arr->table = (uint64_t *)malloc(arr->size * sizeof(uint64_t));

        // delete element
        if (index != arr->count - 1) {
            // will delete element move to left
            for (int i = index; i < arr->count - 1; i++) {
                arr->table[i] = old_table[i + 1];
            }
        }
        // 直接拷贝前面部分的元素
        memcpy(arr->table, old_table, (arr->count - 1) * sizeof(uint64_t));

        free(old_table);
        arr->count--;
        return 1;
    } else {
        // delete element
        for (int i = index; i < arr->count; i++) {      // all element shift left
            // element forward after index
            arr->table[i - 1] = arr->table[i];
        }

        arr->count--;
        return 1;
    }*/
    
    for (int i = index; i < arr->count; i++) {
        arr->table[i - 1] = arr->table[i];
    }

    arr->count--;

    if ((arr->count <= arr->size / 4) && arr->count > 0) {
        printf("shrink array\n");
        arr->size /= 2;
        uint64_t *new_table = (uint64_t *)realloc(arr->table, arr->size * sizeof(uint64_t));

        arr->table = new_table;
    }

    return 1;
}


int array_get(array_t *arr, int index, uint64_t *valptr) {
    if (index > arr->count)
        return 0;   // out of range
    else {
        *valptr = arr->table[index];
        return 1;
    }
}


void print_array(array_t *arr) {
    if (arr == NULL)
        return;

    for (int i = 0; i < arr->count; i++) {
        printf("%ld ", arr->table[i]);
    }

    printf("\n");
}
