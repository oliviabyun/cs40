/* calculate.h
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This is the interface for our calculate module that handles UM operations 
 * related to calculations such as conditional move, load value, add, multiply,
 * divide, and bitwise NAND.
 */

#ifndef CALCULATE_H
#define CALCULATE_H

#include <stdint.h>

void conditional_move(unsigned ra, unsigned rb, unsigned rc, 
                        uint64_t *registers);
void load_value(unsigned ra, unsigned value, uint64_t *registers);
void add(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers);
void multiply(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers);
void divide(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers);
void bitwise_NAND(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers);

#endif