#include <libultraship.h>
#include <libultra/types.h>
#include <align_asset_macro.h>
#include <macros.h>
#include <string.h>
#include <common_structs.h>
#include <segments.h>
#include <decode.h>
#include <stubs.h>

#include "memory.h"
#include "main.h"
#include "code_800029B0.h"
#include "defines.h"

#include "course_offsets.h"

#include "engine/courses/Course.h"

#include <stdio.h>

#include "port/Game.h"

s32 sGfxSeekPosition;
s32 sPackedSeekPosition;

/* Opcodes packés (parité avec tools/displaylist_packer.c) */
enum PackedOp {
    PG_LIGHTS_0               = 0x00, /* 0..0x14 mappés sur unpack_lights */
    /* Presets de combine renommés pour refléter les macros G_CC_* */
    PG_SETCOMBINE_CC_MODULATERGBA      = 0x15,
    PG_SETCOMBINE_CC_MODULATERGBDECALA = 0x16,
    PG_SETCOMBINE_CC_SHADE             = 0x17,
    PG_RMODE_OPA             = 0x18,
    PG_RMODE_TEXEDGE         = 0x19,
    PG_TILECFG_A             = 0x1A,
    PG_TILECFG_B             = 0x1B,
    PG_TILECFG_C             = 0x1C,
    PG_TILECFG_D             = 0x1D,
    PG_TILECFG_E             = 0x1E,
    PG_TILECFG_F             = 0x1F,
    PG_TIMG_LOADBLOCK_0      = 0x20,
    PG_TIMG_LOADBLOCK_1      = 0x21,
    PG_TIMG_LOADBLOCK_2      = 0x22,
    PG_TIMG_LOADBLOCK_3      = 0x23,
    PG_TIMG_LOADBLOCK_4      = 0x24,
    PG_TIMG_LOADBLOCK_5      = 0x25,
    PG_TEXTURE_ON            = 0x26,
    PG_TEXTURE_OFF           = 0x27,
    PG_VTX1                  = 0x28,
    PG_TRI1                  = 0x29,
    PG_ENDDL                 = 0x2A,
    PG_DL                    = 0x2B,
    PG_TILECFG_G             = 0x2C,
    PG_CULLDL                = 0x2D,
    PG_SETCOMBINE_ALT        = 0x2E,
    PG_RMODE_XLU             = 0x2F,
    PG_SPLINE3D              = 0x30,
    PG_VTX_BASE              = 0x32, /* 0x33..0x52 → variant vtx2 */
    PG_SETCOMBINE_CC_DECALRGBA  = 0x53,
    PG_RMODE_OPA_DECAL       = 0x54,
    PG_RMODE_XLU_DECAL       = 0x55,
    PG_SETGEOMETRYMODE       = 0x56,
    PG_CLEARGEOMETRYMODE     = 0x57,
    PG_TRI2                  = 0x58,
    PG_EOF                   = 0xFF,
};

static u8 sMemoryPool[0xFFFFFFF]; // Stock memory pool size: 0xAB630
uintptr_t sPoolEnd = sMemoryPool + sizeof(sMemoryPool);

uintptr_t sPoolFreeSpace;
struct MainPoolBlock* sPoolListHeadL;
struct MainPoolBlock* sPoolListHeadR;

struct MainPoolState* gMainPoolState = NULL;

struct UnkStruct_802B8CD4 D_802B8CD4[] = { 0 };
s32 D_802B8CE4 = 0; // pad
s32 memoryPadding[2];

#define PRINT_MEMPOOL                                                                                                       \
    printf("\nPool Start: %p, Pool End: %p, size: 0x%lX\ngNextFreeMemoryAddress: 0x%lX\n\n", (void*) sMemoryPool,   \
           (void*) (sMemoryPool + sizeof(sMemoryPool)),                                                                      \
           (unsigned long) (((sMemoryPool + sizeof(sMemoryPool)) - sMemoryPool)),                                           \
           (unsigned long) gNextFreeMemoryAddress)

/**
 * @brief Returns the address of the next available memory location and updates the memory pointer
 * to reference the next location of available memory based provided size to allocate.
 * @param size of memory to allocate.
 * @return Address of free memory
 */
void* get_next_available_memory_addr(uintptr_t size) {
    uintptr_t freeSpace = (uintptr_t) gNextFreeMemoryAddress;
    size = ALIGN16(size);
    gNextFreeMemoryAddress += size;

    if (gNextFreeMemoryAddress > sPoolEnd) {
        printf("[memory.c] get_next_available_memory_addr(): Memory Pool Out of Bounds! Out of memory!\n");
        PRINT_MEMPOOL;
    }

    return (void*) freeSpace;
    return freeSpace;
}

