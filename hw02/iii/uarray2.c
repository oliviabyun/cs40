/*   
        uarray2.c
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Implementation of UArray2, which is a 2-dimensional unboxed
                 array data structure.
*/

#include <stdio.h>
#include <stdlib.h>
#include <uarray2.h>
#include <assert.h>

#define T UArray2_T

/*
* name:         UArray2_new
* description:  Creating an instance of UArray2_T type, which is comprised
                of creating our main UArray of width # of elements and 
                iterating through it in order to create each sub-UArray 
                of height # of elements.
* parameters:   int width and int height dimensions of 2D array to be 
                initialized, size of element in bytes
* returns:      UArray2_T type array
*/
T UArray2_new(int width, int height, int size) {
        assert(width > 0 && height > 0 && size > 0);
        T u2array = malloc(sizeof(struct UArray2_T));
        assert(u2array != NULL);
        
        /* Initializing the contents of struct */
        u2array->width = width;
        u2array->height = height;
        u2array->size = size;
        u2array->main_arr = UArray_new(width, sizeof(UArray_T));
        
        /* Creating a new subarray for each element of the main array */
        for (int i = 0; i < width; i++) {
                *(UArray_T *) UArray_at(u2array->main_arr, i) = 
                                UArray_new(height, size);
        }
        return u2array;
}

/*
* name: Array2_free     
* description: Frees each element contained in main_arr before freeing
*              heap-allocated memory associated with main_arr itself.
* parameters:  pointer to a 2D array (UArray2_T)
* returns:     none
*/
void UArray2_free(T *u2array) {
        assert(u2array != NULL);

        /* Iterate through the main array  */
        for (int i = 0; i < (*u2array)->width; i++) {
                /* Free each corresponding sub-array in the main array */
                UArray_T *curr = 
                        (UArray_T *) UArray_at((*u2array)->main_arr, i);
                UArray_free(curr);
        }
        /* Free the main array and struct */
        UArray_free(&((*u2array)->main_arr));
        free(*u2array);
}

/*
* name:         UArray2_width
* description:  Returning the width of the UArray2 as an integer type
* parameters:   UArray2_T type array called u2array
* returns:      width of u2array
*/
int UArray2_width(T u2array) {
        assert(u2array != NULL);
        return u2array->width;
}

/*
* name:         UArray2_height
* description:  Returning the height of the UArray2 as an integer type
* parameters:   2D array struct (UArray2_T) instance called u2array
* returns:      height of u2array
*/
int UArray2_height(T u2array) {
        assert(u2array != NULL);
        return u2array->height;
}

/*
* name:         UArray2_size
* description:  Returning the size of the UArray2 as an integer type
* parameters:   2D array struct (UArray2_T) instance called u2array
* returns:      size of u2array
*/
int UArray2_size(T u2array) {
        assert(u2array != NULL);
        return u2array->size;
}


/*
* name: UArray2_at
* description: Locates and returns a pointer to an element at a specified
               index (given row and column)
* parameters:  2D array struct (UArray2_T), integer column and row indices of 
               desired element
* returns:     pointer to specified element in 2D array
*/
void *UArray2_at(T u2array, int col, int row) {
        assert(u2array != NULL);
        /* col must be in bounds: between 0 and width - 1 */
        assert(col >= 0 && col < (u2array->width));
        /* row must be in bounds: between 0 and height - 1 */
        assert(row >= 0 && row < (u2array->height));
        
        UArray_T *target;
        /* Iterate through main_arr until reaching the correct column index */
        for (int i = 0; i <= col; i++) {
                /* Iterate through UArray until finding correct row index */
                for (int j = 0; j <= row; j++) {
                        if (i == col && j == row) {
                                /* Retrieve pointer to element */
                                UArray_T *curr = 
                                        UArray_at(u2array->main_arr, i);
                                target = (UArray_T *)UArray_at(*curr, j);
                        }
                }
        }
       return target;
}

/*
* name:        UArray2_map_col_major       
* description: Calls an apply function for each element in the array; row
               indices vary more rapidly than col indices.
* parameters:  2D array struct (UArray2_T), specified apply function (void), 
                and closure (void pointer)
* returns:     none
*/
void UArray2_map_col_major(T u2array, 
                void apply(int col, int row, T u2array, 
                        void *p, void *cl), void *cl) {
        assert(u2array != NULL);
        /* Iterate through each row */
        for (int i = 0; i < u2array->width; i++) {
                /* Iterate through each column */
                for (int j = 0; j < u2array->height; j++) {
                        UArray_T *curr_ptr = 
                                (UArray_T *) UArray2_at(u2array, i, j);
                        /* Call apply on each element */
                        apply(i, j, u2array, curr_ptr, cl); 
                }
        }
}

/*
* name:         UArray2_map_row_major
* description:  Calls an apply function for each element in the array; col
                indices vary more rapidly than row indices.
* parameters:   2D array struct (UArray2_T), specified apply function (void), 
                and closure (void pointer)
* returns:      none
*/
void UArray2_map_row_major(T u2array, 
                void apply(int col, int row, T u2array, 
                        void *p, void *cl), void *cl) {
        assert(u2array != NULL);
        /* Iterate through each column */
        for (int i = 0; i < u2array->height; i++) {
                /* Iterate through each row */
                for (int j = 0; j < u2array->width; j++) {
                        UArray_T *curr_ptr = 
                                (UArray_T *) UArray2_at(u2array, j, i);
                        apply(j, i, u2array, curr_ptr, cl);
                }
        }
}