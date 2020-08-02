#pragma once

#include "SDL2/SDL_events.h"

#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

namespace reone {

namespace render {

class Camera {
public:
    virtual bool handle(const SDL_Event &event) = 0;
    virtual void update(float dt) = 0;

    const glm::mat4 &projection() const;
    const glm::mat4 &view() const;
    const glm::vec3 &position() const;
    float heading() const;

protected:
    glm::mat4 _projection { 1.0f };
    glm::mat4 _view { 1.0f };
    glm::vec3 _position { 0.0f };
    float _heading { 0.0f };
};

} // namespace render

} // namespace reone
