/*
 *     transform.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the implementation for transform, where 2D pixel arrays can be 
 *     converted from rgb color space to video color space. They can also 
 *     undergo discrete cosine transformations, and/or become quantized. This 
 *     can also happen in the reverse order, where a user can calculate chroma 
 *     codes, inverse discrete cosine, transform to video color space, then to 
 *     rgb color space.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "transform.h"
#include "arith40.h"

/* transform apply functions */
void apply_vid(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *float_array);
void apply_floats(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *vid_array);
void apply_discrete(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *vid_array);
void apply_inv_discrete(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *video_arr);
void apply_quant(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *discrete_array);
void apply_chroma(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *quant_array);

/* 
 *      name: transform_compress
 *   purpose: aids in compressing an image by transforming a floating-point
 *            pixel array using a discrete cosine transformation and
 *            quantization
 *    inputs: float_arr - a 2D array containing Rgb_float struct elements
 *   outputs: a 2D array (of type A2Methods_UArray2) containing Discrete_pix 
 *            elements
 *    errors: raises a checked runtime error if the methods suite is NULL or
 *            if the given floating-point 2D array is null
 */
A2Methods_UArray2 transform_compress(A2Methods_UArray2 float_arr)
{
        assert(float_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* transform from floating point to video color space */
        A2Methods_UArray2 vid_arr = float_to_video(float_arr);

        /* discrete cosine transformation */
        A2Methods_UArray2 discrete_arr = video_to_discrete(vid_arr);
        methods->free(&vid_arr);

        /* quantize pixels */
        A2Methods_UArray2 quant_arr = discrete_to_quant(discrete_arr);
        methods->free(&discrete_arr);

        return quant_arr;
}

/* 
 *      name: transform_decompress
 *   purpose: aids in decompressing an image by transforming a quantized array
 *            of pixels in scaled integer representation into a pixel array
 *            in video color space in floating-point representation
 *    inputs: quant_arr - the 2D array of pixels to be decompressed (containing
 *            Quant_pix elements)
 *   outputs: a 2D array (of type A2Methods_UArray2) containing Video_pix
 *            elements
 *    errors: raises a checked runtime error if the provided array is NULL or 
 *            if the methods suite is NULL
 */
A2Methods_UArray2 transform_decompress(A2Methods_UArray2 quant_arr)
{
        assert(quant_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /*calculate chroma elements */
        A2Methods_UArray2 discrete_arr = quant_to_discrete(quant_arr);

        /* inverse discrete cosine */
        A2Methods_UArray2 vid_arr = discrete_to_video(discrete_arr);
        methods->free(&discrete_arr);
        
        /* transform back to video color space */
        A2Methods_UArray2 float_arr = video_to_float(vid_arr);
        
        methods->free(&vid_arr); /* free heap-allocated memory */

        return float_arr;
}

/* 
 *      name: float_to_video
 *   purpose: convert UArray2 with pixels of type float to UArray2 with pixels
 *            in component video color space
 *    inputs: float_arr - A UArray2 with pixels of type float
 *   outputs: A UArray2 with pixels in component video color space
 *    errors: raises a CRE if the provided float_arr is NULL or if the methods
 *            suite is NULL
 */
A2Methods_UArray2 float_to_video(A2Methods_UArray2 float_arr)
{
        assert(float_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* create new array to store Video_pix elements */
        A2Methods_UArray2 vid_arr = methods->new(methods->width(float_arr), 
                        methods->height(float_arr), sizeof(struct Video_pix));

        /* transform each pixel float to component video color space */
        methods->map_col_major(vid_arr, apply_vid, float_arr);

        return vid_arr;
}

/* 
 *      name: apply_vid
 *   purpose: apply function to convert pixels from rgb colorspace to component
 *            video colorspace
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the new 2D array containing Rgb_float struct elements
 *                    where the RGB values are floats
 *             elem - a pointer to the current element in the array (of type
 *                    struct Video_pix)
 *      float_array - a void pointer to an A2Methods_UArray2 Rgb_float array
 *                    containing the current image pixel data
 *   outputs: none
 *    errors: throws a checked runtime error if the current element, closure
 *            argument, or methods suite are NULL;
 */
void apply_vid(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *float_array)
{
        (void)array;
        assert(elem != NULL);
        assert(float_array != NULL);

        /* get 2D array of pixels in floating-point from closure */
        A2Methods_UArray2 float_arr = (A2Methods_UArray2) float_array;

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* get floating-point pixel and its RGB values at current index */
        struct Rgb_float *curr = methods->at(float_arr, col, row);
        float r = curr->red;
        float g = curr->green;
        float b = curr->blue;
        
        /* get current video pixel */
        struct Video_pix *pixel = (struct Video_pix *)elem; 
        
        /* calculate values for Y, Pb, and Pr */
        pixel->y = (0.299 * r) + (0.587 * g) + (0.114 * b);
        pixel->pb = (-0.168736 * r) - (0.331264 * g) + (0.5 * b);
        pixel->pr = (0.5 * r) - (0.418688 * g) - (0.081312 * b);
}

/* 
 *      name: video_to_discrete
 *   purpose: convert UArray2 
 *    inputs: vid_arr - an A2MethodsUArray2 of Video_pix struct elements.
 *   outputs: an A2MethodsUarray2 of Discrete_pix struct elements.
 *    errors: throws a checked runtime error if the provided vid_arr or the
 *            methods suite are NULL
 */
A2Methods_UArray2 video_to_discrete(A2Methods_UArray2 vid_arr)
{
        assert(vid_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain;  /* get methods suite */
        assert(methods != NULL);

        /* calculate height and width for new discrete array */
        int width = methods->width(vid_arr) / 2;
        int height = methods->height(vid_arr) / 2;

        /* create new 2D array to store Discrete_pix elements */
        A2Methods_UArray2 discrete_arr = methods->new(width, height, 
                                        sizeof(struct Discrete_pix));

        /* discrete cosine transformation */
        methods->map_col_major(discrete_arr, apply_discrete, vid_arr);
        
        return discrete_arr;
}

/* 
 *      name: apply_discrete
 *   purpose: apply function to apply discrete cosine transform on each group
 *            of 4 pixels.
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the new 2D array containing Discrete_pix struct elements
 *             elem - a pointer to the current element in the array (of type
 *                    struct Discrete_pix)
 *        vid_array - a void pointer to an A2Methods_UArray2 Video_pix array
 *                    containing the current image pixel data
 *   outputs: none
 *    errors: raises a Checked Runtime Error if the closure argument or the
 *            current element are NULL or if the methods suite is NULL
 */
void apply_discrete(int col, int row, A2Methods_UArray2 array, void *elem, 
                        void *vid_array)
{
        (void)array;
        assert(elem != NULL);
        assert(vid_array != NULL);
        
        A2Methods_UArray2 vid_arr = (A2Methods_UArray2)vid_array;

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* get the 4 pixels from vid_arr that map to current 4-bit index */
        struct Video_pix *pix1 = methods->at(vid_arr, col * 2, row * 2);
        struct Video_pix *pix2 = methods->at(vid_arr, col * 2 + 1, row * 2);
        struct Video_pix *pix3 = methods->at(vid_arr, col * 2, row * 2 + 1);
        struct Video_pix *pix4 = methods->at(vid_arr, col * 2 + 1, row * 2 + 1);
        
        /* calculate values of a,b,c,d and average pb and pr */
        float a = (pix4->y + pix3->y + pix2->y + pix1->y) / 4.0;
        float b = (pix4->y + pix3->y - pix2->y - pix1->y) / 4.0;
        float c = (pix4->y - pix3->y + pix2->y - pix1->y) / 4.0;
        float d = (pix4->y - pix3->y - pix2->y + pix1->y) / 4.0;
        float pb_avg = (pix1->pb + pix2->pb + pix3->pb + pix4->pb) / 4.0;
        float pr_avg = (pix1->pr + pix2->pr + pix3->pr + pix4->pr) / 4.0;
        
        /* set struct elements */
        struct Discrete_pix *curr = (struct Discrete_pix *) elem;
        curr->a = a;
        curr->b = b;
        curr->c = c;
        curr->d = d;
        curr->pb = pb_avg;
        curr->pr = pr_avg;
}

/* 
 *      name: discrete_to_quant
 *   purpose: convert discrete-cosine-transformed elements into quantized 
 *            elements where b, c, d are scaled ints and a, pb, pr are
 *            unsigned ints.
 *    inputs: an A2Methods_UArray2 of Discrete_pix struct elements
 *   outputs: an A2Methods_UArray2 of Quant_pix struct elements
 *    errors: raises a CRE if the provided discret_arr is NULL or if the 
 *            methods suite is NULL
 */
A2Methods_UArray2 discrete_to_quant(A2Methods_UArray2 discrete_arr) 
{
        assert(discrete_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* get width and height from Discrete_pix array */
        int width = methods->width(discrete_arr);
        int height = methods->height(discrete_arr);
        
        /* create new 2D array to store quantized pixels */
        A2Methods_UArray2 quant_arr = methods->new(width, height, 
                                        sizeof(struct Quant_pix));

        /* quantize each pixel */
        methods->map_col_major(quant_arr, apply_quant, discrete_arr);

        return quant_arr;
}

/* 
 *      name: apply_quant
 *   purpose: apply function to quantize b, c, d, pb, pr values.
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the new 2D array containing Quant_pix struct elements
 *                    where the b, c, d values are scaled ints and a, pb, pr 
 *                    are unsigned ints.
 *             elem - a void pointer to the current Quant_pix element in the
 *                    2D array.
 *   discrete_array - a void pointer to a A2Methods_UArray2 instance containing
 *                    the current a, b, c, d, Pb, Pr of a pixel in float form.
 *   outputs: none
 *    errors: raises a Checked Runtime Error if the closure argument or the
 *            current element are NULL or if the methods suite is NULL
 */
void apply_quant(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *discrete_array)
{
        (void)array;
        assert(elem != NULL);
        assert(discrete_array != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        A2Methods_UArray2 discrete_arr = (A2Methods_UArray2) discrete_array;

        /* get pixel at current index of old array */
        struct Discrete_pix *pixel = methods->at(discrete_arr, col, row);

        struct Quant_pix *curr = (struct Quant_pix *) elem;        
        
        /* force b,c,d into -0.3 to +0.3 range */
        fix_discrete_range(pixel);
        
        /* code a into 9 unsigned bits */
        curr->a = pixel->a * 511;

        /* quantize b, c, d into five-bit signed values */
        curr->b = round(50 * pixel->b);
        curr->c = round(50 * pixel->c);
        curr->d = round(50 * pixel->d);
        
        /* get pb and pr values */
        curr->pb = Arith40_index_of_chroma(pixel->pb);
        curr->pr = Arith40_index_of_chroma(pixel->pr);
}

/* 
 *      name: fix_discrete_range
 *   purpose: force b, c, d values into -0.3 to +0.3 range
 *    inputs: pixel - an element in the A2Methods_UArray2 instance containing
 *                    the current a, b, c, d, Pb, Pr of a pixel in float form
 *   outputs: none
 *    errors: raises a CRE if the given pixel is NULL
 */
void fix_discrete_range(struct Discrete_pix *pixel)
{
        assert(pixel != NULL);

        /* force b into the range [-0.3, 0.3] */
        if (pixel->b < -0.3) {
                pixel->b = -0.3;
        } else if (pixel->b > 0.3) {
                pixel->b = 0.3;
        }

        /* force c into the range [-0.3, 0.3] */
        if (pixel->c < -0.3) {
                pixel->c = -0.3;
        } else if (pixel->c > 0.3) {
                pixel->c = 0.3;
        }

        /* force d into the range [-0.3, 0.3] */
        if (pixel->d < -0.3) {
                pixel->d = -0.3;
        } else if (pixel->d > 0.3) {
                pixel->d = 0.3;
        }
}

/* 
 *      name: quant_to_discrete
 *   purpose: given a quantized array of Quant_pix struct elements, calculates
 *            chroma values and transforms each pixel into a Discrete_pix
 *            struct element
 *    inputs: quant_arr - a 2D pixel array of Quant_pix struct elements (array
 *            of type A2Methods_UArray2)
 *   outputs: a 2D pixel array containing Discrete_pix struct elements
 *    errors: throws a CRE if the provided quant_arr is NULL or if the
 *            methods suite is NULL
 */
A2Methods_UArray2 quant_to_discrete(A2Methods_UArray2 quant_arr) 
{
        assert(quant_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* get width and height of given Quant_pix array */
        int width = methods->width(quant_arr);
        int height = methods->height(quant_arr);
        
        /* create new array of Discrete_pix struct elements */
        A2Methods_UArray2 discrete_arr = methods->new(width, height, 
                                        sizeof(struct Discrete_pix));

        /* calculate chroma values */
        methods->map_col_major(discrete_arr, apply_chroma, quant_arr);

        return discrete_arr;
        
}

/* 
 *      name: apply_chroma
 *   purpose: apply function to convert pixels (of type struct Discrete_pix)
 *            to floating-point quantized pixels (of type struct Quant_pix)
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the new 2D array containing Discrete_pix struct elements
 *                    where a, b, c, d, Pb, Pr values of a pixel are floats.
 *             elem - a void pointer to the current Discrete_pix element in the
 *                    2D array.
 *      quant_array - a void pointer to the 2D array containing Quant_pix 
 *                    struct elements where the b, c, d values are scaled ints 
 *                    and a, pb, pr are unsigned ints.
 *   outputs: none
 *    errors: raise a CRE if the methods suite is NULL or if the closure 
 *            argument or the current element are NULL
 */
void apply_chroma(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *quant_array)
{
        (void)array;
        assert(elem != NULL);
        assert(quant_array != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        A2Methods_UArray2 quant_arr = (A2Methods_UArray2) quant_array;

        /* get pixel at current index of quant_arr */
        struct Quant_pix *pixel = methods->at(quant_arr, col, row);

        struct Discrete_pix *curr = (struct Discrete_pix *) elem;

        /* convert four-bit chroma codes to PB and PR */
        curr->pb = Arith40_chroma_of_index(pixel->pb);
        curr->pr = Arith40_chroma_of_index(pixel->pr);
        
        /* transform a,b,c,d to floats */
        curr->a = ((float)pixel->a / 511.0);
        curr->b = ((float)pixel->b / 50.0);
        curr->c = ((float)pixel->c / 50.0);
        curr->d = ((float)pixel->d / 50.0);
}

/* 
 *      name: discrete_to_video
 *   purpose: transforms a 2D pixel array of Discrete_pix elements to a 2D
 *            array in video color space storing Video_pix struct elements
 *    inputs: discrete_arr - an A2MethodsUarray2 of Discrete_pix struct elements
 *   outputs: a 2D array of pixels stored as Video_pix struct elements
 *    errors: throws a CRE if the given discrete_arr is NULL or if the methods
 *            suite is NULL
 */
A2Methods_UArray2 discrete_to_video(A2Methods_UArray2 discrete_arr) 
{
        assert(discrete_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        int width = methods->width(discrete_arr) * 2;
        int height = methods->height(discrete_arr) * 2;
        A2Methods_UArray2 vid_arr = methods->new(width, height, 
                                        sizeof(struct Video_pix));
        methods->map_col_major(discrete_arr, apply_inv_discrete, vid_arr);

        return vid_arr;
}

/* 
 *      name: apply_inv_discrete
 *   purpose: an apply function that performs an inverse discrete cosine
 *            transformation on each 4-bit group of pixels
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the discrete array containing Discrete_pix elements
 *             elem - a void pointer to the current element in the discrete
 *                    array (element of type struct Discrete_pix)
 *        video_arr - a void pointer to the the new 2D array containing 
 *                    Video_pix elements where Y, Pb, and Pr are in 
 *                    floating-point representation
 *   outputs: none
 *    errors: raises a checked runtime error if the current element, closure 
 *            argument, or methods suite are NULL
 */
void apply_inv_discrete(int col, int row, A2Methods_UArray2 array, void *elem, 
                        void *video_arr)
{
        assert(elem != NULL);
        assert(video_arr != NULL);
        (void)array;

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* get the current 2D array containing Video_pix elements */
        A2Methods_UArray2 vid_arr = (A2Methods_UArray2)video_arr;
        
        /* calculate the 4 pixel indices of the 4-bit group */
        struct Video_pix *pix1 = methods->at(vid_arr, col * 2, row * 2);
        struct Video_pix *pix2 = methods->at(vid_arr, col * 2 + 1, row * 2);
        struct Video_pix *pix3 = methods->at(vid_arr, col * 2, row * 2 + 1);
        struct Video_pix *pix4 = methods->at(vid_arr, col * 2 + 1, row * 2 + 1);

        /* get current element along with its a,b,c,d values */
        struct Discrete_pix *curr = (struct Discrete_pix *) elem;
        
        float a = curr->a;
        float b = curr->b;
        float c = curr->c;
        float d = curr->d;
        
        /* calculate Y1, Y2, Y3, Y4 values */
        pix1->y = a - b - c + d;
        pix2->y = a - b + c - d;
        pix3->y = a + b - c - d;
        pix4->y = a + b + c + d;
        
        /* pb and pr are still the same: copy over values */
        add_pb_pr(curr, pix1);
        add_pb_pr(curr, pix2);
        add_pb_pr(curr, pix3);
        add_pb_pr(curr, pix4);
}

/* 
 *      name: add_pb_pr
 *   purpose: sets the pb and pr values of a Video_pix struct given a 
 *            corresponding Discrete_pix struct
 *    inputs: curr - a pointer to a Discrete_pix struct containing data for
 *            a single pixel after undergoing a discrete cosine transformation
 *             pix - a pointer to a Video_pix housing y, pb, pr as floats
 *   outputs: none
 *    errors: raises a checked runtime error if curr or pix are NULL
 */
void add_pb_pr(struct Discrete_pix *curr, struct Video_pix *pix)
{
        assert(curr != NULL);
        assert(pix != NULL);

        /* copy pb and pr values from Discrete_pix into Video_pix */
        pix->pb = curr->pb;
        pix->pr = curr->pr;
}

/* 
 *      name: video_to_float
 *   purpose: transforms a given pixel array in video color space to an RGB
 *            floating point representation
 *    inputs: video_arr - a 2D array (of type A2Methods_UArray2) containing
 *            Video_pix elements (storing pixel data as floats y, pb, pr)
 *   outputs: an A2Methods_UArray2 instance with Rgb_float struct elements
 *            containing RGB values in float form
 *    errors: raises a checked runtime error if the given video_arr is NULL
 */
A2Methods_UArray2 video_to_float(A2Methods_UArray2 video_arr)
{
        assert(video_arr != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* create a new array to house pixels in RGB floating-point form */
        A2Methods_UArray2 float_arr = methods->new(methods->width(video_arr), 
                        methods->height(video_arr), sizeof(struct Rgb_float));

        /* transform pixels from component video to RGB color space */
        methods->map_col_major(float_arr, apply_floats, video_arr);

        return float_arr;
}

/* 
 *      name: apply_floats
 *   purpose: apply function that transforms a single pixel from video color
 *            space to an RGB-floating point representation, claculating each
 *            RGB value
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the current 2D array of Rgb_float struct elements
 *             elem - a void pointer to the current element in the array (of
 *                    type Rgb_float struct)
 *        vid_array - a void pointer to the closure argument: the
 *                    A2Methods_UArray2 with Video_pix elements containing the
 *                    old image pixel data
 *   outputs: none
 *    errors: raises a checked runtime error if the current element, closure
 *            argument, or methods suite are NULL
 */
void apply_floats(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *vid_array)
{
        (void)array;
        assert(elem != NULL);
        assert(vid_array != NULL);

        A2Methods_UArray2 vid_arr = (A2Methods_UArray2) vid_array;
        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* get element at current index of vid_arr and its struct values */
        struct Video_pix *curr = methods->at(vid_arr, col, row);
        float y = curr->y;
        float pb = curr->pb;
        float pr = curr->pr;

        struct Rgb_float *pixel = (struct Rgb_float *) elem;

        /* calculate RGB values based on y, pb, and pr */
        pixel->red = (1.0 * y) + (1.402 * pr);
        pixel->green = (1.0 * y) - (0.344136 * pb) - (0.714136 * pr);
        pixel->blue = (1.0 * y) + (1.772 * pb);
        
        /* force values into the range [0, 1] */
        fix_color_range(pixel);
}

/* 
 *      name: fix_color_range
 *   purpose: forces floating-point RGB values into a range of [0, 1]
 *    inputs: pixel - a pointer to the current Rgb_float pixel containing
 *            red, green, and blue values in floating-point representation
 *   outputs: none
 *    errors: raises a checked runtime error if the pixel is NULL
 */
void fix_color_range(struct Rgb_float *pixel)
{
        assert(pixel != NULL);

        /* force red value in bounds */
        if (pixel->red < 0) {
                pixel->red = 0;
        } else if (pixel->red > 1) {
                pixel->red = 1;
        }
        
        /* force green value in bounds */
        if (pixel->green < 0) {
                pixel->green = 0;
        } else if (pixel->green > 1) {
                pixel->green = 1;
        }

        /* force blue value in bounds */
        if (pixel->blue < 0) {
                pixel->blue = 0;
        } else if (pixel->blue > 1) {
                pixel->blue = 1;
        }
}