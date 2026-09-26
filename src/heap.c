#include "common.h"

extern HeapNode heap_generalHead;
extern HeapNode heap_collisionHead;

HeapNode* general_heap_create(void) {
#ifdef PORT
    // Wait for audio thread to finish before resetting the heap.
    // The audio thread reads sample data from bank pointers in the heap;
    // resetting while audio is processing causes SIGSEGV.
    extern void port_flush_audio(void);
    port_flush_audio();
#endif
    return _heap_create(&heap_generalHead, GENERAL_HEAP_SIZE);
}

void* general_heap_malloc(s32 size) {
    return _heap_malloc(&heap_generalHead, size);
}

void* general_heap_malloc_tail(s32 size) {
    return _heap_malloc_tail(&heap_generalHead, size);
}

s32 general_heap_free(void* data) {
    return _heap_free(&heap_generalHead, data);
}

s32 battle_heap_create(void) {
    if (_heap_create(&heap_battleHead, BATTLE_HEAP_SIZE) == (HeapNode*)-1) {
        return -1;
    } else {
        return 0;
    }
}

s32 func_8002ACDC(void) {
    return 0;
}

void* heap_malloc(s32 size) {
    if (gGameStatusPtr->context == CONTEXT_WORLD) {
        return general_heap_malloc(size);
    } else {
        return _heap_malloc(&heap_battleHead, size);
    }
}

s32 heap_free(void* data) {
#ifdef PORT
    // heap_malloc picks its arena from the game context when allocating, and this picked it again
    // from the context when freeing. Anything allocated in one context and freed in another went
    // to the wrong arena: the badge crash, status icon popups, image fx colour buffers and hud
    // elements all did this. Free a block to the arena it actually lives in.
    {
        u8* p = (u8*)data;
        u8* gen = (u8*)&heap_generalHead;
        u8* btl = (u8*)&heap_battleHead;

        if (p >= gen && p < gen + GENERAL_HEAP_SIZE) {
            return general_heap_free(data);
        }
        if (p >= btl && p < btl + BATTLE_HEAP_SIZE) {
            return _heap_free(&heap_battleHead, data);
        }
    }
#endif
    if (gGameStatusPtr->context != CONTEXT_WORLD) {
        return _heap_free(&heap_battleHead, data);
    } else {
        return general_heap_free(data);
    }
}

s32 collision_heap_create(void) {
    if (_heap_create(&heap_collisionHead, COLLISION_HEAP_SIZE) == (HeapNode*)-1) {
        return -1;
    }
    return 0;
}

void* collision_heap_malloc(s32 size) {
    if (gGameStatusPtr->context == CONTEXT_WORLD) {
        return _heap_malloc(&heap_collisionHead, size);
    } else {
        return _heap_malloc(&heap_battleHead, size);
    }
}

s32 collision_heap_free(void* data) {
    if (gGameStatusPtr->context != CONTEXT_WORLD) {
        return _heap_free(&heap_battleHead, data);
    } else {
        return _heap_free(&heap_collisionHead, data);
    }
}
