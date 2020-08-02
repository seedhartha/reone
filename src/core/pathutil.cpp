#include "pathutil.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

namespace fs = boost::filesystem;

namespace reone {

fs::path getPathIgnoreCase(const fs::path &basePath, const std::string &relPath) {
    std::vector<std::string> tokens;
    boost::split(tokens, relPath, boost::is_any_of("/"), boost::token_compress_on);

    for (auto &entry : fs::directory_iterator(basePath)) {
        std::string filename(entry.path().filename().string());
        boost::to_lower(filename);

        if (filename == tokens[0]) {
            if (tokens.size() == 1) {
                return entry.path();
            }
            std::string relPath2(relPath.substr(tokens[0].length() + 1));

            return getPathIgnoreCase(entry.path(), relPath2);
        }
    }

    throw std::runtime_error(str(boost::format("Path not found: %s %s") % basePath % relPath));
}

} // namespace reone
