/*
 *     decompress.h
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

void decompress(FILE *fp);