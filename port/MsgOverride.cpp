/*
 * MsgOverride.cpp - lets PaperBoat language mods replace the ROM's message text.
 *
 * Background: this port reads every message straight from the ROM
 * (dma_load_msg in src/msg.c).  PaperBoat instead keeps each message as a
 * "messages/MSG_<Name>" blob inside .o2r archives, so translation mods made
 * for PaperBoat (for example the Spanish one on GameBanana) are just .o2r
 * files full of those blobs.  Engine.cpp already adds every .o2r found in
 * ux0:data/papership/mods to libultraship's archive manager, but nothing ever
 * asked those archives for message text.  This file does.
 *
 * Messages a mod does not provide fall back to the ROM, so partial
 * translations work and nothing changes when no mod is installed.
 */

#include "msg_override.h"

#include <cstdio>
#include <cstring>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ship/Context.h>
#include <ship/resource/File.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/archive/Archive.h>
#include <ship/resource/archive/ArchiveManager.h>

namespace {

// Every caller of dma_load_msg() passes a 0x400-byte buffer.
constexpr unsigned int kMsgBufferSize = 0x400;
// Decoded messages are small; keep the hot ones so per-frame menu text does not
// re-inflate a zip entry every frame.  Dropped wholesale if it ever grows past this.
constexpr size_t kCacheLimitBytes = 256 * 1024;
constexpr const char* kResourcePrefix = "messages/MSG_";

std::mutex sMutex;
bool sReported = false;
bool sLoggedFirstHit = false;
std::unordered_map<unsigned int, std::vector<uint8_t>> sCache;
size_t sCacheBytes = 0;
std::unordered_set<unsigned int> sProblemLogged;

std::shared_ptr<Ship::ArchiveManager> GetArchiveManager() {
    auto context = Ship::Context::GetInstance();
    if (context == nullptr) {
        return nullptr;
    }
    auto resourceManager = context->GetResourceManager();
    if (resourceManager == nullptr) {
        return nullptr;
    }
    return resourceManager->GetArchiveManager();
}

// One-time summary in log.txt so it is obvious whether a mod was picked up and,
// if not, what the loaded archives actually contain.
void ReportOnce(const std::shared_ptr<Ship::ArchiveManager>& archiveManager) {
    if (sReported) {
        return;
    }
    sReported = true;

    auto files = archiveManager->ListFiles();
    unsigned long messageFiles = 0;
    const size_t prefixLen = std::strlen(kResourcePrefix);
    if (files != nullptr) {
        for (const auto& name : *files) {
            if (name.compare(0, prefixLen, kResourcePrefix) == 0) {
                messageFiles++;
            }
        }
    }
    fprintf(stderr, "[MSG] mod text override: %lu \"%s*\" files found in loaded archives\n", messageFiles,
            kResourcePrefix);

    if (messageFiles == 0) {
        auto archives = archiveManager->GetArchives();
        if (archives != nullptr) {
            for (const auto& archive : *archives) {
                fprintf(stderr, "[MSG]   archive loaded: %s\n", archive->GetPath().c_str());
            }
        }
        if (files != nullptr) {
            unsigned long shown = 0;
            for (const auto& name : *files) {
                if (shown++ >= 8) {
                    break;
                }
                fprintf(stderr, "[MSG]   sample entry: %s\n", name.c_str());
            }
        }
    }
}

void LogProblemOnce(unsigned int msgID, const char* what, const std::string& path) {
    if (sProblemLogged.insert(msgID).second) {
        fprintf(stderr, "[MSG] mod message %s (id 0x%X): %s -- using ROM text\n", path.c_str(), msgID, what);
    }
}

} // namespace

extern "C" int Port_MsgLoadOverride(unsigned int msgID, void* dest, unsigned int destCap) {
    if (dest == nullptr || destCap < 2) {
        return 0;
    }

    const char* suffix = port_msg_resource_suffix(msgID);
    if (suffix == nullptr) {
        return 0;
    }

    std::lock_guard<std::mutex> lock(sMutex);

    const bool useCache = (destCap == kMsgBufferSize);
    if (useCache) {
        auto hit = sCache.find(msgID);
        if (hit != sCache.end()) {
            std::memcpy(dest, hit->second.data(), hit->second.size());
            return 1;
        }
    }

    auto archiveManager = GetArchiveManager();
    if (archiveManager == nullptr) {
        return 0;
    }
    ReportOnce(archiveManager);

    std::string path = kResourcePrefix;
    path += suffix;

    // HasFile() first: ArchiveManager::LoadFile() default-inserts a null entry for unknown paths.
    if (!archiveManager->HasFile(path)) {
        return 0;
    }
    auto file = archiveManager->LoadFile(path);
    if (file == nullptr || !file->IsLoaded || file->Buffer == nullptr) {
        LogProblemOnce(msgID, "could not be read from its archive", path);
        return 0;
    }

    uint8_t decoded[kMsgBufferSize];
    size_t decodedLen = 0;
    int rc = port_msg_decode_blob(reinterpret_cast<const uint8_t*>(file->Buffer->data()), file->Buffer->size(),
                                  decoded, destCap < kMsgBufferSize ? destCap : kMsgBufferSize, &decodedLen);
    if (rc == PORT_MSG_DECODE_BAD) {
        LogProblemOnce(msgID, "has an unexpected format", path);
        return 0;
    }
    if (rc == PORT_MSG_DECODE_TRUNCATED) {
        LogProblemOnce(msgID, "is longer than the 0x3FF-byte message buffer and was cut", path);
    }

    if (!sLoggedFirstHit) {
        sLoggedFirstHit = true;
        fprintf(stderr, "[MSG] mod text override active: first hit %s (id 0x%X, %lu bytes)\n", path.c_str(), msgID,
                (unsigned long) decodedLen);
    }

    std::memcpy(dest, decoded, decodedLen);

    if (useCache) {
        if (sCacheBytes + decodedLen > kCacheLimitBytes) {
            sCache.clear();
            sCacheBytes = 0;
        }
        sCache.emplace(msgID, std::vector<uint8_t>(decoded, decoded + decodedLen));
        sCacheBytes += decodedLen;
    }
    return 1;
}
