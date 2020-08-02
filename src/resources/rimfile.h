#pragma once

#include "binfile.h"
#include "types.h"

namespace reone {

namespace resources {

class RimFile : public BinaryFile, public IResourceProvider {
public:
    struct Resource {
        std::string resRef;
        ResourceType type { ResourceType::Invalid };
        uint32_t offset { 0 };
        uint32_t size { 0 };
    };

    RimFile();

    bool supports(ResourceType type) const override;
    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) override;
    ByteArray getResourceData(int idx);

    const std::vector<Resource> &resources() const;

private:
    int _resourceCount { 0 };
    uint32_t _resourcesOffset { 0 };
    std::vector<Resource> _resources;

    void doLoad() override;
    void loadResources();
    Resource readResource();
    ByteArray getResourceData(const Resource &res);
};

} // namespace resources

} // namespace reone
