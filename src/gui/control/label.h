#pragma once

#include "control.h"

namespace reone {

namespace gui {

class Label : public Control {
public:
    Label();
    Label(const std::string &tag);
};

} // namespace gui

} // namespace reone
