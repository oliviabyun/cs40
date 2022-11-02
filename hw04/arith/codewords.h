/*
 *     codewords.h
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the interface for codewords, where a user can take a quantized
 *     2D array of pixels and pack them into codewords, then print to standard
 *     output. The reverse can also be done, where a compressed image can be
 *     read in and codewords can be unpacked into quantized pixels in scaled
 *     integer representation.
 *     
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "transform.h"

/* COMPRESSION FUNCTIONS */
void codewords_compress(A2Methods_UArray2 quant_array);
A2Methods_UArray2 codewords_pack(A2Methods_UArray2 quant_array);
void codewords_print(A2Methods_UArray2 word_array);

/* DECOMPRESSION FUNCTIONS */
A2Methods_UArray2 codewords_decompress(FILE *fp);
A2Methods_UArray2 codewords_read(FILE *fp);
A2Methods_UArray2 codewords_unpack(A2Methods_UArray2 word_array);
