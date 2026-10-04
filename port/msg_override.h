#pragma once

/*
 * Message override: lets PaperBoat-format language mods (.o2r files that carry
 * "messages/MSG_*" blobs, e.g. the Spanish translation) replace the text this
 * port would otherwise read from the ROM.
 *
 * Install location on Vita: ux0:data/papership/mods/<mod>.o2r
 */

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Size of the LUS binary resource header + the uint32 blob size field. */
#define PORT_MSG_BLOB_HEADER_SIZE 0x44

/* Return codes of port_msg_decode_blob(). */
#define PORT_MSG_DECODE_OK 1
#define PORT_MSG_DECODE_TRUNCATED 2 /* decoded, but did not fit and was cut */
#define PORT_MSG_DECODE_BAD 0       /* malformed; dest untouched */

/*
 * "Intro_0001", "MAC_Gate_0003", ... for message ID ((section << 16) | index),
 * or NULL if the ID is outside the known table.  The PaperBoat resource is
 * "messages/MSG_" + this suffix.  Generated: see msg_resource_names.c.
 */
const char* port_msg_resource_suffix(unsigned int msgID);

/*
 * Decode one PaperBoat message blob file (header + uint32 size + bytes) into
 * dest.  dest receives the raw message bytes, always terminated with 0xFD
 * (MSG_CHAR_READ_END) inside destCap bytes.  If outLen is not NULL it receives
 * the number of bytes written to dest (terminator included).  Pure function,
 * no I/O.
 */
int port_msg_decode_blob(const uint8_t* file, size_t fileSize, uint8_t* dest, size_t destCap, size_t* outLen);

/*
 * Try to satisfy dma_load_msg() from a loaded mod.  Returns 1 and fills dest
 * (destCap bytes available) if a mod supplies this message; 0 means "not
 * overridden, fall back to the ROM".
 */
int Port_MsgLoadOverride(unsigned int msgID, void* dest, unsigned int destCap);

#ifdef __cplusplus
}
#endif
