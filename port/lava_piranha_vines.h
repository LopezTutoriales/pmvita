#ifndef LAVA_PIRANHA_VINES_H
#define LAVA_PIRANHA_VINES_H

#include "common.h"

extern u8 PortLavaPiranhaVineBase[4][16];
void port_lava_piranha_set_script(s32 vine, s32 index);
void port_lava_piranha_set_script_for_dest(void* dest, s32 index);
s16* port_lava_piranha_translate(s16* animPos);

#endif
