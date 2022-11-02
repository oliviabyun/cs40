/*
 *     transform.h
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the interface for transform where the client can see what can be 
 *     done: pixel arrays can be converted from rgb color space to video color
 *     space. They can also undergo discrete cosine transformations, and/or 
 *     become quantized. This can also happen in the reverse order, where
 *     a user can calculate chroma codes, inverse discrete cosine, transform
 *     to video color space, then to rgb color space.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "pnm.h"
#include "uarray2.h"
#include "a2methods.h"
#include "ppm_rgb.h"


typedef struct Video_pix *Video_pix;
typedef struct Discrete_pix *Discrete_pix;
typedef struct Quant_pix *Quant_pix;

/* 
 * purpose: store data necessary for video color space
 * members: y - luminance value of a pixel (float)
 *          pb, pr - color-difference values of a pixel (float)
 */
struct Video_pix {
        float y, pb, pr; /* Y, Pb, Pr of a pixel in float form */
};

/* 
 * purpose: store 2 by 2 pixel block data after discrete cosine transformation
 * members: a, b, c, d - Discrete Cosine Transform space brightness 
 *                       values (float)
 *          pb, pr - pb and pr average values for each 2x2 block of 
 *                   pixels (float)
 */
struct Discrete_pix {
        /* a, b, c, d, Pb, Pr of a pixel in float form */
        float a, b, c, d, pb, pr;
};

/* 
 * purpose: store 2 by 2 pixel block data after quantization
 * members: b, c, d - 5-bit signed value representation of DCT brightness 
 *                    values (int)
 *          a - 9-bit unsigned value representation of DCT brightness a 
 *              value (unsigned)
 *          pb, pr - quantized unsigned value representation of average pb, pr 
 *                   values (unsigned)
 */
struct Quant_pix {
        /* a, b, c, d, Pb, Pr of a pixel in scaled integer form */
        int b, c, d;
        unsigned a, pb, pr;
};

/* main logic functions: compression and decompression */
A2Methods_UArray2 transform_compress(A2Methods_UArray2 float_arr);
A2Methods_UArray2 transform_decompress(A2Methods_UArray2 quant_arr);

/* COMPRESSION FUNCTIONS: floating-point -> video color space */
A2Methods_UArray2 float_to_video(A2Methods_UArray2 float_arr);

/* DECOMPRESSION FUNCTIONS: video color space -> floating-point */
A2Methods_UArray2 video_to_float(A2Methods_UArray2 video_arr);

/* COMPRESSION FUNCTIONS: discrete cosine transformation */
A2Methods_UArray2 video_to_discrete(A2Methods_UArray2 vid_arr);

/* DECOMPRESSION FUNCTIONS: inverse discrete cosine transformation */
A2Methods_UArray2 discrete_to_video(A2Methods_UArray2 discrete_arr);
void add_pb_pr(struct Discrete_pix *curr, struct Video_pix *pix);
void fix_color_range(struct Rgb_float *pixel);

/* COMPRESSION FUNCTIONS: quantization */
A2Methods_UArray2 discrete_to_quant(A2Methods_UArray2 discrete_arr);
void fix_discrete_range(struct Discrete_pix *pixel);

/* DECOMPRESSION FUNCTIONS: calculating chroma codes */
A2Methods_UArray2 quant_to_discrete(A2Methods_UArray2 quant_arr);