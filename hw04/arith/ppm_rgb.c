/*
 *     ppm_rgb.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the implementation for ppm_rgb, where 2D pixel arrays can be 
 *     transformed to/from a scaled integer representation and a floating point
 *     representation. Users can also trim the last row/column of an image or
 *     print a decompressed/regular PPM image to standard output.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "ppm_rgb.h"

void apply_int(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *float_array);
void apply_float(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *pnmppm);

/* 
 *      name: ppmrgb_decompress
 *   purpose: convert the given UArray2 of pixels of type float back to 
 *            scaled ints in a Pnm_ppm. print the Pnm_ppm to stdout.
 *    inputs: transformed - the UArray2 of pixels of type float
 *   outputs: none
 *    errors: raises a checked runtime error if the provided 2D array is NULL
 */
void ppmrgb_decompress(A2Methods_UArray2 transformed)
{
        assert(transformed != NULL);

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);
        
        /* convert pixels from floating point to integer */
        Pnm_ppm image = float_to_int(transformed);
        
        /* print the ppm to stdout */
        print(image);
        
        /* free image */
        Pnm_ppmfree(&image);
}

/* 
 *      name: print
 *   purpose: print the given Pnm_ppm to standard output
 *    inputs: transformed - pointer to Pnm_ppm which contains a UArray2 with 
 *            pixels of type unsigned
 *   outputs: none
 *    errors: raises a CRE if provided Pnm_ppm is NULL
 */
void print(Pnm_ppm transformed)
{
        assert(transformed != NULL);

        /* write ppm to standard output */
        Pnm_ppmwrite(stdout, transformed);
}

/* 
 *      name: ppmrgb_compress
 *   purpose: read ppm from given file, trim if needed so that the width and 
 *            height are even, convert from scaled ints to floating point
 *    inputs: fp - pointer to beginning of file to be compressed
 *   outputs: A uarray2 of pixels with type float
 *    errors: raises a checked runtime error if the methods suite is null
 */
A2Methods_UArray2 ppmrgb_compress(FILE *fp) 
{
        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        Pnm_ppm pixmap = Pnm_ppmread(fp, methods); /* read in PPM image */

        /* trim the image as needed */
        pixmap = trim(pixmap); 

        /* transform to floating point representation */
        A2Methods_UArray2 float_arr = int_to_float(pixmap);

        Pnm_ppmfree(&pixmap); /* free heap-allocated memory */

        return float_arr;
}

/* 
 *      name: trim
 *   purpose: trim last row or column of image if height or width is odd.
 *    inputs: image - pointer to Pnm_ppm which contains a UArray2 with pixels
 *            of type unsigned
 *   outputs: The trimmed image.
 *    errors: raises a checked runtime error if the image is NULL
 */
Pnm_ppm trim(Pnm_ppm image)
{
        assert(image != NULL);
        
        /* if width is odd, trim the last column */
        if ((image->width % 2) == 1) {
                image->width = image->width - 1;
        }

        /* if height is odd, trim the last row */
        if ((image->height % 2) == 1) {
                image->height = image->height - 1;
        }

        return image;
}

/* 
 *      name: int_to_float
 *   purpose: convert the given Pnm_ppm with UArray2 with pixels of type
 *            unsigned to a UArray2 with pixels of type float
 *    inputs: image - pointer to Pnm_ppm contained a UArray2 with pixels of
 *            type unsigned
 *   outputs: A 2D array with pixels of type float
 *    errors: raises a CRE if the provided Pnm_ppm is NULL
 */
A2Methods_UArray2 int_to_float(Pnm_ppm image)
{
        assert(image != NULL);
        
        /* get methods suite */
        const A2Methods_T methods = (const A2Methods_T)image->methods;
        assert(methods != NULL);
        
        /* make new 2D array to hold pixels in floating-point form */
        A2Methods_UArray2 float_arr = methods->new(image->width, image->height,
                                        sizeof(struct Rgb_float));
        
        /* transform each pixel from scaled int to floating point */
        methods->map_col_major(float_arr, apply_float, image);

        return float_arr;
}

