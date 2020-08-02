#pragma once

#include <istream>
#include <memory>

#include "../core/types.h"

namespace reone {

std::unique_ptr<std::istream> wrap(const ByteArray &arr);

inline std::unique_ptr<std::istream> wrap(const std::shared_ptr<ByteArray> &arr) {
    return wrap(*arr.get());
}

} // namespace reone
