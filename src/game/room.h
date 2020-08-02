#pragma once

#include <memory>

#include "glm/vec3.hpp"

#include "../render/modelinstance.h"
#include "../render/walkmesh.h"

namespace reone {

namespace game {

class Room {
public:
    Room(
        const std::string &name,
        const glm::vec3 &position,
        const std::shared_ptr<render::ModelInstance> &model,
        const std::shared_ptr<render::Walkmesh> &walkmesh);

    const glm::vec3 &position() const;
    std::shared_ptr<render::ModelInstance> model() const;
    std::shared_ptr<render::Walkmesh> walkmesh() const;

private:
    std::string _name;
    glm::vec3 _position { 0.0f };
    std::shared_ptr<render::ModelInstance> _model;
    std::shared_ptr<render::Walkmesh> _walkmesh;
};

} // namespace game

} // namespace reone
