#include "camera.h"

namespace reone {

namespace render {

const glm::mat4 &Camera::projection() const {
    return _projection;
}

const glm::mat4 &Camera::view() const {
    return _view;
}

const glm::vec3 &Camera::position() const {
    return _position;
}

float Camera::heading() const {
    return _heading;
}

} // namespace render

} // namespace reone
