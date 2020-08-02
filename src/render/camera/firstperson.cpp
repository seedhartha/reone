#include "firstperson.h"

#include "glm/ext.hpp"

namespace reone {

namespace render {

static const float kMovementSpeed = 5.0f;
static const float kMouseMultiplier = glm::pi<float>() / 2000.0f;

FirstPersonCamera::FirstPersonCamera(float aspect, float fovy, float zNear, float zFar) {
    _projection = glm::perspective(fovy, aspect, zNear, zFar);
}

bool FirstPersonCamera::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
            return handleMouseMotion(event.motion);
        case SDL_KEYDOWN:
            return handleKeyDown(event.key);
        case SDL_KEYUP:
            return handleKeyUp(event.key);
        default:
            return false;
    }
}

bool FirstPersonCamera::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    _heading = glm::mod(
        _heading - event.xrel * kMouseMultiplier,
        glm::two_pi<float>());

    constexpr float quarterPi = glm::quarter_pi<float>();

    _pitch = glm::clamp(
        _pitch - event.yrel * kMouseMultiplier,
        -quarterPi,
        quarterPi);

    updateView();

    return true;
}

void FirstPersonCamera::updateView() {
    glm::quat orientation(glm::vec3(glm::half_pi<float>(), 0.0f, 0.0f));
    orientation *= glm::quat(glm::vec3(_pitch, _heading, 0.0f));

    _view = glm::translate(glm::mat4(1.0f), _position) * glm::mat4_cast(orientation);
    _view = glm::inverse(_view);
}

bool FirstPersonCamera::handleKeyDown(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = true;
            return true;

        case SDL_SCANCODE_A:
            _moveLeft = true;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = true;
            return true;

        case SDL_SCANCODE_D:
            _moveRight = true;
            return true;

        default:
            return false;
    }
}

bool FirstPersonCamera::handleKeyUp(const SDL_KeyboardEvent &event) {
    switch (event.keysym.scancode) {
        case SDL_SCANCODE_W:
            _moveForward = false;
            return true;

        case SDL_SCANCODE_A:
            _moveLeft = false;
            return true;

        case SDL_SCANCODE_S:
            _moveBackward = false;
            return true;

        case SDL_SCANCODE_D:
            _moveRight = false;
            return true;

        default:
            return false;
    }
}

void FirstPersonCamera::update(float dt) {
    bool positionChanged = false;
    float sinYawAdj = glm::sin(_heading) * kMovementSpeed * dt;
    float cosYawAdj = glm::cos(_heading) * kMovementSpeed * dt;
    float sinPitchAdj = glm::sin(_pitch) * kMovementSpeed * dt;

    if (_moveForward) {
        _position.x -= sinYawAdj;
        _position.y += cosYawAdj;
        _position.z += sinPitchAdj;
        positionChanged = true;
    }
    if (_moveLeft) {
        _position.x -= cosYawAdj;
        _position.y -= sinYawAdj;
        positionChanged = true;
    }
    if (_moveBackward) {
        _position.x += sinYawAdj;
        _position.y -= cosYawAdj;
        _position.z -= sinPitchAdj;
        positionChanged = true;
    }
    if (_moveRight) {
        _position.x += cosYawAdj;
        _position.y += sinYawAdj;
        positionChanged = true;
    }

    if (positionChanged) updateView();
}

void FirstPersonCamera::setPosition(const glm::vec3 &pos) {
    _position = pos;
    updateView();
}

void FirstPersonCamera::setHeading(float heading) {
    _heading = heading;
    updateView();
}

} // namespace render

} // namespace reone
