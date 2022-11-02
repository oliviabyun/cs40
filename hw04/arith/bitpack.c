/*
 *     bitpack.c
 *     by Helena Lowe (hlowe01) & Olivia Byun (obyun01)
 *     arith
 *     10/26/22
 *
 *     This is the implementation for bitpack where integers can be tested
 *     to see if they fit in a certain number of bits, values of specific
 *     widths and least significant bit can be extracted from an
 *     integer field, and new values can be inserted into words.
 *     
 */

#include "except.h" 
#include "bitpack.h"
#include "assert.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

uint64_t Bitpack_shift_left(uint64_t field, unsigned shift_amount);
uint64_t Bitpack_shift_rightu(uint64_t field, unsigned shift_amount);
int64_t Bitpack_shift_rights(int64_t field, unsigned shift_amount);
int most_signif_bit(uint64_t field, unsigned width);
uint64_t compute_power_2(int exponent);

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/* 
 *      name: Bitpack_fitsu
 *   purpose: determine whether a given integer can be represented in width bits
 *    inputs:     n - a integer
 *            width - number of bits (unsigned)
 *   outputs: returns true if the argument n can be represented in width bits 
 *              in an unsigned representation, false otherwise.
 *    errors: throws a checked runtime error if width is > 64
 */
bool Bitpack_fitsu(uint64_t n, unsigned width)
{
        assert(width <= 64); 

        if (width == 0 && n == 0) {
                return true; /* fit 0 in a width of 0 */
        } else if (width == 0) {
                return false; /* can't fit nonzero val in width of 0 */
        } else if (n <= ((uint64_t)1 << width) - 1) {
                return true; /* number is less than maximum value */
        }
        
        return false;
}

/* 
 *      name: Bitpack_fitss
 *   purpose: determine whether a given integer can be represented in width bits
 *    inputs:     n - a integer
 *            width - number of bits (unsigned)
 *   outputs: returns true if the argument n can be represented in width bits 
 *              in a signed representation, false otherwise.
 *    errors: throws a checked runtime error if width is <= 0
 */
bool Bitpack_fitss(int64_t n, unsigned width)
{
        assert(width <= 64);
        
        if (width == 0) {
                if (n == 0) {
                        return true; /* fit 0 in width of 0 */
                } 
                return false; /* can't fit nonzero val in width of 0 */
        }

        /* calculate lower bound */
        int64_t lower_bound = -1 * (int64_t)(Bitpack_shift_left(1, width - 1));

        /* calculate upper bound */
        uint64_t upper_bound = (Bitpack_shift_left(1, width - 1) - 1);
        
        /* verify that provided value is in range */
        if (n >= lower_bound && n <= (int64_t)upper_bound) {
                return true;
        }

        return false;
}

/* 
 *      name: Bitpack_getu
 *   purpose: extract a field from a word given the width of the field and the 
 *              location of the field's least significant bit
 *    inputs:  word - the word to extract a field from
 *            width - width of the field to be extracted
 *              lsb - index of least significant bit
 *   outputs: returns extracted field from word given the width of the field 
 *              and the location of the field's least significant bit
 *    errors: throws a CRE if width is > 64.
 */
uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        
        if (width == 0) {
                return 0;
        }
        
        /* calculate mask */
        uint64_t mask = Bitpack_shift_left(compute_power_2(width) - 1, lsb);

        /* get desired field using mask and LSB */
        uint64_t field = Bitpack_shift_rightu(word & mask, lsb);

        return field;
}

/* 
 *      name: Bitpack_gets
 *   purpose: extract a field from a word given the width of the field and the 
 *              location of the field's least significant bit
 *    inputs:  word - the word to extract a field from
 *            width - width of the field to be extracted
 *              lsb - index of least significant bit
 *   outputs: returns extracted field from word given the width of the field 
 *              and the location of the field's least significant bit
 *    errors: throws a CRE if width is > 64.
 */
int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
        assert(width <= 64);
        (void)word;
        (void)lsb;
        
        /* field of width 0 is defined to contain the value 0 */
        if (width == 0) {
                return 0;
        }
        
        uint64_t unsigned_field = Bitpack_getu(word, width, lsb);

        if (most_signif_bit(unsigned_field, width) == 1) { /* num is negative */
                /* create mask */
                uint64_t mask = Bitpack_shift_left(compute_power_2(width) - 1, 
                                                        lsb);

                /* get desired field */
                int64_t field = Bitpack_shift_rights(word & mask, lsb);

                /* shift left then right to propagate 1's */
                field = Bitpack_shift_left(field, 64 - width);
                field = Bitpack_shift_rights(field, 64 - width);

                return field;
        } else {
                /* number is positive - can return unsigned value */
                return (int64_t)unsigned_field;
        }
}

/* 
 *      name: Bitpack_newu
 *   purpose: return a new word which is identical to the original word, except 
 *              that the field of width with least significant bit at lsb will 
 *              have been replaced by a width-bit representation of value.
 *    inputs:  word - the word we are editing
 *            width -  the width of the field in word to be replaced
 *              lsb - index of least significant bit
 *            value - the value that is replacing the field in word
 *   outputs: returns a new word which is identical to the original word, 
 *              except that the field of width with least significant bit at 
 *              lsb will have been replaced by a width-bit representation of 
 *              value.
 *    errors: width w that does not satisfy 0 <= w <= 64 (CRE)
 *                    - caught from call to fitsu
 *            width w and lsb that do not satisfy w + lsb <= 64. (CRE)
 */
uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb, 
                        uint64_t value)
{
        /* value must fit within width unsigned bits */
        if (Bitpack_fitsu(value, width) == false) {
                RAISE(Bitpack_Overflow);
        } else if ((width + lsb) > 64) {
                RAISE(Bitpack_Overflow);
        }
        
        /* calculate value to clear new desired field */
        uint64_t clear_field_value = ~(Bitpack_shift_left(
                                        compute_power_2(width) - 1, lsb));
        word = word & clear_field_value;

        /* create new word with value inserted */
        word = word | Bitpack_shift_left(value, lsb);
        
        return word;
}

/* 
 *      name: Bitpack_news
 *   purpose: return a new word which is identical to the original word, except 
 *              that the field of width with least significant bit at lsb will 
 *              have been replaced by a width-bit representation of value.
 *    inputs:  word - the word we are editing
 *            width - the width of the field in word to be replaced
 *              lsb - index of least significant bit
 *            value - the value that is replacing the field in word
 *   outputs: returns a new word which is identical to the original word, 
 *              except that the field of width with least significant bit at 
 *              lsb will have been replaced by a width-bit representation of 
 *              value.
 *    errors: width w that does not satisfy 0 <= w <= 64 (CRE) 
 *                      - caught from call to fitss
 *            width w and lsb that do not satisfy w + lsb <= 64. (CRE)
 */
uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                        int64_t value)
{
        /* value must fit within width signed bits */
        if (Bitpack_fitss(value, width) == false) {
                RAISE(Bitpack_Overflow);
        } else if ((width + lsb) > 64) {
                RAISE(Bitpack_Overflow);
        } 

        /* calculate value to clear new desired field */
        uint64_t clear_field_value = ~(Bitpack_shift_left(
                                        compute_power_2(width) - 1, lsb));
        
        word = word & clear_field_value;
        
        /* shift left and right to propagate 1's */
        uint64_t new_value = Bitpack_shift_left((uint64_t)value, 64 - width);
        new_value = Bitpack_shift_rightu(new_value, 64 - width);

        /* shift value into desired position */
        new_value = Bitpack_shift_left(new_value, lsb);

        /* create new word with value inserted */
        word = word | new_value;

        return word;
}

/* 
 *      name: Bitpack_shift_left
 *   purpose: Shift an unsigned integer left by shift_amount
 *    inputs: field - the integer to be shifted left (uint64_t)
 *            shift_amount - the number of bits to shift by (unsigned)
 *   outputs: the shifted 64 bit unsigned integer
 *    errors: throws a CRE if shift_amount is > 64
 */
uint64_t Bitpack_shift_left(uint64_t field, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64) {
                /* return largest number possible */
                return (uint64_t)1 << (shift_amount - 1);
        }

        return field << shift_amount;
}

/* 
 *      name: Bitpack_shift_rightu
 *   purpose: Shift an unsigned integer right by shift_amount
 *    inputs: field - the integer to be shifted right (uint64_t)
 *            shift_amount - the number of bits to shift by (unsigned)
 *   outputs: the shifted 64 bit unsigned integer
 *    errors: throws a CRE if shift_amount is > 64
 */
uint64_t Bitpack_shift_rightu(uint64_t field, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64) {
                return (uint64_t)0; /* smallest unsigned int is 0 */
        }

        return field >> shift_amount;
}

/* 
 *      name: Bitpack_shift_rights
 *   purpose: Shift a signed integer right by shift_amount
 *    inputs: field - the integer to be shifted right (int64_t)
 *            shift_amount - the number of bits to shift by (unsigned)
 *   outputs: the shifted 64 bit signed integer
 *    errors: throws a CRE if shift_amount is > 64
 */
int64_t Bitpack_shift_rights(int64_t field, unsigned shift_amount)
{
        assert(shift_amount <= 64);

        if (shift_amount == 64) {
                if (most_signif_bit(field, 64) == 0) { /* MSB is 0 */
                        return (int64_t)0;
                } else { /* most significant bit is 1 */
                        /* return most negative number possible */
                        return ((int64_t)1 << 62); 
                }
        }

        return (field >> shift_amount);
}

/* 
 *      name: most_signif_bit
 *   purpose: given an unsigned 64 bit integer, determines and returns the
 *            most significant bit in the integer; used to determine if a
 *            number is negative or positive in a signed representation
 *    inputs: field - the current 64-bit unsigned integer to calculate the
 *                    most significant bit of
 *            width - the width of the current 64-bit unsigned integer
 *   outputs: 1 if the most significant bit is a 1 (negative #), 0 otherwise
 *    errors: throws a CRE if width is greater than 64
 */
int most_signif_bit(uint64_t field, unsigned width)
{
        assert(width <= 64);

        /* obtain number with 1 in most significant bit place */
        uint64_t msb = (uint64_t)1 << (width - 1);
        
        /* determine if current field has a leading 1: negative */
        if ((field & msb) == msb) {
                return 1;
        }

        return 0; /* most significant bit is 0: positive */
}

/* 
 *      name: compute_power_2
 *   purpose: calculates powers of 2; given an exponent, calculates the integer
 *            2 raised to the exponent
 *    inputs: exponent - the power to raise 2 to (integer)
 *   outputs: the calculated power of 2 (of type uint64_t)
 *    errors: throws a CRE if the exponent is less than 0
 */
uint64_t compute_power_2(int exponent)
{
        assert(exponent >= 0);
        
        return Bitpack_shift_left((uint64_t)1, exponent);
}