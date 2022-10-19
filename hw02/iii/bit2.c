/*   
        bit2.c
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Implementation of Bit2, which is a 2-dimensional unboxed
                 array of bits.
*/

#include <stdio.h>
#include <stdlib.h>
#include <bit2.h>
#include <assert.h>

#define T Bit2_T

/* 
 * Bit2_T struct
 * Purpose: Houses elements that overall represent a 2D bit array
 */
struct T {
        UArray_T main_arr;      /* UArray containing Bit vectors */
        int width;              /* width of 2D array */
        int height;             /* height of 2D array */
};

/*
* name:         Bit2_new
* description:  Initializes 2D bit array struct (Bit2_T). Initializes the
                dimensions of the 2D array (elements width and height in the
                struct) to the values passed in. Initializes a new UArray_T of
                Bit_T vectors
* parameters:   width (int) and height (int) to set the dimensions of the 2D
                array (Bit2_T)
* returns:      the newly initialized Bit2_T 2D array
*/
T Bit2_new(int width, int height) {
        assert(width > 0 && height > 0);
        T bit2array = malloc(sizeof(struct Bit2_T));
        assert(bit2array != NULL);
        
        /* Initializing the contents of struct */
        bit2array->width = width;
        bit2array->height = height;
        bit2array->main_arr = UArray_new(width, sizeof(Bit_T));
        
        /* Creating a new subarray for each element of the main array */
        for (int i = 0; i < width; i++) {
                *(Bit_T *) UArray_at(bit2array->main_arr, i) = 
                                Bit_new(height);
        }
        
        return bit2array;
}

/*
* name:         Bit2_free
* description:  frees all heap-allocated memory associated with each Bit vector
                within the UArray main_arr then frees the main_arr and overall
                Bit2_T bit2array struct
* parameters:   pointer to Bit2_T bit2array struct
* returns:      none
*/
void Bit2_free(T *bit2array) {
        assert(bit2array != NULL);
        
        for (int i = 0; i < (*bit2array)->width; i++) {
                /* free each bit vector */
                Bit_T *curr = (Bit_T *) UArray_at((*bit2array)->main_arr, i);
                Bit_free(curr);
        }
        
        UArray_free(&((*bit2array)->main_arr)); /* free UArray main_arr */
        free(*bit2array); /* free the bit2array struct */
}

/*
* name:         Bit2_width 
* description:  given a Bit2_T 2D array, returns its width (aka length of the
                UArray main_arr)
* parameters:   a Bit2_T bit2array struct
* returns:      integer width of the Bit2_T 2D array
*/
int Bit2_width(T bit2array) {
        assert(bit2array != NULL);
        return bit2array->width;
}

/*
* name:         Bit2_height
* description:  given a Bit2_T 2D array, returns its height (aka length of any
                of the Bit vectors contained in the UArray main_arr)
* parameters:   a Bit2_T bit2array struct
* returns:      integer height of the Bit2_T 2D array
*/
int Bit2_height(T bit2array) {
        assert(bit2array != NULL);
        return bit2array->height;
}

/*
* name:         Bit2_get
* description:  given a column index and row index, finds and returns the value
                of the Bit in that location of the Bit2_T 2D array
* parameters:   a 2D bit array of type Bit2_T, a specific column index (int),
                and row index (int).
* returns:      the int value of the bit at the given location in the Bit2_T
                2D array
*/
int Bit2_get(T bit2array, int col, int row) {
        assert(bit2array != NULL);
        /*col must be in bounds: between 0 and width - 1 */
        assert(col >= 0 && col < bit2array->width);
        /*row must be in bounds: between 0 and height - 1 */
        assert(row >= 0 && row < bit2array->height);

        /* get Bit vector at given column index */
        Bit_T *curr_bit_vector = (Bit_T *) UArray_at(bit2array->main_arr, col);
        
        /* get value of element at given row index of Bit vector */
        int target = Bit_get(*curr_bit_vector, row);
        return target;
}

/*
* name:         Bit2_put
* description:  puts the passed in bit value (int) into a specified
                column index and row index into the passed in 2D array (Bit2_T)
* parameters:   a 2D bit array of type Bit2_T, a specific column index (int),
                and row index (int).
* returns:      the previous bit value (int) that existed at the passed 
                in column and row indices (if a pre-existing value existed)
*/
int Bit2_put(T bit2array, int col, int row, int bit) {
        assert(bit2array != NULL);
        /* col must be in bounds: between 0 and width - 1 */
        assert(col >= 0 && col < bit2array->width);
        /* row must be in bounds: between 0 and height - 1 */
        assert(row >= 0 && row < bit2array->height);   

        /* get bit vector at the specified column, which is UArray index */
        Bit_T *curr_bit_vector = (Bit_T *) UArray_at(bit2array->main_arr, col);
        int prev = Bit_put(*curr_bit_vector, row, bit);
        
        /* return previous value of specified bit, if existent */
        return prev;
}

/*
* name:         Bit2_map_col_major  
* description:  iterates through the 2D array (Bit2_T) in column major order
                and calls the passed in apply function which applies its
                specified logic on every single element of the Bit2_T array.
* parameters:   a 2D array (Bit2_T), an apply function (void),
                and a closure variable (void pointer)
* returns:      none
*/
void Bit2_map_col_major(T bit2array, 
                void apply(int col, int row, T bit2array, 
                        int bit, void *cl), void *cl) {
        assert(bit2array != NULL);
        assert(cl != NULL);
        /* iterate through each col */
        for (int i = 0; i < bit2array->width; i++) {
                /* iterate through each row */
                for (int j = 0; j < bit2array->height; j++) {
                        /* get the bit vector at the specified indices */
                        int curr = Bit2_get(bit2array, i, j);
                        /* call given apply function */
                        apply(i, j, bit2array, curr, cl);
                }
        }  
}

/*
* name:         Bit2_map_row_major
* description:  iterates through the 2D array (Bit2_T) in row major order
                and calls the passed in apply function which applies its
                specified logic on every single element of the Bit2_T array.
* parameters:   a 2D array (Bit2_T), an apply function (void),
                and a closure variable (void pointer)
* returns:      none
*/
void Bit2_map_row_major(T bit2array, 
                void apply(int col, int row, T bit2array, 
                        int bit, void *cl), void *cl) {
        assert(bit2array != NULL);
        assert(cl != NULL);
        /* iterate through each row */
        for (int i = 0; i < bit2array->height; i++) {
                /* iterate through each col */
                for (int j = 0; j < bit2array->width; j++) {
                         /* get the bit vector at the specified indices */
                        int curr = Bit2_get(bit2array, j, i);
                        /* call given apply function */
                        apply(j, i, bit2array, curr, cl);
                }
        }  
}