#include "tools.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "../core/log.h"
#include "../resources/biffile.h"
#include "../resources/util.h"

using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void BifTool::list(const fs::path &path, const fs::path &keyPath) const {
    if (!fs::exists(keyPath)) {
        throw std::runtime_error("BIF: key file does not exist: " + keyPath.string());
    }
    KeyFile key;
    key.load(keyPath);

    auto &files = key.files();
    std::string filename(path.filename().string());
    int bifIdx = getFileIndexByFilename(files, filename);
    if (bifIdx == -1) {
        throw std::runtime_error("BIF: filename not found in Key file: " + filename);
    }

    for (auto &key : key.keys()) {
        if (key.bifIdx == bifIdx) {
            info(boost::format("%16s\t%4s") % key.resRef % getExtByResType(key.resType));
        }
    }
}

int BifTool::getFileIndexByFilename(const std::vector<KeyFile::FileEntry> &files, const std::string &filename) const {
    for (int i = 0; i < files.size(); ++i) {
        if (boost::contains(files[i].filename, filename)) {
            return i;
        }
    }
    return -1;
}

void BifTool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    if (!fs::exists(keyPath)) {
        throw std::runtime_error("BIF: key file does not exist: " + keyPath.string());
    }
    KeyFile key;
    key.load(keyPath);

    auto &files = key.files();
    std::string filename(path.filename().string());
    int bifIdx = getFileIndexByFilename(files, filename);
    if (bifIdx == -1) {
        throw std::runtime_error("BIF: filename not found in Key file: " + filename);
    }
    BifFile bif;
    bif.load(path);

    for (auto &key : key.keys()) {
        if (key.bifIdx != bifIdx) continue;
        info(boost::format("Extracting %16s\t%4s") % key.resRef % getExtByResType(key.resType));

        fs::path resPath(destPath);
        resPath.append(key.resRef + "." + getExtByResType(key.resType));

        ByteArray data(bif.getResourceData(key.resIdx));
        fs::ofstream res(resPath, std::ios::binary);
        res.write(data.data(), data.size());
    }
}

} // namespace tools

} // namespace reone
