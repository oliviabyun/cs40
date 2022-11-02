/*
 *     decompress.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This file is the interface for decompress and allows a user to decompress
 *     a provided image (stored using codewords) into a regular PPM image.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "decompress.h"
#include "codewords.h"

/* 
 *      name: decompress
 *   purpose: decompresses a provided image
 *    inputs: fp - pointer to beginning of file to be decompressed
 *   outputs: none
 *    errors: raises a checked runtime error if the file pointer is NULL
 */
void decompress(FILE *fp)
{
        assert(fp != NULL);
        
        /* read in compressed image into array of words */
        A2Methods_UArray2 quant_array = codewords_decompress(fp);

        /* transform quantized pixels to floating-point RGB pixels */
        A2Methods_UArray2 float_array = transform_decompress(quant_array);

        /* transform to RGB scaled-int form and print regular PPM */
        ppmrgb_decompress(float_array);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* free heap-allocated memory */
        methods->free(&float_array);
        methods->free(&quant_array);
}