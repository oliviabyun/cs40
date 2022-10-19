/*
 *     uarray2b.c
 *     by Olivia Byun (obyun01) and Alyssa Williams (awilli36)
 *     10/13/2022
 *     HW3: locality
 *
 *     Summary purpose: 
 *     UArray2b represents an blocked two-dimensional array. It makes an array
 *     with blocksize, where the blocksize is the width of each block. A block
 *     stores a group of element, where the elements are stored next to each 
 *     other in memory. The UArray2b is divided into these blocks, where the 
 *     user can specify the blocksize or choose a default blocksize. UArray2b
 *     can store any type of element in a two-dimensional array of any size 
 *
 *     Use: 
 *     This program is useful for creating a blocked array, which can have 
 *     better cache times for certain programs.     
 */

#include "uarray2b.h"
#include "uarray2.h"
#include <uarray.h>
#include <stdio.h>
#include <mem.h>
#include <assert.h>
#include <math.h>

#define T UArray2b_T

/* 
 *  struct
 *  Purpose: Houses elements that overall represent a 2D blocked array
 */
struct T {
        UArray2_T main_arr;      /* UArray containing Bit vectors */
        int width;              /* width of 2D array in blocks */
        int height;             /* height of 2D array in blocks */
        int size;           
        int blocksize;
        int blocks_wide;
        int blocks_high;          
};

typedef struct apply_cl {
        //void apply(int col, int row, T array2b, void *elem, void *cl);
        void (*apply)();
        T array2b;
} *apply_cl;

/*
 *        name: UArray2b_new
 * description: Creates a new UArray2b with the given dimensions, size of 
 *              element, and blocksize.
 *  parameters:     width - width of the UArray2b (num columns)
 *                 height - height of the UArray2b (num rows)
 *                   size - size of each element in the UArray2b in bytes 
 *              blocksize - the width of each block 
 *     returns: A new UArray2b
 *      errors: throws a CRE if width, height, or size is <= 0, or if the 
 *              blocksize is < 1. Throws a Hanson exception is there are errors
 *              allocating memory 
 */
extern T UArray2b_new (int width, int height, int size, int blocksize)
{
        assert(width > 0 && height > 0 && size > 0);
        assert(blocksize >= 1);

        /* initialize struct and struct variables */
        T uarray2b = ALLOC(sizeof(*uarray2b));
        assert(uarray2b != NULL);

        uarray2b->width = width;
        uarray2b->height = height;
        uarray2b->size = size;
        uarray2b->blocksize = blocksize;
        
        /* calculate number of blocks in blocked array */
        int num_blocks = (width * height) / (blocksize * blocksize);
        (void)num_blocks;

        /* calculate number of blocks and initialize main 2D array */
        uarray2b->blocks_wide = ((width + (blocksize - 1)) / blocksize);
        uarray2b->blocks_high = ((height + (blocksize - 1)) / blocksize);
        uarray2b->main_arr = UArray2_new(uarray2b->blocks_wide, 
                                uarray2b->blocks_high, sizeof(UArray_T));
        
        /* initialize UArray elements */
        for (int col = 0; col < uarray2b->blocks_wide; col++) {
                for (int row = 0; row < uarray2b->blocks_high; row++) {
                        *(UArray_T *) UArray2_at(uarray2b->main_arr, col, row) 
                                = UArray_new(blocksize * blocksize, size);
                }      
        }
        
        return uarray2b;
}

/*
 *        name: UArray2b_new_64k_block
 * description: Creates a new UArray2b with the given dimensions, and size of 
 *              element. Makes the blocksize to be as big as possible and have
 *              the block still fit in 64KB. If the size of the element is 
 *              bigger than 64KB, then the blocksize is 1. 
 *  parameters:     width - width of the UArray2b (num columns)
 *                 height - height of the UArray2b (num rows)
 *                   size - size of each element in the UArray2b in bytes 
 *     returns: A new UArray2b
 *      errors: throws a CRE if width, height, or size is <= 0. Throws a Hanson
 *              exception is there are errors allocating memory  
 */
extern T UArray2b_new_64K_block(int width, int height, int size)
{       
        assert(width > 0 && height > 0 && size > 0);
        /* get # of cells in one block */
        int blocksize;
        if (size > (64 * 1024)) {
                blocksize = 1;
        } else {
                double block_cells = (64 * 1024) / size;
                /* size of one block */
                blocksize = (int) sqrt(block_cells);
        }
        
        return UArray2b_new(width, height, size, blocksize);
}

/*
 *        name: UArray2b_free
 * description: Frees the memory allocated for the UArray2b
 *  parameters: array2b - the UArray2b to be freed
 *     returns: None
 *      errors: Throws a CRE if the given UArray2b is NULL. Throws a Hanson 
 *              exception if there are issues freeing memory. 
 */
