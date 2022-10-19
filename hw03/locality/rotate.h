#include "a2methods.h"
#include "pnm.h"

#ifndef ROTATE_H
#define ROTATE_H

extern const int HORIZONTAL; /* flag to flip image horizontally */
extern const int VERTICAL; /* flag to flip image vertically */
extern const int TRANSPOSE; /* flag to transpose image */

void rotate0(int col, int row, A2Methods_UArray2 uarray2, 
             A2Methods_Object *ptr, void *cl);
             
void rotate90(int col, int row, A2Methods_UArray2 uarray2, 
              A2Methods_Object *ptr, void *cl);
              
void rotate180(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl);
               
void rotate270(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl);
               
void flip_horiz(int col, int row, A2Methods_UArray2 uarray2, 
                A2Methods_Object *ptr, void *cl);
                
void flip_vert(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl);
               
void transpose(int col, int row, A2Methods_UArray2 uarray2, 
               A2Methods_Object *ptr, void *cl);
               
void rotate(int rotation, A2Methods_UArray2 uarray2, Pnm_ppm pixmap_output, 
            A2Methods_mapfun *map);

#endif 