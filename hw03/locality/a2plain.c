/*
 *     a2plain.c
 *     by Olivia Byun (obyun01) and Alyssa Williams (awilli36)
 *     10/13/2022
 *     HW3: locality
 *
 *     Summary purpose: 
 *     Implements A2Methods with a regular UArray2. 
 *
 *     Use: 
 *     Used for modularity purposes so that you can make an A2Methods for 
 *     either a blocked or regular UArray2, and call functions that work for 
 *     both. 
 *
 */


#include <string.h>
#include <a2plain.h>
#include "uarray2.h"
#include <assert.h>

/************************************************/
/* Define a private version of each function in */
/* A2Methods_T that we implement.               */
/************************************************/

/*
 *        name: new
 * description: creates a new instance of a plain 2D array (of type UArray2)
 *              given dimensions and size
 *  parameters:  width - integer representing the number of columns in the
 *                       2D array
 *              height - integer representing the number of rows in the 2D
 *                       array
 *                size - integer representing the size of each element in the
 *                       2D array, in bytes
 *     returns: a new A2Methods_UArray2 instance (plain 2D array)   
 *      errors: throws a checked runtime error if width, height, or size are
 *              less than or equal to zero
 */
static A2Methods_UArray2 new(int width, int height, int size)
{
        assert (width > 0 && height > 0 && size > 0);
        return UArray2_new(width, height, size);
}

/*
 *        name: new_with_blocksize
 * description: creates a new instance of a plain 2D array (of type UArray2);
 *              ignores blocksize since a plain 2D array does not require one,
 *              but still takes one in for A2Methods abstraction purposes.
 *  parameters: width - integer representing the number of columns in the
 *                       2D array
 *              height - integer representing the number of rows in the 2D
 *                       array
 *                size - integer representing the size of each element in the
 *                       2D array, in bytes
 *           blocksize - the size per block (in this case, 1, since plain
 *                       2D arrays have a blocksize of 1)
 *     returns: a new A2Methods_UArray2 instance (plain 2D array)
 *      errors: throws a checked runtime error if width, height, or size are
 *              less than or equal to zero
 */
static A2Methods_UArray2 new_with_blocksize(int width, int height, int size, 
                                            int blocksize)
{
        assert (width > 0 && height > 0 && size > 0);
        (void) blocksize;
        return UArray2_new(width, height, size);
}

/*
 *        name: a2free
 * description: frees all heap-allocated memory associated with the 2D array
 *  parameters: *array2 - pointer to an A2Methods_UArray2 instance (plain 2D
 *              array) to be freed
 *     returns: none
 *      errors: throws a checked runtime error if array2 is NULL
 */
static void a2free(A2Methods_UArray2 *array2)
{
        assert (array2 != NULL);
        UArray2_free((UArray2_T *) array2);
} 

/*
 *        name: width
 * description: returns the width (number of columns) in a given plain 2D array
 *  parameters: array2 - an A2Methods_UArray2 instance (plain 2D array)
 *     returns: the width (number of columns) in array2
 *      errors: throws a checked runtime error if array2 is NULL 
 */
static int width(A2Methods_UArray2 array2)
{
        assert (array2 != NULL);
        return UArray2_width(array2);
}

/*
 *        name: height
 * description: returns the height (number of rows) in a given plain 2D array
 *  parameters: array2 - an A2Methods_UArray2 instance (plain 2D array)
 *     returns: the height (number of columns) in array2
 *      errors: throws a checked runtime error if array2 is NULL 
 */
static int height(A2Methods_UArray2 array2)
{
        assert (array2 != NULL);
        return UArray2_height(array2);
}

/*
 *        name: size
 * description: returns the size (bytes per element) in a given plain 2D array
 *  parameters: array2 - an A2Methods_UArray2 instance (plain 2D array)
 *     returns: the size (number of bytes per element) in array2
 *      errors: throws a checked runtime error if array2 is NULL 
 */
static int size(A2Methods_UArray2 array2)
{
        assert (array2 != NULL);
        return UArray2_size(array2);
}

/*
 *        name: blocksize
 * description: returns the size of a block in a 2D array
 *  parameters: array2 - an A2Methods_UArray2 instance (plain 2D array)
 *     returns: 1 (since UArray2 is plain, not blocked - each "block is 
 *              1 element")
 *      errors: throws a checked runtime error if array2 is NULL 
 */
static int blocksize(A2Methods_UArray2 array2)
{
        (void)array2;
        return 1;
}

/*
 *        name: at
 * description: finds and returns the element at a given index in a plain 2D 
                array
 *  parameters: array2 - an A2Methods_UArray2 instance (plain 2D array)
 *                 col - the column index to retrieve the element
 *                 row - the row index to retrieve the element
 *     returns: the element at the specified col, row index of array2
 *      errors: throws a checked runtime error if array2 is NULL or if col or
 *              row are out of bounds
 */
