/*
 * Decoder for PaperBoat message blobs.
 *
 * Layout of "messages/MSG_*" inside an .o2r (a standard LUS binary resource of
 * type Blob):
 *
 *   0x00      u8   byte order of the multi-byte fields below (0 = little, 1 = big)
 *   0x01..3F       rest of the 64-byte LUS header (type, version, id, ...) -- not needed
 *   0x40      u32  number of payload bytes
 *   0x44..    u8[] the message bytes, exactly as stored in the ROM's message
 *                  data (same control codes, terminated by 0xFD)
 *
 * This file has no dependency on libultraship so it can be unit-tested on a PC.
 */

#include "msg_override.h"

#include <string.h>

#define MSG_CHAR_READ_END 0xFD

static uint32_t rd32(const uint8_t* p, int bigEndian) {
    if (bigEndian) {
        return ((uint32_t) p[0] << 24) | ((uint32_t) p[1] << 16) | ((uint32_t) p[2] << 8) | (uint32_t) p[3];
    }
    return ((uint32_t) p[3] << 24) | ((uint32_t) p[2] << 16) | ((uint32_t) p[1] << 8) | (uint32_t) p[0];
}

int port_msg_decode_blob(const uint8_t* file, size_t fileSize, uint8_t* dest, size_t destCap, size_t* outLen) {
    uint32_t dataSize;
    size_t avail;
    size_t maxBytes;

    if (file == NULL || dest == NULL || destCap < 2) {
        return PORT_MSG_DECODE_BAD;
    }
    if (fileSize < PORT_MSG_BLOB_HEADER_SIZE) {
        return PORT_MSG_DECODE_BAD;
    }
    if (file[0] != 0 && file[0] != 1) {
        return PORT_MSG_DECODE_BAD;
    }

    dataSize = rd32(file + 0x40, file[0] == 1);
    avail = fileSize - PORT_MSG_BLOB_HEADER_SIZE;
    if (dataSize == 0 || (size_t) dataSize > avail) {
        return PORT_MSG_DECODE_BAD;
    }

    /* Keep one byte of slack, same limit the ROM path in dma_load_msg() uses (0x3FF of 0x400). */
    maxBytes = destCap - 1;

    if ((size_t) dataSize > maxBytes) {
        memcpy(dest, file + PORT_MSG_BLOB_HEADER_SIZE, maxBytes);
        dest[maxBytes - 1] = MSG_CHAR_READ_END;
        if (outLen != NULL) {
            *outLen = maxBytes;
        }
        return PORT_MSG_DECODE_TRUNCATED;
    }

    memcpy(dest, file + PORT_MSG_BLOB_HEADER_SIZE, dataSize);
    if (dest[dataSize - 1] != MSG_CHAR_READ_END) {
        dest[dataSize] = MSG_CHAR_READ_END; /* dataSize <= destCap - 1, always in bounds */
        if (outLen != NULL) {
            *outLen = (size_t) dataSize + 1;
        }
    } else if (outLen != NULL) {
        *outLen = dataSize;
    }
    return PORT_MSG_DECODE_OK;
}
