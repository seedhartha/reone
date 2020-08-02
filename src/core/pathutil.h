#pragma once

#include <boost/filesystem.hpp>

namespace reone {

boost::filesystem::path getPathIgnoreCase(const boost::filesystem::path &basePath, const std::string &relPath);

} // namespace reone
