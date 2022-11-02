#include "bitpack.h"
#include "assert.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
        /* test fitsu function */
        assert(Bitpack_fitsu(5, 3) == true);
        assert(Bitpack_fitsu(0, 0) == true);
        assert(Bitpack_fitsu(1, 0) == false);

        /* test fitss function */
        assert(Bitpack_fitss(0, 0) == true);
        assert(Bitpack_fitss(1, 0) == false);
        
        assert(Bitpack_fitss(5, 3) == false);
        assert(Bitpack_fitss(2, 3) == true);
        assert(Bitpack_fitss(1, 2) == true);
        assert(Bitpack_fitss(-1, 2) == true);

        /* test getu function */
        assert(Bitpack_getu(10, 2, 0) == 2);
        assert(Bitpack_getu(10, 2, 2) == 2);
        assert(Bitpack_getu(10, 0, 2) == 0);
        assert(Bitpack_getu(0x3f4, 6, 2) == 61);

        /* test gets function */
        assert(Bitpack_gets(10, 2, 0) == -2);
        assert(Bitpack_gets(25, 3, 2) == -2);
        assert(Bitpack_gets(17, 3, 2) == -4);
        assert(Bitpack_gets(0x3f4, 6, 2) == -3);

        /* test newu function */
        assert(Bitpack_newu(45, 3, 1, 5) == 43);
        //assert(Bitpack_newu(45, 65, 1, 5) == 43); //CRE
        assert(Bitpack_newu(141, 0, 2, 0) == 141);

        /* test news function */
        assert(Bitpack_news(141, 3, 2, -3) == 149);
        assert(Bitpack_news(141, 0, 2, 0) == 141);
        
        /* law testing (combinations) */
        /* law: Bitpack_getu(Bitpack_newu(word, w, lsb, val), w, lsb) == val */
        assert(Bitpack_getu(Bitpack_newu(45, 3, 1, 5), 3, 1) == 5);
        /* law: getu(newu(word, w, lsb, val), w2, lsb2) 
                                        == getu(word, w2, lsb2) */
        assert(Bitpack_getu(Bitpack_newu(45, 3, 1, 5), 1, 0) == 
                                                Bitpack_getu(45, 1, 0));

        /* large tests */
        uint64_t large_num = (uint64_t)1 << 63;
        uint64_t test_word = Bitpack_news(large_num, 2, 2, 1);
        assert(Bitpack_gets(test_word, 1, 63) == 
                                        Bitpack_gets(large_num, 1, 63));
        
        printf("success! :)\n");
}