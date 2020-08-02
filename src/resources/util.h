#pragma once

#include <string>

#include "types.h"

namespace reone {

namespace resources {

const std::string &getExtByResType(ResourceType type);
ResourceType getResTypeByExt(const std::string &ext);

} // namespace resources

} // namespace reone