static uintptr_t get_texture2(size_t offset, const course_texture* textures) {
    if (!((offset >= 0x5000000) && (offset < 0x6000000))) {
        return NULL;
    }
    size_t totalOffset = 0x5000000;

    while (textures->addr) {
        if (totalOffset == offset) {
            return (uintptr_t) (textures->addr);
        }
        totalOffset += textures->data_size;
        textures++;
    }

    printf("memory.c: get_texture()\n  TEXTURE NOT FOUND DURING DISPLAYLIST EXTRACT\n");
    printf("  offset: 0x%zX\n", offset);
    return NULL;
}

/**
 * @brief Sets the starting location for allocating memory and calculates pool size.
 *
 * Default memory size, 701.984 Kilobytes.
 */
void initialize_memory_pool() {

    uintptr_t poolStart = sMemoryPool;
    // uintptr_t sPoolEnd = sMemoryPool + sizeof(sMemoryPool);

    bzero(sMemoryPool, sizeof(sMemoryPool));

    poolStart = ALIGN16(poolStart);
    // Truncate to a 16-byte boundary.
    sPoolEnd &= ~0xF;

    gFreeMemorySize = (sPoolEnd - poolStart) - 0x10;
    gNextFreeMemoryAddress = poolStart;

    PRINT_MEMPOOL;
}

/**
 * @brief Allocates memory and adjusts gFreeMemorySize.
 */
void* allocate_memory(size_t size) {
    uintptr_t freeSpace;

    size = ALIGN16(size);
    if (gFreeMemorySize < size) {
        printf("[memory.c] allocate_memory(): gFreeMemorySize below zero!\n");
        printf("gFreeMemorySize before alloc: 0x%zX (requested: 0x%zX)\n", gFreeMemorySize, size);
        PRINT_MEMPOOL;
    }
    gFreeMemorySize -= size;

    freeSpace = (uintptr_t) gNextFreeMemoryAddress;
    gNextFreeMemoryAddress += size;

    if (gNextFreeMemoryAddress > sPoolEnd) {
        printf("[memory.c] allocate_memory(): Memory Pool Out of Bounds! Out of memory!\n");
        PRINT_MEMPOOL;
    }

    return (void*) freeSpace;
}

UNUSED void func_802A7D54(s32 arg0, s32 arg1) {
    gD_80150158[arg0].unk0 = arg0;
    gD_80150158[arg0].unk8 = arg1;
}

/**
 * @brief Allocate and DMA.
 */
void* load_data(uintptr_t startAddr, uintptr_t endAddr) {
    void* allocated;
    uintptr_t size = endAddr - startAddr;

    allocated = allocate_memory(size);
    if (allocated != 0) {
        dma_copy((u8*) allocated, (u8*) startAddr, size);
    }
    return (void*) allocated;
}

UNUSED void main_pool_init(uintptr_t start, uintptr_t end) {
    start = ALIGN16(start);
    end = ALIGN16(end - 15);

    sPoolFreeSpace = (end - start) - 16;

    sPoolListHeadL = (struct MainPoolBlock*) start;
    sPoolListHeadR = (struct MainPoolBlock*) end;
    sPoolListHeadL->prev = NULL;
    sPoolListHeadL->next = NULL;
    sPoolListHeadR->prev = NULL;
    sPoolListHeadR->next = NULL;
}

/**
 * Allocate a block of memory from the pool of given size, and from the
 * specified side of the pool (MEMORY_POOL_LEFT or MEMORY_POOL_RIGHT).
 * If there is not enough space, return NULL.
 */
UNUSED void* main_pool_alloc(uintptr_t size, uintptr_t side) {
    struct MainPoolBlock* newListHead;
    void* addr = NULL;

    size = ALIGN16(size) + 8;
    if (sPoolFreeSpace >= size) {
        sPoolFreeSpace -= size;
        if (side == MEMORY_POOL_LEFT) {
            newListHead = (struct MainPoolBlock*) ((u8*) sPoolListHeadL + size);
            sPoolListHeadL->next = newListHead;
            newListHead->prev = sPoolListHeadL;
            addr = (u8*) sPoolListHeadL + 8;
            sPoolListHeadL = newListHead;
        } else {
            newListHead = (struct MainPoolBlock*) ((u8*) sPoolListHeadR - size);
            sPoolListHeadR->prev = newListHead;
            newListHead->next = sPoolListHeadR;
            sPoolListHeadR = newListHead;
            addr = (u8*) sPoolListHeadR + 8;
        }
    }
    return addr;
}
/**
 * Free a block of memory that was allocated from the pool. The block must be
 * the most recently allocated block from its end of the pool, otherwise all
 * newer blocks are freed as well.
 * Return the amount of free space left in the pool.
 */
