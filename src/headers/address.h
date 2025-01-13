/* BCST - Introduction to Computer Systems
 * Author:      hjwgfwb@gmail.com
 * Github:      https://github.com/1766hjwgfwb/csapp_stay
 * This project is to learn csapp simulator project of yangminz(QEMU)
 */

// CT, CI, CO : cache tag, cache index, cache offset
// size form i7

#ifndef ADDRESS_H
#define ADDRESS_H


#include <stdint.h>

#define SRAM_CACHE_TAG_LEN (40)
#define SRAM_CACHE_INDEX_LEN (6)
#define SRAM_CACHE_OFFSET_LEN (6)

#define PHYSICAL_PAGE_OFFSET_LEN (12)
#define PHYSICAL_PAGE_NUMBER_LEN (40)
#define PHYSICAL_ADDRESS_LEN (52)




typedef union {
    uint64_t address_value;

    // physical address: 52
    struct {
        union {
            uint64_t paddr_value : PHYSICAL_ADDRESS_LEN;
            
            struct {
                uint64_t PRO : PHYSICAL_PAGE_OFFSET_LEN;
                uint64_t PPN : PHYSICAL_PAGE_NUMBER_LEN;
            };
        };
    };

    // sram cache: 52
    struct {
        uint64_t CO: SRAM_CACHE_OFFSET_LEN;  
        uint64_t CI: SRAM_CACHE_INDEX_LEN;
        uint64_t CT: SRAM_CACHE_TAG_LEN;
    };

}address_t;















#endif /* ADDRESS_H */