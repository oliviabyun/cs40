/* calculate.c
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This is the implementation for our calculate module that handles UM
 * operations related to calculations such as conditional move, load value,
 * add, multiply, divide, and bitwise NAND.
 */

#include <stdio.h>
#include "calculate.h"
#include <assert.h>

/* 
 *      name: conditional_move
 *   purpose: if the value in register c does not equal 0, then the value in 
 *            register b is stored in register a
 *    inputs:        ra - register a
 *                   rb - register b
 *                   rc - register c
 *            registers - the array containing the registers
 *   outputs: none
 *    errors: throws a CRE if registers is null
 */
void conditional_move(unsigned ra, unsigned rb, unsigned rc, 
                        uint64_t *registers)
{
        assert(registers != NULL);
        
        if (registers[rc]) {
                registers[ra] = registers[rb];
        }
}

/* 
 *      name: load_value
 *   purpose: the given value is stored in the given register a
 *    inputs:       ra - register a
 *               value - the value to store in register a
 *           registers - the array containing the registers
 *   outputs: none
 *    errors: throws a CRE if registers is null
 */
void load_value(unsigned ra, unsigned value, uint64_t *registers)
{
        assert(registers != NULL);
        registers[ra] = value;        
}

/* 
 *      name: add
 *   purpose: adds the values in register b and c together and stores the sum 
 *            in register a. The sum is modded by 2^32 
 *            in register a
 *    inputs:       ra - register a
 *                  rb - register b
 *                  rc - register c
 *           registers - the array containing the registers
 *   outputs: none
 *    errors: throws a CRE if registers is null
 */
void add(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers)
{
        assert(registers != NULL);
        registers[ra] = (registers[rb] + registers[rc]) % ((uint64_t)2 << 32);
}

/* 
 *      name: multiply
 *   purpose: multiplies the values in register b and c together and stores the
 *            product in register a. The product is modded by 2^32
 *    inputs:       ra - register a
 *                  rb - register b
 *                  rc - register c
 *           registers - the array containing the registers 
 *   outputs: none
 *    errors: throws a CRE if registers is null
 */
void multiply(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers)
{
        assert(registers != NULL);
        registers[ra] = (registers[rb] * registers[rc]) % ((uint64_t)2 << 32);
}

/* 
 *      name: divide
 *   purpose: divides the value in register b by the value in register c and 
 *            stores the quotient in register a
 *    inputs:       ra - register a
 *                  rb - register b
 *                  rc - register c
 *           registers - the array containing the registers
 *   outputs: none
 *    errors: throws a CRE if registers is null
 *            URE for division by zero
 */
void divide(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers)
{
        assert(registers != NULL);
        registers[ra] = registers[rb] / registers[rc];
}

/* 
 *      name: bitwise_NAND
 *   purpose: performs the bitwise operation nand on the values in register b
 *            and c and stores the result in register a
 *    inputs:       ra - register a
 *                  rb - register b
 *                  rc - register c
 *           registers - the array containing the registers
 *   outputs: none
 *    errors: throws a CRE if registers is null
 */
void bitwise_NAND(unsigned ra, unsigned rb, unsigned rc, uint64_t *registers)
{
        assert(registers != NULL);
        registers[ra] = ~(registers[rb] & registers[rc]);
}