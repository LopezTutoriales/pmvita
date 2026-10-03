enum {
    VINE_0      = 0,
    VINE_1      = 1,
    VINE_2      = 2,
    VINE_3      = 3,
    NUM_VINES   = 4
};

#ifdef PORT
#include "lava_piranha_vines.h"
#define VINE_0_BASE (intptr_t) PortLavaPiranhaVineBase[0]
#define VINE_1_BASE (intptr_t) PortLavaPiranhaVineBase[1]
#define VINE_2_BASE (intptr_t) PortLavaPiranhaVineBase[2]
#define VINE_3_BASE (intptr_t) PortLavaPiranhaVineBase[3]
#elif defined(SHIFT)
extern Addr D_80200000;
extern Addr D_80204000;
extern Addr D_80207000;
extern Addr D_8020A000;
#define VINE_0_BASE (intptr_t) &D_80200000
#define VINE_1_BASE (intptr_t) &D_80204000
#define VINE_2_BASE (intptr_t) &D_80207000
#define VINE_3_BASE (intptr_t) &D_8020A000
#else
#define VINE_0_BASE 0x80200000
#define VINE_1_BASE 0x80204000
#define VINE_2_BASE 0x80207000
#define VINE_3_BASE 0x8020A000
#endif

#include "world/common/lava_piranha/skele1.c"
#include "world/common/lava_piranha/skele2.c"
#include "world/common/lava_piranha/skele3.c"
