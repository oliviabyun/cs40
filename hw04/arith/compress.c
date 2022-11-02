/*
 *     compress.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This file is the implementation for compress and compresses a provided 
 *     PPM image into a compressed image stored using codewords.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "compress.h"
#include "codewords.h"

/* 
 *      name: compress
 *   purpose: compresses a provided PPM image
 *    inputs: fp - pointer to beginning of file to be compressed
 *   outputs: none
 *    errors: raises a checked runtime error if the file pointer is NULL
 */
void compress(FILE *fp) 
{
        assert(fp != NULL);

        /* transform from scaled integer to floating-point representation */
        A2Methods_UArray2 float_arr = ppmrgb_compress(fp);

        /* discrete cosine transformation and quantization */
        A2Methods_UArray2 quant_arr = transform_compress(float_arr);

        /* pack into codewords */
        codewords_compress(quant_arr);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* free heap-allocated memory */
        methods->free(&float_arr);
        methods->free(&quant_arr);
}