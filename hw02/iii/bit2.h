/*   
        bit2.h
        HW 2, CS 40 (Fall 2022)
        Authors: Olivia Byun (obyun01), Jiyoon Choi (jchoi13)
        Purpose: Interface that supports a two-dimensional array of bits.
*/

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED
#include "bit.h"
#include "uarray.h"

#define T Bit2_T
typedef struct T *T;

/*
* name:         Bit2_new
* description:  initializes a new 2D bit array (type Bit2_T) given a width
                and height
* parameters:   width (int) and height (int) of the 2D bit array to be
*               initialized
* returns:      A new 2D bit array (type Bit2_T)
* logical expectations: width and height are greater than zero; mallocing
                        bit2array should be successful; aborts if assertions
                        are false
*/
T Bit2_new(int width, int height);

/*
* name:         Bit2_free
* description:  frees all heap-allocated memory associated with the 2D bit
                array
* parameters:   the 2D bit array to be freed (of type Bit2_T)
* returns:      none
* logical expectations: aborts if bit2array is null
*/
void Bit2_free(T *bit2array);

/*
* name:         Bit2_width
* description:  returns width of the specified 2D bit array
* parameters:   a 2D bit array of type Bit2_T
* returns:      the integer width of the 2D bit array
* logical expectations: aborts if bit2array is null
*/
int Bit2_width(T bit2array);

/*
* name:         Bit2_height
* description:  returns height of the specified 2D bit array
* parameters:   a 2D bit array of type Bit2_T
* returns:      the integer width of the 2D bit array
* logical expectations: aborts if bit2array is null
*/
int Bit2_height(T bit2array);

/*
* name:         Bit2_get
* description:  gets a bit value at a specified index of the 2D bit array
* parameters:   2D bit array (type Bit2_T), a specified column index (int) and
                a specified row index (int).
* returns:      the bit value at index (col, row) of the 2D bit array
* logical expectations: we assert that the passed in Bit2_T array is not null 
                        and that the column index and row index are within 
                        valid bounds.
*/
int Bit2_get(T bit2array, int col, int row);

/*
* name:         Bit2_put
* description:  puts a bit value at a specified index of the 2D bit array
* parameters:   a 2D bit array of type Bit2_T, a specific column index (int),
                and row index (int).
* returns:      the previous value of the Bit in the specified index, if
                existent
* logical expectations: we assert that the passed in Bit2_T array is
                not null and that the column index and row
                index are within valid bounds.
*/
int Bit2_put(T bit2array, int col, int row, int bit);

/*
* name:         Bit2_map_col_major
* description:  iterates through the 2D bit array in column major order and 
                calls a specified apply function on each element
* parameters:   a 2D bit array of type Bit2_T, an apply function and a closure 
                (void pointer) that is freely available for the client to pass 
                information through.
* returns:      none
* logical expectations: the Bit2_T array and void pointer closure are not null. 
*/
void Bit2_map_col_major(T bit2array, 
                void apply(int col, int row, T bit2array, 
                        int bit, void *cl), void *cl);

/*
* name:         Bit2_map_row_major
* description:  iterates through the 2D bit array in row major order and 
                calls a specified apply function on each element
* parameters:   a 2D bit array of type Bit2_T, an apply function and a closure 
                variable (void pointer) that is freely available for the client 
                to pass information through.
* returns:      none
* logical expectations: the Bit2_T array and closure are not null.
*/
void Bit2_map_row_major(T bit2array, 
                void apply(int col, int row, T bit2array, 
                        int bit, void *c1), void *cl);

#undef T
#endif
