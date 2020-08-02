#pragma once

#include "binfile.h"

namespace reone {

namespace resources {

struct TalkTableString {
    std::string text;
    std::string soundResRef;
};

class TlkFile;

struct TalkTable {
public:
    TalkTable() = default;
    const TalkTableString &getString(int32_t ref) const;

private:
    std::vector<TalkTableString> _strings;

    TalkTable(const TalkTable &) = delete;
    TalkTable &operator=(const TalkTable &) = delete;

    friend class TlkFile;
};

class TlkFile : public BinaryFile {
public:
    TlkFile();
    std::shared_ptr<TalkTable> table() const;

private:
    uint32_t _stringCount { 0 };
    uint32_t _stringsOffset { 0 };
    std::shared_ptr<TalkTable> _table;

    void doLoad() override;
    void loadStrings();
};

} // namespace resources

} // namespace reone
