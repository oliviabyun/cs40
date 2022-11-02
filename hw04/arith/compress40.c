/*
 *     compress40.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This file either compresses or decompresses an image given an input
 *     file.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "assert.h"
#include "pnm.h"
#include "compress.h"
#include "decompress.h"

/* 
 *      name: compress40
 *   purpose: compresses a given PPM image 
 *    inputs: a pointer to the beginning of the image file to be compressed
 *   outputs: none
 *    errors: throws a CRE if the given file pointer is NULL
 */
extern void compress40(FILE *input) 
{
        assert(input != NULL);

        /* compress and print compressed image */
        compress(input);
}

/* 
 *      name: decompress40
 *   purpose: decompresses a given compressed image
 *    inputs: a pointer to the beginning of the compressed image file
 *   outputs: none
 *    errors: throws a CRE if the given file pointer is NULL
 */
extern void decompress40(FILE *input) 
{
        assert(input != NULL);

        /* decompress and print decompressed image */
        decompress(input);
}