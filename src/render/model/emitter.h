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

#include <memory>

#include "glm/vec2.hpp"

#include "../texture.h"

namespace reone {

namespace render {

class Emitter {
public:
    enum class UpdateMode {
        Invalid,
        Fountain,
        Single,
        Explosion
    };

    enum class RenderMode {
        Invalid = 0,
        Normal = 1,
        BillboardToWorldZ = 2,
        MotionBlur = 3,
        BillboardToLocalZ = 4,
        AlignedToParticleDir = 5
    };

    enum class BlendMode {
        Invalid,
        Normal,
        Punch,
        Lighten
    };

    template <class T>
    struct Constraints {
        T start;
        T mid;
        T end;
    };

    UpdateMode updateMode() const { return _updateMode; }
    RenderMode renderMode() const { return _renderMode; }
    BlendMode blendMode() const { return _blendMode; }
    int renderOrder() const { return _renderOrder; }

    std::shared_ptr<Texture> texture() const { return _texture; }
    int gridWidth() const { return _gridWidth; }
    int gridHeight() const { return _gridHeight; }
    int frameStart() const { return _frameStart; }
    int frameEnd() const { return _frameEnd; }

    glm::vec2 &size() { return _size; }
    const glm::vec2 &size() const { return _size; }
    Constraints<float> &particleSize() { return _particleSize; }
    const Constraints<float> &particleSize() const { return _particleSize; }
    Constraints<glm::vec3> &color() { return _color; }
    const Constraints<glm::vec3> &color() const { return _color; }
    Constraints<float> &alpha() { return _alpha; }
    const Constraints<float> &alpha() const { return _alpha; }
    int birthrate() const { return _birthrate; }
    int lifeExpectancy() const { return _lifeExpectancy; }
    float velocity() const { return _velocity; }
    float randomVelocity() const { return _randomVelocity; }
    float spread() const { return _spread; }
    bool loop() const { return _loop; }
    int fps() const { return _fps; }

    void setFrameStart(int value) { _frameStart = value; }
    void setFrameEnd(int value) { _frameEnd = value; }
    void setBirthrate(int value) { _birthrate = value; }
    void setLifeExpectancy(int value) { _lifeExpectancy = value; }
    void setVelocity(float value) { _velocity = value; }
    void setRandomVelocity(float value) { _randomVelocity = value; }
    void setSpread(float value) { _spread = value; }
    void setLoop(bool value) { _loop = value; }
    void setFPS(int value) { _fps = value; }

private:
    UpdateMode _updateMode { UpdateMode::Invalid };
    RenderMode _renderMode { RenderMode::Invalid };
    BlendMode _blendMode { BlendMode::Invalid };
    int _renderOrder { 0 };

    std::shared_ptr<Texture> _texture;
    int _gridWidth { 0 };
    int _gridHeight { 0 };
    int _frameStart { 0 };
    int _frameEnd { 0 };

    glm::vec2 _size { 0.0f };
    int _birthrate { 0 }; /**< rate of particle birth per second */
    int _lifeExpectancy { 0 }; /**< life of each particle in seconds */
    float _velocity { 0.0f };
    float _randomVelocity { 0.0f };
    float _spread { 0.0f };
    bool _loop { false };
    int _fps { 0 };

    Constraints<float> _particleSize;
    Constraints<glm::vec3> _color;
    Constraints<float> _alpha;

    friend class MdlReader;
};

} // namespace render

} // namespace reone