UNUSED uintptr_t main_pool_free(void* addr) {
    struct MainPoolBlock* block = (struct MainPoolBlock*) ((u8*) addr - 8);
    struct MainPoolBlock* oldListHead = (struct MainPoolBlock*) ((u8*) addr - 8);

    if (oldListHead < sPoolListHeadL) {
        while (oldListHead->next != NULL) {
            oldListHead = oldListHead->next;
        }
        sPoolListHeadL = block;
        sPoolListHeadL->next = NULL;
        sPoolFreeSpace += (uintptr_t) oldListHead - (uintptr_t) sPoolListHeadL;
    } else {
        while (oldListHead->prev != NULL) {
            oldListHead = oldListHead->prev;
        }
        sPoolListHeadR = block->next;
        sPoolListHeadR->prev = NULL;
        sPoolFreeSpace += (uintptr_t) sPoolListHeadR - (uintptr_t) oldListHead;
    }
    return sPoolFreeSpace;
}
// main_pool_realloc
UNUSED void* main_pool_realloc(void* addr, uintptr_t size) {
    void* newAddr = NULL;
    struct MainPoolBlock* block = (struct MainPoolBlock*) ((u8*) addr - 8);

    if (block->next == sPoolListHeadL) {
        main_pool_free(addr);
        newAddr = main_pool_alloc(size, MEMORY_POOL_LEFT);
    }
    return newAddr;
}

UNUSED uintptr_t main_pool_available(void) {
    return sPoolFreeSpace - 8;
}

UNUSED uintptr_t main_pool_push_state(void) {
    struct MainPoolState* prevState = gMainPoolState;
    uintptr_t freeSpace = sPoolFreeSpace;
    struct MainPoolBlock* lhead = sPoolListHeadL;
    struct MainPoolBlock* rhead = sPoolListHeadR;

    gMainPoolState = main_pool_alloc(sizeof(*gMainPoolState), MEMORY_POOL_LEFT);
    gMainPoolState->freeSpace = freeSpace;
    gMainPoolState->listHeadL = lhead;
    gMainPoolState->listHeadR = rhead;
    gMainPoolState->prev = prevState;
    return sPoolFreeSpace;
}

/**
 * Restore pool state from a previous call to main_pool_push_state. Return the
 * amount of free space left in the pool.
 */
UNUSED uintptr_t main_pool_pop_state(void) {
    sPoolFreeSpace = gMainPoolState->freeSpace;
    sPoolListHeadL = gMainPoolState->listHeadL;
    sPoolListHeadR = gMainPoolState->listHeadR;
    gMainPoolState = gMainPoolState->prev;
    return sPoolFreeSpace;
}
// similar to sm64 dma_read
UNUSED void* func_802A80B0(u8* dest, u8* srcStart, u8* srcEnd) {
    void* addr;
    uintptr_t size = srcStart - dest;
    addr = main_pool_alloc(size, (uintptr_t) srcEnd);

    if (addr != 0) {

        osInvalDCache(addr, size);
        osPiStartDma(&gDmaIoMesg, OS_MESG_PRI_NORMAL, OS_READ, (uintptr_t) dest, addr, size, &gDmaMesgQueue);
        osRecvMesg(&gDmaMesgQueue, &gMainReceivedMesg, OS_MESG_BLOCK);
    }
    return addr;
}

UNUSED struct AllocOnlyPool* alloc_only_pool_init(uintptr_t size, uintptr_t side) {
    void* addr;
    struct AllocOnlyPool* subPool = NULL;

    size = ALIGN4(size);
    addr = main_pool_alloc(size + sizeof(struct AllocOnlyPool), side);
    if (addr != NULL) {
        subPool = (struct AllocOnlyPool*) addr;
        subPool->totalSpace = size;
        subPool->usedSpace = (s32) addr + sizeof(struct AllocOnlyPool);
        subPool->startPtr = 0;
        subPool->freePtr = (u8*) addr + sizeof(struct AllocOnlyPool);
    }
    return subPool;
}

UNUSED uintptr_t func_802A82AC(s32 arg0) {
    uintptr_t temp_v0;
    uintptr_t phi_v1;

    temp_v0 = D_801502A0 - arg0;
    phi_v1 = 0;
    if (temp_v0 >= (uintptr_t) gDisplayListHead) {
        D_801502A0 = temp_v0;
        phi_v1 = temp_v0;
    }
    return phi_v1;
}

// unused mio0 decode func.
UNUSED uintptr_t func_802A8348(s32 arg0, s32 arg1, s32 arg2) {
    uintptr_t offset;
    UNUSED void* pad;
    uintptr_t oldAddr;
    void* newAddr;

    offset = ALIGN16(arg1 * arg2);
    oldAddr = gNextFreeMemoryAddress;
    newAddr = (void*) (oldAddr + offset);
    pad = &newAddr;
#ifdef TARGET_N64
    osInvalDCache(newAddr, offset);
    osPiStartDma(&gDmaIoMesg, 0, 0, (uintptr_t) &_other_texturesSegmentRomStart[SEGMENT_OFFSET(arg0)], newAddr, offset,
                 &gDmaMesgQueue);
    osRecvMesg(&gDmaMesgQueue, &gMainReceivedMesg, 1);
#endif

    func_80040030((u8*) newAddr, (u8*) oldAddr);
    gNextFreeMemoryAddress += offset;
    return oldAddr;
}

