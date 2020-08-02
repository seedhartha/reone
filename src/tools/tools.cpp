#include "tools.h"

using namespace std;

using namespace reone::resources;

namespace fs = boost::filesystem;

namespace reone {

namespace tools {

void Tool::list(const fs::path &path, const fs::path &keyPath) const {
    throwNotImplemented();
}

void Tool::extract(const fs::path &path, const fs::path &keyPath, const fs::path &destPath) const {
    throwNotImplemented();
}

void Tool::convert(const fs::path &path, const fs::path &destPath) const {
    throwNotImplemented();
}

void Tool::throwNotImplemented() const {
    throw logic_error("Not implemented");
}

#ifdef REONE_WITH_TOOLS

unique_ptr<Tool> getToolByPath(GameVersion version, const fs::path &path) {
    string ext(path.extension().string());
    if (ext == ".key") {
        return make_unique<KeyTool>();
    } else if (ext == ".bif") {
        return make_unique<BifTool>();
    } else if (ext == ".erf") {
        return make_unique<ErfTool>();
    } else if (ext == ".rim") {
        return make_unique<RimTool>();
    } else if (ext == ".2da") {
        return make_unique<TwoDaTool>();
    } else {
        return make_unique<GffTool>();
    }
}

#else

unique_ptr<Tool> getToolByPath(GameVersion version, const fs::path &path) {
    throw runtime_error("Tools disabled");
}

#endif // REONE_WITH_TOOLS

} // namespace tools

} // namespace reone
