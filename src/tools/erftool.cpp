#include "tools.h"

#include <boost/format.hpp>

#include "../core/log.h"
#include "../resources/erffile.h"
#include "../resources/util.h"

using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void ErfTool::list(const fs::path &path, const fs::path &keyPath) const {
    ErfFile erf;
    erf.load(path);

    for (auto &key : erf.keys()) {
        info(boost::format("%16s\t%4s") % key.resRef % getExtByResType(key.resType));
    }
}

void ErfTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    ErfFile erf;
    erf.load(path);

    auto &keys = erf.keys();
    for (int i = 0; i < keys.size(); ++i) {
        const ErfFile::Key &key = keys[i];
        info(boost::format("Extracting %16s\t%4s") % key.resRef % getExtByResType(key.resType));

        fs::path resPath(destPath);
        resPath.append(key.resRef + "." + getExtByResType(key.resType));

        ByteArray data(erf.getResourceData(i));
        fs::ofstream res(resPath, std::ios::binary);
        res.write(data.data(), data.size());
    }
}

} // namespace tools

} // namespace reone
