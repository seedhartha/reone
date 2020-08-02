#pragma once

#include "binfile.h"

namespace reone {

namespace resources {

class BifFile : public BinaryFile {
public:
    BifFile();
    ByteArray getResourceData(int idx);

private:
    struct ResourceEntry {
        uint32_t offset { 0 };
        uint32_t fileSize { 0 };
    };

    int _resourceCount { 0 };
    uint32_t _tableOffset { 0 };

    void doLoad() override;
    ResourceEntry readResourceEntry(int idx);
};

} // namespace resources

} // namespace reone
