#pragma once

#include <string>

namespace reone {

namespace net {

struct NetworkOptions {
    std::string host;
    int port { 0 };
};

} // namespace net

} // namespace reone
