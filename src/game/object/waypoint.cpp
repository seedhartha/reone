#include "waypoint.h"

#include <boost/algorithm/string.hpp>

using namespace reone::resources;

namespace reone {

namespace game {

Waypoint::Waypoint(uint32_t id) : Object(id) {
    _type = ObjectType::Waypoint;
}

void Waypoint::load(const GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    _position[0] = gffs.getFloat("XPosition");
    _position[1] = gffs.getFloat("YPosition");
    _position[2] = gffs.getFloat("ZPosition");

    float headingSin = gffs.getFloat("XOrientation");
    float headingCos = gffs.getFloat("YOrientation");
    _heading = -glm::atan(headingSin, headingCos);

    updateTransform();
}

} // namespace game

} // namespace reone
