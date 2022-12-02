/* perform_io.h
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This is the interface for our perform_io module, which handles UM operations
 * related to file I/O (input and output).
 */


#ifndef PERFORM_IO_H
#define PERFORM_IO_H

#include <stdint.h>

void output(unsigned value);
void input(unsigned rc, uint64_t *registers);

#endif