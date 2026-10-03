#include "common.h"
#include "model.h"
#include "port_aspect.h"
#include <string.h>

// end_00/end_01 parade backdrops are only 4:3 wide; widescreen redraws them shifted to fill the sides

#define BACKDROP_WIDTH    1500.0f
#define GROUND_TILE_WIDTH 750.0f
#define ROAD_STRIP_WIDTH  3750.0f

#define FIRST_SLOT CUSTOM_GFX_4
#define NUM_SLOTS  16

// treeIndex is the map's MODEL_* id; the port's model name list holds raw N64 pointers
typedef struct ParadeCopy {
    s32 treeIndex;
    f32 stepX;
    s32 count;
} ParadeCopy;

typedef struct ParadeMap {
    const char* mapName;
    const ParadeCopy* copies;
    s32 numCopies;
} ParadeMap;

// end_00: sky 0x0, cloud 0x2, mountain 0x4, j2 0x7, j1 0x11, j3_b 0x12, j3 0x13, j25 0x31, o226 0x32, j27 0x33
static const ParadeCopy sEnd00Copies[] = {
    { 0x00, -BACKDROP_WIDTH, 1 },    { 0x00, BACKDROP_WIDTH, 1 },        { 0x02, -BACKDROP_WIDTH, 1 },
    { 0x02, BACKDROP_WIDTH, 1 },     { 0x04, -BACKDROP_WIDTH, 1 },       { 0x04, BACKDROP_WIDTH, 1 },
    { 0x11, -GROUND_TILE_WIDTH, 2 }, { 0x13, -GROUND_TILE_WIDTH, 2 },    { 0x12, -GROUND_TILE_WIDTH, 2 },
    { 0x07, -ROAD_STRIP_WIDTH, 1 },  { 0x31, GROUND_TILE_WIDTH, 2 },     { 0x32, GROUND_TILE_WIDTH, 2 },
    { 0x33, GROUND_TILE_WIDTH, 2 },
};

// end_01: sky 0x0, mountain 0x2, o145 0x4, o146 0x5, j2 0x6
static const ParadeCopy sEnd01Copies[] = {
    { 0x00, -BACKDROP_WIDTH, 1 },    { 0x00, BACKDROP_WIDTH, 1 },        { 0x02, -BACKDROP_WIDTH, 1 },
    { 0x02, BACKDROP_WIDTH, 1 },     { 0x04, -GROUND_TILE_WIDTH, 2 },    { 0x05, -GROUND_TILE_WIDTH, 2 },
    { 0x06, -GROUND_TILE_WIDTH, 2 },
};

static const ParadeMap sParadeMaps[] = {
    { "end_00", sEnd00Copies, ARRAY_COUNT(sEnd00Copies) },
    { "end_01", sEnd01Copies, ARRAY_COUNT(sEnd01Copies) },
};

typedef struct ParadeSlot {
    Model* model;
    const ParadeCopy* copies[4];
    s32 numCopies;
} ParadeSlot;

static ParadeSlot sSlots[NUM_SLOTS];

static void append_copy(Gfx* dl, f32 dx) {
    Matrix4f mtx;
    Mtx* dispMtx = &gDisplayContext->matrixStack[gMatrixListPos++];

    guTranslateF(mtx, dx, 0.0f, 0.0f);
    guMtxF2L(mtx, dispMtx);
    gSPMatrix(gMainGfxPos++, dispMtx, G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_PUSH);
    gSPDisplayList(gMainGfxPos++, dl);
    gSPPopMatrix(gMainGfxPos++, G_MTX_MODELVIEW);
}

static void build_copies(s32 index) {
    ParadeSlot* slot;
    Gfx* dl;
    s32 i, j;

    if (index < 0 || index >= NUM_SLOTS || gPortWindowAspectRatio <= 4.0f / 3.0f + 0.01f) {
        return;
    }
    slot = &sSlots[index];
    if (slot->model == NULL || slot->model->modelNode == NULL || slot->model->modelNode->displayData == NULL) {
        return;
    }

    dl = slot->model->modelNode->displayData->displayList;
    if (dl == NULL) {
        return;
    }
    for (i = 0; i < slot->numCopies; i++) {
        for (j = 1; j <= slot->copies[i]->count; j++) {
            append_copy(dl, slot->copies[i]->stepX * j);
        }
    }
}

static ParadeSlot* claim_slot(Model* model, s32* nextSlot) {
    s32 i;

    for (i = FIRST_SLOT; i < *nextSlot; i++) {
        if (sSlots[i].model == model) {
            return &sSlots[i];
        }
    }
    if (*nextSlot >= NUM_SLOTS) {
        return NULL;
    }

    i = (*nextSlot)++;
    sSlots[i].model = model;
    sSlots[i].numCopies = 0;
    set_mdl_custom_gfx_set(model, i, ENV_TINT_UNCHANGED);
    model->flags |= MODEL_FLAG_USES_CUSTOM_GFX;
    set_custom_gfx_builders(i, NULL, build_copies);
    return &sSlots[i];
}

void port_parade_backdrop_setup(const char* mapName) {
    const ParadeMap* map = NULL;
    s32 nextSlot = FIRST_SLOT;
    u32 i;

    memset(sSlots, 0, sizeof(sSlots));
    if (mapName == NULL) {
        return;
    }
    for (i = 0; i < ARRAY_COUNT(sParadeMaps); i++) {
        if (strcmp(mapName, sParadeMaps[i].mapName) == 0) {
            map = &sParadeMaps[i];
        }
    }
    if (map == NULL) {
        return;
    }

    for (i = 0; i < (u32)map->numCopies; i++) {
        const ParadeCopy* copy = &map->copies[i];
        Model* model;
        ParadeSlot* slot;

        model = get_model_from_list_index(get_model_list_index_from_tree_index(copy->treeIndex));
        if (model == NULL || model->modelNode == NULL) {
            continue;
        }
        slot = claim_slot(model, &nextSlot);
        if (slot != NULL && slot->numCopies < (s32)ARRAY_COUNT(slot->copies)) {
            slot->copies[slot->numCopies++] = copy;
        }
    }
}
