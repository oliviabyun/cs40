/* memory.c
 * by Alyssa Williams (awilli36) and Olivia Byun (obyun01)
 * 11/21/22
 * 
 * This is the implementation for our memory module, which handles UM operations
 * related to memory. Functionality includes mapping and unmapping segments,
 * loading a program, and segmented loading/storing.
 */

#include <stdio.h>
#include "memory.h"
#include "assert.h"
#include "uarray.h"

/* 
 *      name: map_segment
 *   purpose: creates a new segment with the number of words equal to the value
 *            specified. Each word in the new segment is initialized to 0. 
 *            the new segment identifier is placed in register b. 
 *    inputs:              rb - register b 
 *                  num_words - number of words to have in the new segment
 *                  registers - the array containing the registers
 *                   segments - the sequence containing the segments
 *          unmapped_segments - the sequence containing the segments that have 
 *                              been unmapped
 *                   next_seg - the identifier of the next segment that has 
 *                              never been mapped to
 *   outputs: none
 *    errors: throws a CRE if registers, segments, unmapped_segments, or 
 *            next_seg is null
 */
void map_segment(unsigned rb, unsigned num_words, uint64_t *registers, 
                Seq_T segments, Seq_T unmapped_segments, int *next_seg)
{
        assert(registers != NULL);
        assert(segments != NULL);
        assert(unmapped_segments != NULL);
        assert(next_seg != NULL);
        
        UArray_T new_seg = UArray_new(num_words, sizeof(uint64_t));

        /* initialize elements to 0 */
        for (unsigned i = 0; i < num_words; i++) {
                *(uint64_t *)UArray_at(new_seg, i) = 0;
        }
        

        if (Seq_length(unmapped_segments) == 0) { /* map to a new segment */
                Seq_addhi(segments, new_seg);
                registers[rb] = *next_seg;
                (*next_seg)++;
        } else { /* reuse an unmapped segment */
                uint64_t new_id = (uintptr_t)Seq_remlo(unmapped_segments);
                Seq_put(segments, new_id, new_seg);
                registers[rb] = new_id;
        }
}

/* 
 *      name: unmap_segment
 *   purpose: Unmap a given segment in register c. After the register is
 *            unmapped, the identifier is placed into the unmapped_segments
 *            sequence so that it can be reused for future map segment
 *            instructions
 *    inputs:          seg_id - the identifier of the segment to unmap
 *                   segments - the sequence of segments 
 *          unmapped_segments - the sequence of segments that have been 
 *                              unmapped
 *   outputs: none
 *    errors: unchecked runtime error if instruction unmaps either $m[0] or a
 *            segment that is not mapped; throws a CRE if segments or
 *            unmapped_segments are NULL
 *            raises a CRE if segments is NULL
 */
void unmap_segment(unsigned seg_id, Seq_T segments, Seq_T unmapped_segments)
{
        assert(segments != NULL);
        assert(unmapped_segments != NULL);

        UArray_T seg = Seq_get(segments, seg_id);
        
        UArray_free(&seg);

        /* set segment to NULL and add to unmapped segments */
        Seq_put(segments, seg_id, NULL);
        Seq_addlo(unmapped_segments, (void *)(uintptr_t)seg_id);
}

/* 
 *      name: load_program
 *   purpose: duplicates the value in register b and replaces segment 0, which
 *            holds the instructions to execute the program.
 *    inputs:   seg_id - the identifier of the segment to duplicate
 *            segments - the sequence of segments
 *   outputs: none
 *    errors: unchecked runtime error if an instruction loads a program from a
 *            segment that is not mapped
 *            raises a CRE if segments is NULL
 */
void load_program(unsigned seg_id, Seq_T segments)
{
        assert(segments != NULL);

        if (seg_id != 0) {
                /* duplicate value in register b */
                UArray_T to_copy = Seq_get(segments, seg_id);
                UArray_T copy = UArray_copy(to_copy, UArray_length(to_copy));
                
                /* replace segment 0 and free heap-allocated memory */
                UArray_T seg0 = Seq_get(segments, 0);
                UArray_free(&seg0);
                Seq_put(segments, 0, copy);
        }
}

/* 
 *      name: segmented_load
 *   purpose: stores the value with the given segment identifier and offset 
 *            into register a
 *    inputs:        ra - register a
 *               seg_id - segment identifier of the desired value
 *               offset - word offset of the desired value
 *            registers - the array containing the registers 
 *             segments - the sequence containing the segments
 *   outputs: none
 *    errors: URE if the segment with the given identifier is unmapped 
 *            URE if the offset is outside the bounds of the mapped segment 
 *            CRE if segments or registers is NULL
 */
void segmented_load(unsigned ra, unsigned seg_id, unsigned offset, 
                    uint64_t *registers, Seq_T segments)
{
        assert(registers != NULL);
        assert(segments != NULL);

        UArray_T seg = Seq_get(segments, seg_id);
        registers[ra] = *(uint64_t *)UArray_at(seg, offset);
}

/* 
 *      name: segmented_store
 *   purpose: stores the given value at place with the given segment identifier
 *            and offset
 *    inputs:   seg_id - segment identifier of the desired location 
 *              offset - word offset of the desired location
 *               value - value to be stored
 *            segments - sequence of segments 
 *   outputs: none
 *    errors: URE if the segmented store refers to an unmapped segment
 *            URE if the segmented store refers to a location outside the bounds
 *            of a mapped segment
 *            CRE if segments is NULL
 */
void segmented_store(unsigned seg_id, unsigned offset, unsigned value, 
                        Seq_T segments)
{
        assert(segments != NULL);
        
        UArray_T seg = Seq_get(segments, seg_id);
        *(uint64_t *)UArray_at(seg, offset) = value;
}