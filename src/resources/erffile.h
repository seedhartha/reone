#pragma once

#include "binfile.h"
#include "types.h"

namespace reone {

namespace resources {

class ErfFile : public BinaryFile, public IResourceProvider {
public:
    struct Key {
        std::string resRef { 0 };
        uint32_t resId { 0 };
        ResourceType resType { ResourceType::Invalid };
    };

    struct Resource {
        uint32_t offset { 0 };
        uint32_t size { 0 };
    };

    ErfFile();

    bool supports(ResourceType type) const override;
    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) override;
    ByteArray getResourceData(int idx);

    int entryCount() const;
    const std::vector<Key> &keys() const;

private:
    int _entryCount { 0 };
    uint32_t _keysOffset { 0 };
    uint32_t _resourcesOffset { 0 };
    std::vector<Key> _keys;
    std::vector<Resource> _resources;

    void doLoad() override;
    void loadKeys();
    Key readKey();
    void loadResources();
    Resource readResource();
    ByteArray getResourceData(const Resource &res);
};

} // namespace resources

} // namespace reone
