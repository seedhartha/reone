#include "trigger.h"

#include <boost/algorithm/string.hpp>

#include "../../resources/manager.h"

using namespace reone::resources;

namespace reone {

namespace game {

Trigger::Trigger(uint32_t id) : Object(id) {
    _type = ObjectType::Trigger;
}

void Trigger::load(const GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    _position.x = gffs.getFloat("XPosition");
    _position.y = gffs.getFloat("YPosition");
    _position.z = gffs.getFloat("ZPosition");

    updateTransform();

    int transDestIdx = gffs.getInt("TransitionDestin", -1);
    if (transDestIdx != -1) {
        ResourceManager &resources = ResourceManager::instance();
        _transitionDestin = resources.getString(transDestIdx).text;
    }

    _linkedToModule = gffs.getString("LinkedToModule");
    boost::to_lower(_linkedToModule);

    _linkedTo = gffs.getString("LinkedTo");
    boost::to_lower(_linkedTo);

    for (auto &child : gffs.getList("Geometry")) {
        float x = child.getFloat("PointX");
        float y = child.getFloat("PointY");
        float z = child.getFloat("PointZ");

        _geometry.push_back(_transform * glm::vec4(x, y, z, 1.0f));
    }
}

const std::string &Trigger::linkedToModule() const {
    return _linkedToModule;
}

const std::string &Trigger::linkedTo() const {
    return _linkedTo;
}

const std::vector<glm::vec3> &Trigger::geometry() const {
    return _geometry;
}

} // namespace game

} // namespace reone
