#include "rotate.h"
#include "assert.h"

const int HORIZONTAL = 1; /* flag to flip image horizontally */
const int VERTICAL = 2; /* flag to flip image vertically */
const int TRANSPOSE = 3; /* flag to transpose image */

/*
 *        name: rotate0 
 * description: Apply function for rotating the image 0 degrees clockwise. 
 *              Rotates a single pixel 0 degrees. 
 *  parameters:     col - the column index of the current pixel
 *                  row - the row index of the current pixel
 *              uarray2 - the UArray2 of the original image 
 *                  ptr - pointer to the current pixel 
 *                   cl - the closure argument (void pointer)
 *     returns: None
 *      errors: None
 */
void rotate0(int col, int row, A2Methods_UArray2 uarray2, 
             A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;
        
        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;
        
        /* set pixel at current index to the current pixel */
        *(Pnm_rgb)(methods->at(pixels, col, row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: rotate90 
 * description: Apply function for rotating the image 90 degrees clockwise. 
 *              Rotates a single pixel 90 degrees. 
 *  parameters:     col - the column index of the current pixel
 *                  row - the row index of the current pixel
 *              uarray2 - the UArray2 of the original image 
 *                  ptr - pointer to the current pixel 
 *                   cl - the closure argument (void pointer)
 *     returns: None
 *      errors: None
 */
void rotate90(int col, int row, A2Methods_UArray2 uarray2, 
              A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;

        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;
        
        /* the destination image has height and width reversed from source */
        unsigned height = pixmap_output->width; 
        
        /* calculate new column and row indices */
        unsigned new_col = height - row - 1;
        unsigned new_row = col;
        
        /* put current pixel at new col, row index */
        *(Pnm_rgb)(methods->at(pixels, new_col, new_row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: rotate180 
 * description: Apply function for rotating the image 180 degrees clockwise. 
 *              Rotates a single pixel 180 degrees. 
 *  parameters:     col - the column index of the current pixel
 *                  row - the row index of the current pixel
 *              uarray2 - the UArray2 of the original image 
 *                  ptr - pointer to the current pixel 
 *                   cl - the closure argument (void pointer)
 *     returns: None
 *      errors: None
 */
void rotate180(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;
        
        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;
        unsigned height = pixmap_output->height;
        unsigned width = pixmap_output->width;
        
        /* calculate new column and row indices */
        unsigned new_col = width - col - 1;
        unsigned new_row = height - row - 1;
        
        /* put current pixel at new col, row index */
        *(Pnm_rgb)(methods->at(pixels, new_col, new_row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: rotate270 
 * description: Apply function for rotating the image 270 degrees clockwise. 
 *              Rotates a single pixel 270 degrees. 
 *  parameters:     col - the column index of the current pixel
 *                  row - the row index of the current pixel
 *              uarray2 - the UArray2 of the original image 
 *                  ptr - pointer to the current pixel 
 *                   cl - the closure argument (void pointer)
 *     returns: None
 *      errors: None
 */
void rotate270(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;

        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;

        /* the destination image has height and width reversed from source */
        unsigned width = pixmap_output->height; 

        /* calculate new column and row indices */
        unsigned new_col = row;
        unsigned new_row = width - col - 1;
        
        /* put current pixel at new col, row index */
        *(Pnm_rgb)(methods->at(pixels, new_col, new_row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: flip_horiz
 * description: Apply function for flipping the image horizontally (left to 
 *              right). Flips a single pixle. 
 *  parameters:     col - the column index of the current pixel
 *                  row - the row index of the current pixel
 *              uarray2 - the UArray2 of the original image 
 *                  ptr - pointer to the current pixel 
 *                   cl - the closure argument (void pointer)
 *     returns: None
 *      errors: None
 */
void flip_horiz(int col, int row, A2Methods_UArray2 uarray2, 
                A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;

        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;
        unsigned width = pixmap_output->width; 

        /* calculate new column and row indices */
        unsigned new_col = width - col - 1;
        unsigned new_row = row;
        
        /* put current pixel at new col, row index */
        *(Pnm_rgb)(methods->at(pixels, new_col, new_row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: flip_vert
 * description: Apply function for flipping the image vertically (up to 
 *              down). Flips a single pixel. 
 *  parameters:     col - the integer column index of the current pixel
 *                  row - the integer row index of the current pixel
 *              uarray2 - the UArray2 of the original image 
 *                  ptr - pointer to the current pixel 
 *                   cl - the closure argument (void pointer)
 *     returns: None
 *      errors: None
 */
void flip_vert(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;

        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;
        unsigned height = pixmap_output->height; 

        /* calculate new column and row indices */
        unsigned new_col = col;
        unsigned new_row = height - row - 1;
        
        /* put current pixel at new col, row index */
        *(Pnm_rgb)(methods->at(pixels, new_col, new_row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: transpose
 * description: Apply function for transposing an image across the UL-LR axis.
 *              Flips a single pixel.
 *  parameters:     col - the integer column index of the current pixel
 *                  row - the integer row index of the current pixel
 *              uarray2 - the UArray2 of the original image (A2Methods_UArray2)
 *                  ptr - pointer to the current pixel (type A2Methods_Object)
 *                   cl - the closure argument (void pointer)
 *     returns: none
 *      errors: none
 */
void transpose(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl)
{
        (void)uarray2;

        /* get current Pnm_ppm instance and struct variables */
        Pnm_ppm pixmap_output = (Pnm_ppm) cl;
        const A2Methods_T methods = (A2Methods_T) pixmap_output->methods;
        A2Methods_UArray2 pixels = pixmap_output->pixels;

        /* row and column indices are simply flipped */
        unsigned new_col = row;
        unsigned new_row = col;
        
        /* put current pixel at new col, row index */
        *(Pnm_rgb)(methods->at(pixels, new_col, new_row)) = *(Pnm_rgb) ptr;
}

/*
 *        name: rotate
 * description: Applies the given transformation to the source image, putting
 *              the output in the output pixmap
 *  parameters:      rotation - the type of transformation to do
 *                    uarray2 - the UArray2 of the source image
 *              pixmap_output - the pixmap to output the image to
 *                        map - the mapping function to traverse the image with
 *     returns: None
 *      errors: throws a checked runtime error if map is NULL
 */
void rotate(int rotation, A2Methods_UArray2 uarray2, Pnm_ppm pixmap_output, 
            A2Methods_mapfun *map)
{
        assert(map != NULL);

        /* call appropriate mapping and apply function based on rotation */
        if (rotation == 0) {
                map(uarray2, rotate0, pixmap_output);
        } else if (rotation == 90) {
                map(uarray2, rotate90, pixmap_output);
        } else if (rotation == 180) {
                map(uarray2, rotate180, pixmap_output);
        } else if (rotation == 270) {
                map(uarray2, rotate270, pixmap_output);
        } else if (rotation == HORIZONTAL) {
                map(uarray2, flip_horiz, pixmap_output);
        } else if (rotation == VERTICAL) {
                map(uarray2, flip_vert, pixmap_output);
        } else if (rotation == TRANSPOSE) {
                map(uarray2, transpose, pixmap_output);
        }
}
