#pragma once

#include "binfile.h"
#include "types.h"

namespace reone {

namespace resources {

class KeyFile : public BinaryFile {
public:
    struct FileEntry {
        uint32_t fileSize { 0 };
        std::string filename;
    };

    struct KeyEntry {
        std::string resRef;
        ResourceType resType { ResourceType::Invalid };
        int bifIdx { 0 };
        int resIdx { 0 };
    };

    KeyFile();

    const std::string &getFilename(int idx) const;
    bool find(const std::string &resRef, ResourceType type, KeyEntry &key) const;

    const std::vector<FileEntry> &files() const;
    const std::vector<KeyEntry> &keys() const;

private:
    int _bifCount { 0 };
    int _keyCount { 0 };
    uint32_t _filesOffset { 0 };
    uint32_t _keysOffset { 0 };
    std::vector<FileEntry> _files;
    std::vector<KeyEntry> _keys;

    void doLoad() override;
    void loadFiles();
    FileEntry readFileEntry();
    void loadKeys();
    KeyEntry readKeyEntry();
};

} // namespace resources

} // namespace reone
