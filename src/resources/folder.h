#pragma once

#include <set>

#include <boost/filesystem.hpp>

#include "../core/types.h"

#include "types.h"

namespace reone {

namespace resources {

class Folder : public IResourceProvider {
public:
    Folder() = default;
    void load(const boost::filesystem::path &path);

    bool supports(ResourceType type) const override;
    std::shared_ptr<ByteArray> find(const std::string &resRef, ResourceType type) override;

private:
    struct Resource {
        boost::filesystem::path path;
        ResourceType type;
    };

    boost::filesystem::path _path;
    std::multimap<std::string, Resource> _resources;

    Folder(const Folder &) = delete;
    Folder &operator=(const Folder &) = delete;
};

} // namespace resources

} // namespace reone
