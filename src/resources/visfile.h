#pragma once

#include <istream>
#include <memory>

#include "types.h"

namespace reone {

namespace resources {

class VisFile {
public:
    VisFile() = default;
    void load(const std::shared_ptr<std::istream> &in);
    const Visibility &visibility() const;

private:
    Visibility _visibility;
    std::string _roomFrom;

    VisFile(const VisFile &) = delete;
    VisFile &operator=(const VisFile &) = delete;

    void processLine(const std::string &line);
};

} // namespace resources

} // namespace reone