UNUSED u8* func_802A841C(u8* arg0, s32 arg1, s32 arg2) {
    u8* temp_v0;
    void* temp_a0;
    temp_v0 = (u8*) gNextFreeMemoryAddress;
    temp_a0 = temp_v0 + arg2;
    arg1 = ALIGN16(arg1);
    arg2 = ALIGN16(arg2);

    osInvalDCache(temp_a0, arg1);
#ifdef TARGET_N64
    osPiStartDma(&gDmaIoMesg, 0, 0, (uintptr_t) &_other_texturesSegmentRomStart[SEGMENT_OFFSET(arg0)], temp_a0, arg1,
                 &gDmaMesgQueue);
#endif
    osRecvMesg(&gDmaMesgQueue, &gMainReceivedMesg, 1);
    func_80040030((u8*) temp_a0, temp_v0);
    gNextFreeMemoryAddress += arg2;
    return temp_v0;
}

uintptr_t MIO0_0F(u8* arg0, uintptr_t arg1, uintptr_t arg2) {
    uintptr_t oldHeapEndPtr;
    void* temp_v0;

    arg1 = ALIGN16(arg1);
    arg2 = ALIGN16(arg2);
    oldHeapEndPtr = gHeapEndPtr;
    temp_v0 = (void*) gNextFreeMemoryAddress;

    osInvalDCache(temp_v0, arg1);
#ifdef TARGET_N64
    osPiStartDma(&gDmaIoMesg, 0, 0, (uintptr_t) &_other_texturesSegmentRomStart[SEGMENT_OFFSET(arg0)], temp_v0, arg1,
                 &gDmaMesgQueue);
#endif
    osRecvMesg(&gDmaMesgQueue, &gMainReceivedMesg, 1);
    mio0decode((u8*) temp_v0, (u8*) oldHeapEndPtr);
    gHeapEndPtr += arg2;
    return oldHeapEndPtr;
}

struct UnkStr_802AA7C8 {
    u8* unk0;
    uintptr_t unk4;
    uintptr_t unk8;
    uintptr_t unkC;
};

void* decompress_segments(u8* start, u8* end) {
    return NULL;
    UNUSED u32 pad;
    u32 sp28;
    u32 size = ALIGN16(end - start);
    u8* heapEnd;
    u32* freeSpace;

    heapEnd = (u8*) gHeapEndPtr - size;
    // sp20 = temp_a0;
    dma_copy(heapEnd, start, size);
    sp28 = *(u32*) (heapEnd + 4);
    sp28 = ALIGN16(sp28);
    freeSpace = (u32*) gNextFreeMemoryAddress;
    mio0decode(heapEnd, (u8*) freeSpace);
    gNextFreeMemoryAddress += sp28;
    return (void*) freeSpace;
}

extern const course_texture mario_raceway_textures[30];

/* To help verify if ptrs are pointing within segments see gfx_pc.cpp gfx_step() */
uintptr_t vtxSegEnd;
uintptr_t dlSegEnd;
uintptr_t texSegEnd;
size_t texSegSize;
Gfx* testaaa;

u8* load_lakitu_tlut_x64(const char** textureList, size_t length) {
    // Calculate lakitu texture size to allocate
    size_t size = 0;
    for (size_t i = 0; i < length; i++) {
        size += ResourceGetTexSizeByName(textureList[i]);
    }

    u8* textures = (u8*) gNextFreeMemoryAddress;
    gNextFreeMemoryAddress += size;
    size_t offset = 0;
    for (size_t i = 0; i < length; i++) {
        u8* tex = (u8*) LOAD_ASSET_RAW(textureList[i]);
        size_t texSize = ResourceGetTexSizeByName(textureList[i]);
        // printf("\nTEX SIZE: %X\n\n", texSize);
        memcpy(&textures[offset], tex, texSize);
        offset += texSize;
    }
    return textures;
}

/**
 * @brief Loads & DMAs course data. Vtx, textures, displaylists, etc.
 * @param courseId
 */
void load_course(s32 courseId) {
    printf("Loading Course %d\n", courseId);
    gNextFreeMemoryAddress = gFreeMemoryResetAnchor;
    CM_CleanWorld();
    LoadCourse();
    CM_Editor_SetLevelDimensions(gCourseMinX, gCourseMaxX, gCourseMinZ, gCourseMaxZ, gCourseMinY, gCourseMaxY);
}
