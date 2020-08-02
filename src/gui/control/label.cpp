#include "label.h"

namespace reone {

namespace gui {

Label::Label() : Control(ControlType::Label) {
}

Label::Label(const std::string &tag) : Control(ControlType::Label, tag) {
}

} // namespace gui

} // namespace reone
