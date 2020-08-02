#include "tools.h"

#include <boost/format.hpp>

#include "../core/log.h"

using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void KeyTool::list(const fs::path &path, const fs::path &keyPath) const {
    KeyFile key;
    key.load(path);

    for (auto &file : key.files()) {
        info(file.filename);
    }
}

} // namespace tools

} // namespace reone
