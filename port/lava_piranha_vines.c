#include "common.h"
#include "lava_piranha_vines.h"

// stand-ins for the N64 vine DMA addresses; play_model_animation swaps in the script

extern s16 LavaPiranha_ModelScript_00[];
extern s16 LavaPiranha_ModelScript_01[];
extern s16 LavaPiranha_ModelScript_02[];
extern s16 LavaPiranha_ModelScript_03[];
extern s16 LavaPiranha_ModelScript_04[];
extern s16 LavaPiranha_ModelScript_05[];
extern s16 LavaPiranha_ModelScript_06[];
extern s16 LavaPiranha_ModelScript_07[];
extern s16 LavaPiranha_ModelScript_08[];
extern s16 LavaPiranha_ModelScript_09[];
extern s16 LavaPiranha_ModelScript_0A[];
extern s16 LavaPiranha_ModelScript_0B[];
extern s16 LavaPiranha_ModelScript_0C[];
extern s16 LavaPiranha_ModelScript_0D[];
extern s16 LavaPiranha_ModelScript_0E[];
extern s16 LavaPiranha_ModelScript_0F[];
extern s16 LavaPiranha_ModelScript_10[];
extern s16 LavaPiranha_ModelScript_11[];
extern s16 LavaPiranha_ModelScript_12[];
extern s16 LavaPiranha_ModelScript_13[];
extern s16 LavaPiranha_ModelScript_14[];
extern s16 LavaPiranha_ModelScript_15[];
extern s16 LavaPiranha_ModelScript_16[];
extern s16 LavaPiranha_ModelScript_17[];
extern s16 LavaPiranha_ModelScript_18[];
extern s16 LavaPiranha_ModelScript_19[];
extern s16 LavaPiranha_ModelScript_1A[];
extern s16 LavaPiranha_ModelScript_1B[];
extern s16 LavaPiranha_ModelScript_1C[];
extern s16 LavaPiranha_ModelScript_1D[];
extern s16 LavaPiranha_ModelScript_1E[];
extern s16 LavaPiranha_ModelScript_1F[];
extern s16 LavaPiranha_ModelScript_20[];
extern s16 LavaPiranha_ModelScript_21[];
extern s16 LavaPiranha_ModelScript_22[];
extern s16 LavaPiranha_ModelScript_23[];
extern s16 LavaPiranha_ModelScript_24[];

static s16* const sVineScripts[] = {
    LavaPiranha_ModelScript_00,
    LavaPiranha_ModelScript_01,
    LavaPiranha_ModelScript_02,
    LavaPiranha_ModelScript_03,
    LavaPiranha_ModelScript_04,
    LavaPiranha_ModelScript_05,
    LavaPiranha_ModelScript_06,
    LavaPiranha_ModelScript_07,
    LavaPiranha_ModelScript_08,
    LavaPiranha_ModelScript_09,
    LavaPiranha_ModelScript_0A,
    LavaPiranha_ModelScript_0B,
    LavaPiranha_ModelScript_0C,
    LavaPiranha_ModelScript_0D,
    LavaPiranha_ModelScript_0E,
    LavaPiranha_ModelScript_0F,
    LavaPiranha_ModelScript_10,
    LavaPiranha_ModelScript_11,
    LavaPiranha_ModelScript_12,
    LavaPiranha_ModelScript_13,
    LavaPiranha_ModelScript_14,
    LavaPiranha_ModelScript_15,
    LavaPiranha_ModelScript_16,
    LavaPiranha_ModelScript_17,
    LavaPiranha_ModelScript_18,
    LavaPiranha_ModelScript_19,
    LavaPiranha_ModelScript_1A,
    LavaPiranha_ModelScript_1B,
    LavaPiranha_ModelScript_1C,
    LavaPiranha_ModelScript_1D,
    LavaPiranha_ModelScript_1E,
    LavaPiranha_ModelScript_1F,
    LavaPiranha_ModelScript_20,
    LavaPiranha_ModelScript_21,
    LavaPiranha_ModelScript_22,
    LavaPiranha_ModelScript_23,
    LavaPiranha_ModelScript_24,
};

u8 PortLavaPiranhaVineBase[4][16];
static s16* sVineCurrent[4];

void port_lava_piranha_set_script(s32 vine, s32 index) {
    if ((u32)vine < 4 && (u32)index < ARRAY_COUNT(sVineScripts)) {
        sVineCurrent[vine] = sVineScripts[index];
    }
}

void port_lava_piranha_set_script_for_dest(void* dest, s32 index) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (dest == PortLavaPiranhaVineBase[i]) {
            port_lava_piranha_set_script(i, index);
            return;
        }
    }
}

s16* port_lava_piranha_translate(s16* animPos) {
    s32 i;

    for (i = 0; i < 4; i++) {
        if (animPos == (s16*)PortLavaPiranhaVineBase[i]) {
            return sVineCurrent[i];
        }
    }
    return animPos;
}
