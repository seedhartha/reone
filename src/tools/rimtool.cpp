#include "tools.h"

#include <boost/format.hpp>

#include "../core/log.h"
#include "../resources/rimfile.h"
#include "../resources/util.h"

using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void RimTool::list(const fs::path &path, const fs::path &keyPath) const {
    RimFile rim;
    rim.load(path);

    for (auto &res : rim.resources()) {
        info(boost::format("%16s\t%4s") % res.resRef % getExtByResType(res.type));
    }
}

void RimTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    RimFile rim;
    rim.load(path);

    auto &resources = rim.resources();
    for (int i = 0; i < resources.size(); ++i) {
        const RimFile::Resource &resource = resources[i];
        info(boost::format("Extracting %16s\t%4s") % resource.resRef % getExtByResType(resource.type));

        fs::path resPath(destPath);
        resPath.append(resource.resRef + "." + getExtByResType(resource.type));

        ByteArray data(rim.getResourceData(i));
        fs::ofstream res(resPath, std::ios::binary);
        res.write(data.data(), data.size());
    }
}

} // namespace tools

} // namespace reone
