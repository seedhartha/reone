#pragma once

#include "object.h"

namespace reone {

namespace game {

class Waypoint : public Object {
public:
    Waypoint(uint32_t id);

    void load(const resources::GffStruct &gffs);
};

} // namespace game

} // namespace reone
