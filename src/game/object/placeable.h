#pragma once

#include "object.h"

namespace reone {

namespace game {

class Placeable : public Object {
public:
    Placeable(uint32_t id);

    void load(const resources::GffStruct &gffs);

private:
    void loadBlueprint(const resources::GffStruct &gffs);
};

} // namespace game

} // namespace reone
