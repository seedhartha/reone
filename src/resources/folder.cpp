#include "folder.h"

#include <boost/algorithm/string.hpp>

#include "util.h"

namespace fs = boost::filesystem;

namespace reone {

namespace resources {

void Folder::load(const fs::path &path) {
    if (!fs::is_directory(path)) {
        throw std::runtime_error("Folder not found: " + path.string());
    }
    _path = path;

    for (auto &entry : fs::directory_iterator(path)) {
        const fs::path &path2 = entry.path();
        if (fs::is_directory(path2)) continue;

        std::string resRef(path2.filename().replace_extension("").string());
        boost::to_lower(resRef);

        std::string ext(path2.extension().string().substr(1));

        Resource res;
        res.path = path2;
        res.type = getResTypeByExt(ext);

        _resources.insert(std::make_pair(resRef, res));
    }
}

bool Folder::supports(ResourceType type) const {
    return true;
}

std::shared_ptr<ByteArray> Folder::find(const std::string &resRef, ResourceType type) {
    fs::path path;
    for (auto &res : _resources) {
        if (res.first == resRef && res.second.type == type) {
            path = res.second.path;
            break;
        }
    }
    if (path.empty()) {
        return std::shared_ptr<ByteArray>();
    }
    fs::ifstream in(path, std::ios::binary);

    in.seekg(0, std::ios::end);
    size_t size = in.tellg();

    in.seekg(std::ios::beg);
    ByteArray data(size);
    in.read(&data[0], size);

    return std::make_shared<ByteArray>(std::move(data));
}

} // namespace resources

} // namespace reone
