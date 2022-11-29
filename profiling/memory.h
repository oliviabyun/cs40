/* memory.h
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This is the interface for our memory module, which handles UM operations 
 * related to memory. Functionality includes mapping and unmapping segments,
 * loading a program, and segmented loading/storing.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "seq.h"
#include <stdint.h>


void map_segment(unsigned seg_id, unsigned num_words, uint64_t *registers, 
                Seq_T segments, Seq_T unmapped_segments, int *next_seg);
void unmap_segment(unsigned seg_id, Seq_T segments, Seq_T unmapped_segments);

void load_program(unsigned seg_id, Seq_T segments);

void segmented_load(unsigned ra, unsigned seg_id, unsigned offset, 
                        uint64_t *registers, Seq_T segments);
void segmented_store(unsigned seg_id, unsigned offset, unsigned value, 
                        Seq_T segments);

#endif 