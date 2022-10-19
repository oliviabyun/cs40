/*   
        uarray2.h
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Interface that supports a two-dimensional unboxed array.
*/

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED
#include "uarray.h"

#define T UArray2_T
typedef struct T *T;

/* 
*  name: UArray2_T struct
*  description: Houses the data structures that comprise the 2D array and the 
*               variables containing key information about the 2D array
*/
struct T {
        UArray_T main_arr;      /* UArray that will contain UArrays */
        int width;              /* length of main_arr */
        int height;             /* length of each sub-UArray in main_arr */
        int size;               /* size of each element in the 2D array */
};

/*
* name:         UArray2_new
* description:  Creating an instance of 2D unboxed array
* parameters:   int width and int height dimensions of 2D array to be 
                initialized, size of element in bytes
* returns:      Instance of 2D array (UArray2_T)
* logical expectations: A UArray2_T should have a width and height that are both
                        greater than 0, and the size of each element should also
                        be greater than 0.
*/
T UArray2_new(int width, int height, int size);

/*
* name: Array2_free     
* description: Frees each element contained in the 2D array before freeing
*              heap-allocated memory associated with the 2D array itself.
* parameters:  pointer to a 2D array (UArray2_T)
* returns:     none
* logical expectations: the UArray2_T should not be null
*/
void UArray2_free(T *u2array);

/*
* name:         UArray2_width
* description:  Returning the number of elements in main_arr
* parameters:   2D array struct (UArray2_T) instance
* returns:      integer width of the 2D array (UArray2_T)
* logical expectations: A UArray2_T should not be null
*/
int UArray2_width(T u2array);

/*
* name:         UArray2_height
* description:  Returning the number of elements in each
*               subarray of the 2D array 
* parameters:   2D array struct (UArray2_T) instance
* returns:      integer height of the 2D array (UArray2_T u2array)
* logical expectations: the UArray2_T should not be null
*/
int UArray2_height(T u2array);

/*
* name:         UArray2_size
* description:  Returning the size of 2D array (UArray2_T)
* parameters:   2D array struct (UArray2_T) instance
* returns:      integer size of the 2D array (UArray2_T u2array)
* logical expectations: the UArray2_T should not be null
*/
int UArray2_size(T u2array);

/*
* name: UArray2_at
* description: Locates and returns a pointer to an element at a specified
               index (given row and column)
* parameters:  2D array struct (UArray2_T), integer column and row indices of 
               desired element
* returns:     void pointer to specified element in 2D array
* logical expectations: the 2D array is not null. The row and col should be in
                        bounds: row is less than u2array's height and col should
                        be less than u2array's width; they should both be 
                        greater than or equal to zero.
*/
void *UArray2_at(T u2array, int col, int row);

/*
* name:        UArray2_map_col_major       
* description: Calls an apply function for each element in the array; row
               indices vary more rapidly than column indices.
* parameters:  2D array struct (UArray2_T), specified apply function (void), 
                and closure (void pointer)
* returns:     none
* logical expectations: the UArray2_T should not be null
*/
void UArray2_map_col_major(T u2array, 
                void apply(int col, int row, T u2array, 
                        void *p1, void *p2), void *cl);

/*
* name:         UArray2_map_row_major
* description:  Calls an apply function for each element in the array; column
                indices vary more rapidly than row indices.
* parameters:   2D array struct (UArray2_T), specified apply function (void), 
                and closure (void pointer)
* returns:      none
* logical expectations: the UArray2_T should not be null
*/
void UArray2_map_row_major(T u2array, 
                 void apply(int col, int row, T u2array, 
                        void *p1, void *p2), void *cl);

#undef T
#endif
