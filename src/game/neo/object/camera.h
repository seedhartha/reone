/*
 * Copyright (c) 2020-2021 The reone project contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "../../camerastyle.h"

#include "../object.h"

namespace reone {

namespace game {

struct ServicesView;

namespace neo {

class Camera : public Object {
public:
    enum class Mode {
        ThirdPerson,
        Flycam
    };

    class Builder : public Object::Builder<Camera, Builder> {
    public:
        Builder &style(CameraStyle style) {
            _style = std::move(style);
            return *this;
        }

        Builder &aspect(float aspect) {
            _aspect = aspect;
            return *this;
        }

        std::unique_ptr<Camera> build() override {
            return std::make_unique<Camera>(_id, _tag, _sceneNode, _style, _aspect);
        }

    private:
        CameraStyle _style;
        float _aspect;
    };

    class Loader : boost::noncopyable {
    public:
        Loader(IObjectIdSequence &idSeq, ServicesView &services) :
            _idSeq(idSeq),
            _services(services) {
        }

        std::unique_ptr<Camera> load(int style);

    private:
        IObjectIdSequence &_idSeq;
        ServicesView &_services;
    };

    Camera(
        uint32_t id,
        std::string tag,
        std::shared_ptr<scene::SceneNode> sceneNode,
        CameraStyle style,
        float aspect) :
        Object(
            id,
            ObjectType::Camera,
            std::move(tag),
            std::move(sceneNode)),
        _style(std::move(style)),
        _aspect(aspect) {

        flushProjection();
    }

    bool handle(const SDL_Event &e);
    void update(float delta);

    void setMode(Camera::Mode mode) {
        _mode = mode;
    }

private:
    CameraStyle _style;
    float _aspect;

    Mode _mode {Mode::Flycam};
    glm::vec3 _position {0.0f};

    // Orientation
    float _yaw {0.0f};
    float _pitch {glm::half_pi<float>()};

    // Movement
    float _forward {0.0f};
    float _left {0.0f};
    float _backward {0.0f};
    float _right {0.0f};

    bool handleThirdPerson(const SDL_Event &e);
    bool handleFlycam(const SDL_Event &e);

    void updateThirdPerson(float delta);
    void updateFlycam(float delta);

    void flushProjection();
    void flushTransform();
};

} // namespace neo

} // namespace game

} // namespace reone
