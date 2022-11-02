/*
 *     uarray2.c
 *     by Helena Lowe & Zoe Gu, 9/25/2022
 *     iii
 *
 *     This is the implementation for creating a 2D UArray. Each 2D UArray is
 *     A Hanson UArray of UArrays. It creates, frees memory associated with,
 *     get the width, gets the height, gets the number of bytes of each 
 *     element, gets a pointer to a specific element in, and maps apply 
 *     functions to elements in the 2D UArray.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uarray.h"
#include "uarrayrep.h"
#include "uarray2.h"
#include "mem.h"
#include "assert.h"

#define T UArray2_T

/* 
 * purpose: store all information related to a UArray2
 * members: width- number of elements in row of uarray, at least 0
 *          height- number of elements in col of uarray, at least 0
 *          size- number of bytes in one element
 *          uarray- uarray of uarrays
 */
struct T {
        int width;
        int height;
        int size; 
        UArray_T parent; 
};

/* 
 * name:    UArray2_new
 * purpose: allocates space for uarray2
 * inputs:  width which is the length of uarray2
 *          height which is the length of the uarrays made in every element of
 *          uarray2
 *          size which is the number of bytes in each element of uarray2
 * outputs: pointer to UArray2_T struct
 * CREs:    if width is less than 0, if height is less than 0, if size is 
 *          nonpositive
 */
extern T UArray2_new(int width, int height, int size) 
{       
        assert(width >= 0);
        assert(height >= 0); 
        assert(size > 0);
        
        T uarray2 = malloc(sizeof(struct T));
        assert(uarray2 != NULL);

        UArray_T parent_array = UArray_new(width, sizeof(UArray_T));

        /* creating 2D array */
        for (int i = 0; i < width; i++) {
                *(UArray_T *)(UArray_at(parent_array, i)) = 
                UArray_new(height, size);
        }

        uarray2->width = width;
        uarray2->height = height;
        uarray2->size = size;
        uarray2->parent = parent_array;
        
        return uarray2;
}

/* 
 * name:    UArray2_free
 * purpose: deallocates and clears *uarray2
 * inputs:  a pointer to UArray2_T struct
 * outputs: none
 * CREs:    check if supplied argument is NULL
 */
extern void UArray2_free(T *uarray2) 
{
        assert(uarray2 != NULL);

        /* free child arrays */
        for (int i = 0; i < (*uarray2)->width; i++) {
                UArray_free(UArray_at((*uarray2)->parent, i));
        }
        
        /* free parent array and uarray2 struct */
        UArray_free(&((*uarray2)->parent));
        free(*uarray2);
}

/* 
 * name:    UArray2_width
 * purpose: gets the width of the 2D uarray
 * inputs:  a 2D uarray
 * outputs: integer that is the width of the 2D uarray.
 * CREs:    check if supplied argument is NULL.
 */
extern int UArray2_width(T uarray2) 
{
        assert(uarray2 != NULL);
        return uarray2->width;
}

/* 
 * name:    UArray2_height
 * purpose: get the height of the given 2D uarray.
 * inputs:  a 2D uarray.
 * outputs: integer that is the height of the 2D uarray.
 * CREs:    check if supplied argument is NULL.
 */
extern int UArray2_height(T uarray2) 
{       
        assert(uarray2 != NULL);
        return uarray2->height;
}

/* 
 * name:    UArray2_size
 * purpose: gets the number of bytes of a element in the given 2D uarray.
 * inputs:  a 2D uarray.
 * outputs: integer that is the size (number of bytes) of each element in the
            2D uarray.
 * CREs:    check if supplied argument is NULL.
*/
extern int UArray2_size(T uarray2) 
{       
        assert(uarray2 != NULL);
        return uarray2->size;
}

/* 
 * name:    UArray2_at
 * purpose: get the element at the specified indices in the given uarray2
 * inputs:  a 2D uarray, a row index, a column index
 * outputs: pointer to element at specified indicies
 * CREs:    check if supplied argument is NULL, check if row and col are < 0
 */
extern void *UArray2_at(T uarray2, int col, int row) 
{       
        assert(uarray2 != NULL);
        assert(col < uarray2->width && col >= 0);
        assert(row < uarray2->height && row >= 0);

        UArray_T* child_array = UArray_at(uarray2->parent, col);
        return UArray_at(*child_array, row);
}

/* 
 * name:    UArray2_map_row_major
 * purpose: call the apply function on every element in the 2D uarray in row
 *          major order
 * inputs:  a 2D uarray, function that applies to every element in 2D
 *          uarray, accumulator
 * outputs: none
 * CREs:    check if supplied arguments are NULL
 */
extern void UArray2_map_row_major(T uarray2,
      void apply(int col, int row, UArray2_T a, void *elem, void *accum), 
      void *cl) 
{
        assert(uarray2 != NULL);
        
        /* loop through elements in uarray2 in row major order */
        for (int row = 0; row < uarray2->height; row++) {
                for (int col = 0; col < uarray2->width; col++) {
                        apply(col, row, uarray2, UArray2_at(uarray2, col, row),
                                cl);
                }
        }   
}

/* 
 * name:    UArray2_map_col_major
 * purpose: call the apply function on every element in the 2D uarray in column
 *          major order
 * inputs:  a 2D uarray, function that applies to every element in 2D
 *          uarray, accumulator
 * outputs: none
 * CREs:    check if supplied argument are NULL
 */
extern void UArray2_map_col_major(T uarray2,
      void apply(int col, int row, UArray2_T a, void *elem, void *accum), 
      void *cl) 
{
        assert(uarray2 != NULL);

        /* loop through elements in uarray2 in column major order */
        for (int col = 0; col < uarray2->width; col++) {
                for (int row = 0; row < uarray2->height; row++) {
                        apply(col, row, uarray2, UArray2_at(uarray2, col, row),
                                cl);
                }
        }   
}

#undef T