/* 
 *      name: float_to_int
 *   purpose: given a 2D array in floating point representation, converts its
 *            elements to a scaled integer representation
 *    inputs: float_arr - the 2D array containing pixels in floating-point
 *            representation to be converted to scaled integer representation
 *   outputs: A Pnm_ppm image containing each of the pixels
 *    errors: raises a checked runtime error if the provided float_arr is NULL
 *            or if memory allocation fails for Pnm_ppm int_image
 */
Pnm_ppm float_to_int(A2Methods_UArray2 float_arr)
{
        assert(float_arr != NULL);
        
        /* create the new Pnm_ppm to be outputted */
        Pnm_ppm int_image = malloc(sizeof(struct Pnm_ppm));
        assert(int_image != NULL);
        
        /* set the methods and max pixel value of the PPM image */
        int_image->methods = uarray2_methods_plain;
        int_image->denominator = 255;

        /* set the height and width of the new PPM image */
        int width = int_image->methods->width(float_arr);
        int height = int_image->methods->height(float_arr);
        int_image->width = width;
        int_image->height = height;
        
        /* create a 2D array to hold pixels with integer values */
        A2Methods_UArray2 int_arr = int_image->methods->new(width, height, 
                                        sizeof(struct Pnm_rgb));
        int_image->methods->map_col_major(int_arr, apply_int, float_arr);
        
        int_image->pixels = int_arr;
        
        return int_image;
}

/* 
 *      name: apply_float
 *   purpose: apply function to convert each pixel to a floating-point
 *            representation
 *    inputs:   col - the column index of the current element (integer)
 *              row - the row index of the current element (integer)
 *            array - the new 2D array containing Rgb_float struct elements
 *                    where the RGB values are floats
 *             elem - a void pointer to the current Rgb_float element in the
 *                    2D array
 *           pnmppm - a void pointer to a Pnm_ppm instance containing the
 *                    current image data
 *   outputs: none
 *    errors: raises a checked runtime error if the closure argument or the
 *            current element are NULL
 */
void apply_float(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *pnmppm)
{
        (void)array;
        assert(pnmppm != NULL);
        assert(elem != NULL);

        /* get Pnm_ppm image and its corresponding 2D array from closure */
        Pnm_ppm image = (Pnm_ppm) pnmppm;
        A2Methods_UArray2 pixels = image->pixels;

        /* get methods suite and max pixel value from image*/
        const A2Methods_T methods = (const A2Methods_T)image->methods;
        assert(methods != NULL);
        unsigned denom = image->denominator;
        
        /* get current pixel in 2D array and current element */
        Pnm_rgb curr = methods->at(pixels, col, row);
        struct Rgb_float *current = (struct Rgb_float *) elem;
        
        /* cast to float from unsigned and set RGB pixel values */
        current->red = (float)curr->red / denom;
        current->green = (float)curr->green / denom;
        current->blue = (float)curr->blue / denom;
}

/* 
 *      name: apply_int
 *   purpose: apply function to convert each pixel from a floating-point
 *            representation back to a scaled integer representation
 *    inputs:     col - the column index of the current element (integer)
 *                row - the row index of the current element (integer)
 *              array - the 2D array containing Pnm_rgb struct elements
 *                      where the RGB values are integers
 *               elem - a void pointer to the current Pnm_rgb element in the
 *                    2D array
 *        float_array - a void pointer to a A2Methods_UArray2 instance 
 *                   containing the current floating-point pixel representations
 *   outputs: none
 *    errors: raises a checked runtime error if the closure argument or the
 *            current element are NULL
 */
void apply_int(int col, int row, A2Methods_UArray2 array, void *elem, 
                void *float_array)
{
        (void)array;
        assert(elem != NULL);
        assert(float_array != NULL);
        
        /* get UArray2 of pixels of type float from closure */
        A2Methods_UArray2 float_arr = (A2Methods_UArray2) float_array;

        A2Methods_T methods = uarray2_methods_plain; /* get methods suite */
        assert(methods != NULL);

        /* set new denominator and get Rgb_float element at current index */
        int denominator = 255;
        struct Rgb_float *curr = methods->at(float_arr, col, row);
        
        struct Pnm_rgb *current = (struct Pnm_rgb *) elem;

        /* cast to unsigned from float and set RGB pixel values */
        current->red = (unsigned)(curr->red * denominator);
        current->green = (unsigned)(curr->green * denominator);
        current->blue = (unsigned)(curr->blue * denominator);
}