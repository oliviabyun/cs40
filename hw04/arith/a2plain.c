/*
 *     a2plain.c
 *     by Helena Lowe (hlowe01) & Jiyoon Choi (jchoi13)
 *     locality
 *
 *     This is the implementation of a exporting a pointer to 
        the struct that holds the methods suite for UArray2.
 */

#include <string.h>
#include <a2plain.h>
#include "uarray2.h"

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/* 
 * name:    new
 * purpose: allocates space for 2D array on the heap, creates a new 2D array
 * inputs:  width which is the number of elements in one row of the 2D array
 *          height which is the number of elements in one col of the 2D array
 *          size which is the number of bytes in each element of uarray2
 * outputs: pointer to A2Methods_UArray2 struct
 * CREs:    checked in respective uarray2 function
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
        return UArray2_new(width, height, size);
}

/* 
 * name:    new_with_blocksize
 * purpose: allocates space for 2D array on the heap, creates a 
                new 2D array that uses blocksize
 * inputs:  width which is the number of elements in one row of the 2D array
 *          height which is the number of elements in one col of the 2D array
 *          size which is the number of bytes in each element of uarray2
 *          blocksize which is the number of elements in 1 side of a block
 * outputs: pointer to A2Methods_UArray2 struct
 * CREs:    checked in respective uarray2 function
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height, 
                                        int size, int blocksize)
{
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/* 
 * name:    a2free
 * purpose: deallocates and clears *array2p
 * inputs:  a pointer to A2Methods_UArray2 struct
 * outputs: none
 * CREs:    checked in respective uarray2 function
 */
static void a2free(A2Methods_UArray2 *array2p)
{
	UArray2_free((UArray2_T *) array2p);
}

/* 
 * name:    width
 * purpose: gets the width of the 2D uarray called array2
 * inputs:  a pointer to A2Methods_UArray2 struct
 * outputs: the width of the given 2D uarray
 * CREs:    checked in respective uarray2 function
 */
static int width(A2Methods_UArray2 array2)
{
	return UArray2_width(array2);
}

/* 
 * name:    height
 * purpose: gets the height of the 2D uarray called array2
 * inputs:  a pointer to A2Methods_UArray2 struct
 * outputs: the height of the given 2D uarray
 * CREs:    checked in respective uarray2 function
 */
static int height(A2Methods_UArray2 array2)
{
	return UArray2_height(array2);
}

/* 
 * name:    size
 * purpose: gets the size in bytes of each element in the 2D uarray 
 * inputs:  a pointer to A2Methods_UArray2 struct
 * outputs: integer that is the size (number of bytes) of each element in the
            given 2D uarray.
 * CREs:    checked in respective uarray2 function
 */
static int size(A2Methods_UArray2 array2)
{
	return UArray2_size(array2);
}

/* 
 * name:    blocksize TODO check this with other TA
 * purpose: get the blocksize of the 2D array called array2
 * inputs:  a pointer to A2Methods_UArray2 struct
 * outputs: 1 because uarray2 has no blocking, so each TODO
 * CREs:    checked in respective uarray2 function
 */
static int blocksize(A2Methods_UArray2 array2)
{
        (void)array2;
	return 1;
}

/* 
 * name:    at
 * purpose: get the element at the specified indices in the given uarray2
 * inputs:  a pointer to A2Methods_UArray2 struct, column index and
 *          row index of integer type
 * outputs: element of type A2Methods_Object at given indices in the 2D array
 * CREs:    checked in respective uarray2 function
 */
static A2Methods_Object *at(A2Methods_UArray2 array2, int i, int j)
{
	return UArray2_at(array2, i, j);
}

/* defining UArray2_applyfun function */
typedef void UArray2_applyfun(int i, int j, UArray2_T uarray2, 
                                                void *elem, void *cl);

/* 
 * name:    map_row_major
 * purpose: apply given apply function to each element in the given 2D uarray
 *          in row major order
 * inputs:  a pointer to A2Methods_UArray2 struct
 *          function that applies to every element in 2D uarray
 *          the accumulator
 * outputs: none
 * CREs:    checked in respective uarray2 function
 */
static void map_row_major(A2Methods_UArray2 uarray2,
                        A2Methods_applyfun apply,
                        void *cl)
{
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/* 
 * name:    map_col_major
 * purpose: apply given apply function to each element in the given 2D uarray
 *          in column major order
 * inputs:  a pointer to A2Methods_UArray2 struct
 *          function that applies to every element in 2D uarray
 *          the accumulator
 * outputs: none
 * CREs:    checked in respective uarray2 function
 */
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/* name: small_closure
 * purpose: houses the properties necessary for a small applyfun function
 * members: A2Methods_smallapplyfun pointer to variable called apply,
                void pointer to a closure element
 */
struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/* 
 * name:    apply_small
 * purpose: apply given small apply function to each element in 
                the given 2D uarray
 * inputs:  column and row indices of integer type, 
                2D array of UArray2_T type, void pointer
                to element at specified index, void
                pointer to closure element called vcl
 * outputs: none
 * CREs:    none
 */
static void apply_small(int i, int j, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        struct small_closure *cl = vcl;
        (void)i;
        (void)j;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/* 
 * name:    small_map_row_major
 * purpose: apply given small apply function to each element in 
                the given 2D uarray in row_major mapping order
 * inputs:  2D array of UArray2_T type, A2Methods_smallapplyfun 
                variable called apply, void pointer to closure 
                element called cl
 * outputs: none
 * CREs:    checked in respective uarray2 function
 */
static void small_map_row_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/* 
 * name:    small_map_col_major
 * purpose: apply given small apply function to each element in 
                the given 2D uarray in col_major mapping order
 * inputs:  2D array of UArray2_T type, A2Methods_smallapplyfun 
                variable called apply, void pointer to closure 
                element called cl
 * outputs: none
 * CREs:    checked in respective uarray2 function
 */
static void small_map_col_major(A2Methods_UArray2 a2,
                                A2Methods_smallapplyfun apply,
                                void *cl)
{
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/* name: uarray2_methods_plain_struct
 * purpose: houses the methods suite of A2Methods_T type
 * members: methods necessary for initializing, manipulating,
                and freeing the uarray2
 */
static struct A2Methods_T uarray2_methods_plain_struct = {
        new,
        new_with_blocksize,
        a2free,
	width,
	height,
	size,
	blocksize,
	at,
	map_row_major,			
	map_col_major,			
	NULL,                   /* map_block_major */
	map_col_major,	        /* map_default */
	small_map_row_major,	
	small_map_col_major,	
	NULL,                   /* small_map_block_major */ 
	small_map_col_major,	/* small_map_default */
};

// finally the payoff: here is the exported pointer to the struct
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
