/*
 *     ppm_rgb.h
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the interface for ppm_rgb, where 2D pixel arrays can be 
 *     transformed to/from a scaled integer representation and a floating point
 *     representation. Users can also trim the last row/column of an image or
 *     print a decompressed/regular PPM image to standard output.
 */
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "pnm.h"
#include "uarray2.h"
#include "a2methods.h"

typedef struct Rgb_float *Rgb_float;

/* 
 * purpose: store all information related to a pixel in floating-point 
 *          representation
 * members: red - the red value of a single pixel (float)
 *          green - the green value of a single pixel (float)
 *          blue - the blue value of a single pixel (float)
 */
struct Rgb_float {
        float red, green, blue; /* RGB values of a pixel in float form */
};

/* DECOMPRESSION FUNCTIONS */
void ppmrgb_decompress(A2Methods_UArray2 transformed);
Pnm_ppm float_to_int(A2Methods_UArray2 float_arr);
void print(Pnm_ppm transformed);

/* COMPRESSION FUNCTIONS */
A2Methods_UArray2 ppmrgb_compress(FILE *fp);
Pnm_ppm trim(Pnm_ppm image);
A2Methods_UArray2 int_to_float(Pnm_ppm image);