extern void UArray2b_free (T *array2b)
{       
        assert (array2b != NULL && *array2b != NULL);
        
        int blocks_wide = (*array2b)->blocks_wide;
        int blocks_high = (*array2b)->blocks_high;
        UArray2_T main_arr = (*array2b)->main_arr;
        
        /* free each block */
        for (int col = 0; col < blocks_wide; col++) {
                for (int row = 0; row < blocks_high; row++) {
                        UArray_T *curr = (UArray_T *) UArray2_at(main_arr, 
                                                                 col, row);
                        UArray_free(curr);
                }
        }

        /* free the uarray2 */
        UArray2_free(&((*array2b)->main_arr));
        FREE(*array2b);
}

/*
 *        name: UArray2b_width
 * description: Gives the width of the given UArray2b 
 *  parameters: array2b - the UArray2b to give the width of 
 *     returns: the width of the UArray2b 
 *      errors: Throws a CRE if the given UArray2b is NULL
 */
extern int UArray2b_width (T array2b)
{
        assert(array2b != NULL);
        return array2b->width;
}

/*
 *        name: UArray2b_height
 * description: Gives the height of the given UArray2b 
 *  parameters: array2b - the UArray2b to give the height of 
 *     returns: the height of the UArray2b 
 *      errors: throws a CRE if the given UArray2b is NULL
 */
extern int UArray2b_height (T array2b)
{
        assert (array2b != NULL);
        return array2b->height;
}

/*
 *        name: UArray2b_size
 * description: Gives the size of each element in the given UArray2b 
 *  parameters: array2b - the UArray2b to give the size of each element
 *     returns: the size of each element in the UArray2b 
 *      errors: throws a CRE if the given UArray2b is NULL
 */
extern int UArray2b_size (T array2b) 
{
        assert(array2b != NULL);
        return array2b->size;
}

/*
 *        name: UArray2b_blocksize
 * description: Gives the blocksize of the given UArray2b 
 *  parameters: array2b - the UArray2b to give the blocksize 
 *     returns: the blocksize of the UArray2b 
 *      erorrs: throws a CRE if the given UArray2b is NULL
 */
extern int UArray2b_blocksize(T array2b) 
{
        assert (array2b != NULL);
        return array2b->blocksize;
}

/*
 *        name: UArray2b_at
 * description: Gives the element at the given index 
 *  parameters: array2b - the UArray2b to retrieve an element from
 *                  col - the column index of the desired element
 *                  row - the row index of the desired element
 *     returns: a void pointer to the element at the given index
 *      errors: throws a CRE if the given UArray2b is null, or if the given 
 *              column or row index is out of bounds 
 */
extern void *UArray2b_at(T array2b, int col, int row)
{
        assert(array2b != NULL);
        assert(col >= 0 && col < array2b->width);
        assert(row >= 0 && row < array2b->height);

        int blocksize = array2b->blocksize;
        /* find the current block */
        UArray_T *block = UArray2_at(array2b->main_arr, (col / blocksize), 
                                        (row / blocksize));
        
        /* find the index in the current block */
        int index = blocksize * (col % blocksize) + (row % blocksize);

        /* return element at current index of current block */
        return UArray_at(*block, index);
}


/*
 *        name: UArray2b_map
 * description: Applies the given function to every element in the UArray2b
 *  parameters: array2b - the UArray2b to map
 *              apply - the function to apply to every element in the UArray2b
 *                          col - the column index of the element 
 *                          row - the row index of the element
 *                      array2b - the UArray2b 
 *                         elem - the current element 
 *                           cl - the closure argument 
 *                 cl - the closure argument  
 *     returns: None
 *      errors: throws a CRE if the given UArray2b or apply function is NULL
 */
extern void UArray2b_map(T array2b,
                        void apply(int col, int row, T array2b, void *elem,
                                void *cl),
                        void *cl)
{
        assert (array2b != NULL);
        assert (apply != NULL);
        (void)cl;
        int b_width = array2b->blocks_wide;
        int b_height = array2b->blocks_high;
        int b_size = array2b->blocksize;
        
        int width = array2b->width;
        int height = array2b->height;
        for (int b_col = 0; b_col < b_width; b_col++) { 
                for (int b_row = 0; b_row < b_height; b_row++) {
                        UArray_T *block = UArray2_at(array2b->main_arr, 
                                                        b_col, b_row);
                        assert(*block != NULL);
                        
                        for (int cell = 0; cell < b_size * b_size; cell++) { 
                                int col = b_col * b_size + cell / b_size;
                                int row = b_row * b_size + (cell % b_size);
                                if (col < width && row < height) {
                                        void *elem = UArray_at(*block, cell);
                                        apply(col, row, array2b, elem, cl);
                                }
                        }
                }
        }
}

#undef T