/*
 *     uarray2.h
 *     by Helena Lowe (hlowe01) & Zoe Gu (zgu03), 9/25/2022
 *     iii
 *
 *     This is the interface for uarray2 where the client can see what can be
 *     done: a new 2D uarray can be created; the width, height, size, element
 *     of the 2D uarray can be found; memory for the 2D urray can be 
 *     deallocated, and different apply functions can be mapped to each 
 *     element.
 */

#ifndef UARRAY2_H_
#define UARRAY2_H_

#include <stdlib.h>
#include <string.h>
#include "uarray.h"
#include "uarrayrep.h"
#include "mem.h"

#define T UArray2_T
typedef struct T *T; 

/* 
 * name:    UArray2_new
 * purpose: allocates space for uarray2
 * inputs:  width which is the length of uarray2
 *            height which is the length of the uarrays made in every element of
 *            uarray2
 *            size which is the number of bytes in each element of uarray2
 * outputs: pointer to UArray2_T struct
 * CREs:    if width is less than 0, if height is less than 0, if size is 
 *          nonpositive
 */
extern T UArray2_new(int width, int height, int size);

/* 
 * name:    UArray2_free
 * purpose: deallocates and clears *uarray2
 * inputs:  a 2D uarray
 * outputs: none
 * CREs:    check if supplied argument is NULL
 */
extern void UArray2_free(T *uarray2);

/* 
 * name:    UArray2_width
 * purpose: gets the width of the 2D uarray
 * inputs:  a 2D uarray
 * outputs: integer that is the width of the 2D uarray.
 * CREs:    check if supplied argument is NULL.
 */
extern int UArray2_width(T uarray2);

/* 
 * name:    UArray2_height
 * purpose: get the height of the given 2D uarray.
 * inputs:  a 2D uarray.
 * outputs: integer that is the height of the 2D uarray.
 * CREs:    check if supplied argument is NULL.
 */
extern int UArray2_height(T uarray2);

/* 
 * name:    UArray2_size
 * purpose: gets the number of bytes of a element in the given 2D uarray.
 * inputs:  a 2D uarray.
 * outputs: integer that is the size (number of bytes) of each element in the
 *          2D uarray.
 * CREs:    check if supplied argument is NULL.
*/
extern int UArray2_size(T uarray2);

/* 
 * name:    UArray2_at
 * purpose: get the element at the specified indices in the given uarray2.
 * inputs:  a 2D uarray, a row index, a column index.
 * outputs: pointer to element at specified indicies.
 * CREs:    check if supplied arguments are NULL, check if row and col are < 0.
 */
extern void *UArray2_at(T uarray2, int col, int row);

/* 
 * name:    UArray2_map_row_major
 * purpose: call the apply function on every element in the 2D uarray in row
 *          major order.
 * inputs:  a 2D uarray, function that applies to every element in 2D
 *          uarray, accumulator.
 * outputs: none
 * CREs:    check if supplied arguments are NULL.
 */
extern void UArray2_map_row_major(T uarray2,
      void apply(int col, int row, UArray2_T a, void *elem, void *accum), 
      void *cl);

/* 
 * name:    UArray2_map_col_major
 * purpose: call the apply function on every element in the 2D uarray in column
 *          major order.
 * inputs:  a 2D uarray, function that applies to every element in 2D
 *          uarray, accumulator.
 * outputs: none
 * CREs:    check if supplied arguments are NULL.
 */
extern void UArray2_map_col_major(T uarray2,
      void apply(int col, int row, UArray2_T a, void *elem, void *accum), 
      void *cl);

#undef T
#endif
