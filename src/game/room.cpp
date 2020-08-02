#include "room.h"

using namespace reone::render;

namespace reone {

namespace game {

Room::Room(
    const std::string &name,
    const glm::vec3 &position,
    const std::shared_ptr<render::ModelInstance> &model,
    const std::shared_ptr<render::Walkmesh> &walkmesh
) :
    _name(name), _position(position), _model(model), _walkmesh(walkmesh) {
}

const glm::vec3 &Room::position() const {
    return _position;
}

std::shared_ptr<ModelInstance> Room::model() const {
    return _model;
}

std::shared_ptr<Walkmesh> Room::walkmesh() const {
    return _walkmesh;
}

} // namespace game

} // namespace reone
