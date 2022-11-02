/*
 *     codewords.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the implementation for codewords, where a user can take a 
 *     quantized 2D array of pixels and pack them into codewords, then print to
 *     standard output. The reverse can also be done, where a compressed image
 *     can be read in and codewords can be unpacked into quantized pixels in
 *     scaled integer representation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "arith40.h"
#include "bitpack.h"
#include "codewords.h"

/* codewords apply functions */
void apply_pack(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *quant_array);
void apply_unpack(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *word_array);
void apply_print(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *cl);
void apply_read(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *file);

/* 
 *      name: codewords_compress
 *   purpose: given an array with Quant_pix elements (in scaled integer form),
 *            compresses the image by bitpacking and prints to standard output
 *    inputs: quant_array - the 2D pixel array with Quant_pix elements (where
 *            the struct values a,b,c,d,pb,pr are scaled integers)
 *   outputs: none
 *    errors: throws a CRE if the provided quant_array is NULL, or if the 
 *            methods suite is NULL
 */
void codewords_compress(A2Methods_UArray2 quant_array)
{
        assert(quant_array != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* pack codewords into new array */
        A2Methods_UArray2 word_arr = codewords_pack(quant_array);

        /* print codewords */
        codewords_print(word_arr);
        
        methods->free(&word_arr); /* free heap-allocated memory */
}

/* 
 *      name: codewords_decompress
 *   purpose: given a compressed image, aids in decompression by reading in 
 *            the file, unpacking codewords, and calculating chroma codes
 *    inputs: fp - a pointer to the beginning of a file containing a header
 *            with information about the image, and the codewords
 *   outputs: an A2Methods_UArray2 with Quant_pix struct elements (where pixel
 *            data is stored in scaled integer form)
 *    errors: throws a CRE if the provided word_array is NULL
 */
A2Methods_UArray2 codewords_decompress(FILE *fp)
{
        assert(fp != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* read in codewords from file */
        A2Methods_UArray2 word_arr = codewords_read(fp);

        /* unpack codewords into array of Quant_pix elements */
        A2Methods_UArray2 quant_arr = codewords_unpack(word_arr);
        
        methods->free(&word_arr); /* free heap-allocated memory */

        return quant_arr;
}

/* 
 *      name: codewords_pack
 *   purpose: pack each value of each Quant_pix element into a codeword
 *    inputs: quant_array - an A2Methods_UArray2 of Quant_pix struct elements
 *   outputs: an A2Methods_UArray2 of 32 bit codewords (of type uint64_t)
 *    errors: throws a CRE if the provided quant_array is NULL
 */
A2Methods_UArray2 codewords_pack(A2Methods_UArray2 quant_arr)
{
        assert(quant_arr != NULL);
        
        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* create new 2D array of codewords */
        A2Methods_UArray2 word_arr = methods->new(methods->width(quant_arr), 
                        methods->height(quant_arr), sizeof(uint64_t));
        
        /* pack Quant_pix struct elements into words in new array */
        methods->map_col_major(word_arr, apply_pack, quant_arr);

        return word_arr;
}

/* 
 *      name: apply_pack
 *   purpose: apply function to pack the values in each Quant_pix element into
 *            a 32-bit codeword.
 *    inputs:       col - the column index of the current element (integer)
 *                  row - the row index of the current element (integer)
 *                array - the A2Methods_UArray2 of codewords
 *                 elem - a pointer to the current codeword element in the array
 *                        (of type uint64_t)
 *          quant_array - a void pointer to an A2Methods_UArray2 Quant_pix array
 *   outputs: none
 *    errors: raises a Checked Runtime Error if the closure argument or the
 *            current element are NULL or if the methods suite is NULL
 */
void apply_pack(int col, int row, A2Methods_UArray2 array, void *elem, 
        void *quant_array)
{
        (void)array;
        assert(elem != NULL);
        assert(quant_array != NULL);

        /* get 2D array of pixels in floating-point from closure */
        A2Methods_UArray2 quant_arr = (A2Methods_UArray2) quant_array;

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* get current Quant_pix pixel struct */
        struct Quant_pix *curr = methods->at(quant_arr, col, row);

        uint64_t *word = (uint64_t *)elem;

        /* pack Quant_pix values in the codeword */
        *word = Bitpack_newu(*word, 9, 23, curr->a);
        *word = Bitpack_news(*word, 5, 18, curr->b);
        *word = Bitpack_news(*word, 5, 13, curr->c);
        *word = Bitpack_news(*word, 5, 8, curr->d);
        *word = Bitpack_newu(*word, 4, 4, curr->pb);
        *word = Bitpack_newu(*word, 4, 0, curr->pr);
}

/* 
 *      name: codewords_print
 *   purpose: print out the given word_array in row_major and big endian order
 *            to standard output
 *    inputs: word_array - an A2Methods_UArray2 of 32 bit codewords 
                (of type uint64_t)
 *   outputs: none
 *    errors: raises a CRE if the methods suite is NULL or if the given 
 *            word_array is NULL
 */
void codewords_print(A2Methods_UArray2 word_array)
{
        assert(word_array != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* get height and width of 2D pixel array */
        unsigned w = methods->width(word_array);
        unsigned h = methods->height(word_array);

        /* print header */
        fprintf(stdout, "COMP40 Compressed image format 2\n%u %u", w, h);
        fprintf(stdout, "\n");
        
        /* print codewords */
        methods->map_row_major(word_array, apply_print, NULL);
}

/* 
 *      name: apply_print
 *   purpose: print out the contents of the current codeword element in
 *            big endian order to standard output
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the A2Methods_UArray2 of codewords
 *             elem - a pointer to the current codeword element in the array 
 *                      (of type uint64_t)
 *               cl - NULL
 *   outputs: none
 *    errors: raises a CRE if the current element is NULL
 */
void apply_print(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *cl)
{
        (void)col;
        (void)row;
        (void)array;
        (void)cl;
        assert(elem != NULL);

        uint64_t *word = (uint64_t *)elem; /* get the current word */

        /* loop through the 32-bit word in big endian order */
        for (int i = 31; i >= 0; i--) {
                /* print one character at a time */
                putchar(Bitpack_getu(*word, 1, i));
        }
}

/* 
 *      name: codewords_unpack
 *   purpose: unpack each codeword into values for each Quant_pix element
 *    inputs: word_array - an A2Methods_UArray2 of 32 bit codewords 
                (of type uint64_t)
 *   outputs: an A2Methods_UArray2 of Quant_pix struct elements
 *    errors: raises a CRE if the given word_array is NULL or if the methods
 *            suite is NULL
 */
A2Methods_UArray2 codewords_unpack(A2Methods_UArray2 word_array)
{
        assert(word_array != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* initialize new array to hold pixel data in scaled integer form */
        A2Methods_UArray2 quant_arr = methods->new(methods->width(word_array), 
                        methods->height(word_array), sizeof(struct Quant_pix));
        
        /* unpack words into Quant_pix elements of the new array */
        methods->map_col_major(quant_arr, apply_unpack, word_array);

        return quant_arr;
}

/* 
 *      name: apply_unpack
 *   purpose: apply function to unpack the values in each 32-bit codeword into
 *            the values in each Quant_pix struct element
 *    inputs:      col - the column index of the current element (integer)
 *                 row - the row index of the current element (integer)
 *               array - the A2Methods_UArray2 of Quant_pix struct elements
 *                elem - a pointer to the current Quant_pix element in the array
 *          word_array - the A2Methods_UArray2 of 32-bit codewords
 *   outputs: none
 *    errors: raises a CRE if the current element is NULL, or if the closure
 *            argument is NULL, or if the methods suite is NULL
 */
void apply_unpack(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *word_array)
{
        (void)array;
        assert(elem != NULL);
        assert(word_array != NULL);

        A2Methods_UArray2 word_arr = (A2Methods_UArray2) word_array; 

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* get word at current index of array */
        uint64_t *word = methods->at(word_arr, col, row);

        /* get current Quant_pix element */
        struct Quant_pix *pixel = (struct Quant_pix *)elem;
        
        /* unpack words into Quant_pix struct elements */
        pixel->a = Bitpack_getu(*word, 9, 23);
        pixel->b = Bitpack_gets(*word, 5, 18);
        pixel->c = Bitpack_gets(*word, 5, 13);
        pixel->d = Bitpack_gets(*word, 5, 8);
        pixel->pb = Bitpack_getu(*word, 4, 4);
        pixel->pr = Bitpack_getu(*word, 4, 0);
}

/* 
 *      name: codewords_read
 *   purpose: reads a compressed ppm file of codewords and stores the codewords
 *    inputs: fp - a pointer to the start of the compressed ppm file of 
 *            codewords
 *   outputs: an A2Methods_UArray2 of 32 bit codewords (of type uint64_t)
 *    errors: throws a CRE if the given file pointer is NULL, or if the 
 *            methods suite is NULL
 */
A2Methods_UArray2 codewords_read(FILE *fp)
{
        assert(fp != NULL);
        unsigned height, width;
        
        /* read in header */
        int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", &width,
                        &height);
        assert(read == 2);
        
        int c = getc(fp);
        assert(c == '\n');

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* read in codewords and put into uarray2 */
        A2Methods_UArray2 word_array = methods->new(width, height, 
                                                sizeof(uint64_t));

        /* read in each word */
        methods->map_row_major(word_array, apply_read, fp);

        return word_array;
}

/* 
 *      name: codewords_read
 *   purpose: given a codeword, 
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the current 2D pixel array of 64-bit words
 *             elem - a void pointer to the current 64-bit integer word element
 *                    of the 2D array
 *             file - a void pointer to the closure argument, which is a file
 *                    pointer for the compressed image being read in
 *   outputs: none
 *    errors: throws a CRE if the given elem or cl are NULL
 */
void apply_read(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *file)
{
        (void)col;
        (void)row;
        (void)array;
        
        assert(elem != NULL);
        assert(file != NULL);

        FILE *fp = (FILE *)file;
        uint64_t *word = (uint64_t *)elem;
        
        /* word should be 32 bits */
        for (int i = 31; i >= 0; i--) {
                uint64_t c = getc(fp); /* get current char from file */
                *word = Bitpack_newu(*word, 1, i, c); /* update curr word */
        }
}