static A2Methods_Object *at(A2Methods_UArray2 array2, int col, int row)
{
        assert (array2 != NULL);
        assert (col < width(array2) && row < height(array2));
        return UArray2_at(array2, col, row);
}

/*
 *        name: map_row_major
 * description: uses the built in UArray2 map row major function to loop
 *              through the 2D array function in row major order to call the
 *              apply function on each element of the array; column indices 
 *              vary faster than row indices
 *  parameters: uarray2 - an A2Methods_UArray2 instance (plain 2D array)
 *                apply - a given apply function to call on each element of
 *                        the 2D array
 *                  *cl - a void pointer for the client to use to pass through
 *                        context-specific data/information
 *     returns: none
 *      errors: throws a checked runtime error if uarray2 is NULL or if the
 *              apply function is NULL
 */
static void map_row_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        assert (uarray2 != NULL);
        assert (apply != NULL);
        UArray2_map_row_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/*
 *        name: map_col_major
 * description: uses the built in UArray2 map col major function to loop
 *              through the 2D array function in col major order to call the
 *              apply function on each element of the array; row indices 
 *              vary faster than column indices
 *  parameters: uarray2 - an A2Methods_UArray2 instance (plain 2D array)
 *                apply - a given apply function to call on each element of
 *                        the 2D array
 *                  *cl - a void pointer for the client to use to pass through
 *                        context-specific data/information
 *     returns: none
 *      errors: throws a checked runtime error if uarray2 is NULL or if the
 *              apply function is NULL
 */
static void map_col_major(A2Methods_UArray2 uarray2,
                          A2Methods_applyfun apply,
                          void *cl)
{
        assert (uarray2 != NULL && apply != NULL);
        UArray2_map_col_major(uarray2, (UArray2_applyfun*)apply, cl);
}

/*
 *  small closure struct containing both a closure (void pointer) for client's
 *  use and a pointer to a small apply function       
 */
struct small_closure {
        A2Methods_smallapplyfun *apply; 
        void                    *cl;
};

/*
 *        name: apply_small
 * description: an apply function to perform a specified operation on the 2D
 *              array with few parameters
 *  parameters:     col - the column index of the current element
 *                  row - the row index of the current element
 *              uarray2 - an A2Methods_UArray2 instance (plain 2D array)
 *                 elem - a void pointer to the current element at (col, row)
 *                        in uarray2
 *                  vcl - a closure for the client's use (void pointer)
 *     returns: none
 *      errors: throws a checked runtime error if elem is NULL
 */
static void apply_small(int col, int row, UArray2_T uarray2,
                        void *elem, void *vcl)
{
        assert (elem != NULL);
        struct small_closure *cl = vcl;
        (void)col;
        (void)row;
        (void)uarray2;
        cl->apply(elem, cl->cl);
}

/*
 *        name: small_map_row_major
 * description: uses the built in UArray2 map row major function to loop
 *              through the 2D array function in row major order to call the
 *              small apply function on each element of the array; column
 *              indices vary faster than row indices
 *  parameters:      a2 - an A2Methods_UArray2 instance (plain 2D array)
 *        smallapplyfun - a given small apply function to call on each element
 *                        of the 2D array
 *                  *cl - a void pointer for the client to use to pass through
 *                        context-specific data/information
 *     returns: none
 *      errors: throws a checked runtime error if a2 is NULL or if the
 *              apply function is NULL
 */
static void small_map_row_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        assert (a2 != NULL && apply != NULL);
        struct small_closure mycl = { apply, cl };
        UArray2_map_row_major(a2, apply_small, &mycl);
}

/*
 *        name: small_map_col_major
 * description: uses the built in UArray2 map col major function to loop
 *              through the 2D array function in col major order to call the
 *              small apply function on each element of the array; row indices 
 *              vary faster than column indices
 *  parameters:      a2 - an A2Methods_UArray2 instance (plain 2D array)
 *        smallapplyfun - a given small apply function to call on each element
 *                        of the 2D array
 *                  *cl - a void pointer for the client to use to pass through
 *                        context-specific data/information
 *     returns: none
 *      errors: throws a checked runtime error if a2 is NULL or if the
 *              apply function is NULL
 */
static void small_map_col_major(A2Methods_UArray2        a2,
                                A2Methods_smallapplyfun  apply,
                                void *cl)
{
        assert (a2 != NULL && apply != NULL);
        struct small_closure mycl = { apply, cl };
        UArray2_map_col_major(a2, apply_small, &mycl);
}

/*
 * Virtual table that contains all of A2Methods' functions. Some methods are 
 * NULL because they are only for blocked arrays
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
        NULL, /* map_block_major */
        map_col_major, /*map_default */
        small_map_row_major,
        small_map_col_major,
        NULL, /* small_map_block_major */
        small_map_col_major,
};

/* exported pointer to the struct */
A2Methods_T uarray2_methods_plain = &uarray2_methods_plain_struct;
