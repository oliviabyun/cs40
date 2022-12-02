/* perform_io.c
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This is the implementation for our perform_io module, which handles UM
 * operations related to file I/O (input and output).
 */

#include <stdio.h>
#include "perform_io.h"
#include "assert.h"

/* 
 *      name: output
 *   purpose: outputs value in register c to the I/O device
 *    inputs: value - the value in register c to be outputted
 *   outputs: none
 *    errors: unchecked runtime error if value is not in the range [0, 255]
 */
void output(unsigned value)
{
        fputc(value, stdout);
}

/* 
 *      name: input
 *   purpose: obtains input from I/O device and loads register c with the input.
 *            If end of input has been signaled, register c is loaded with a 
 *            full 32-bit word where every bit is 1.
 *    inputs:     value - the value to be loaded into register c
 *            registers - the array containing the registers (uint64_t)
 *   outputs: none
 *    errors: CRE if registers equals NULL
 */
void input(unsigned rc, uint64_t *registers)
{
        assert(registers != NULL);
        int value = fgetc(stdin);

        if (value == EOF) {
                uint32_t word = ~0;
                registers[rc] = (int) word;
        } else {
                registers[rc] = value;
        }
}