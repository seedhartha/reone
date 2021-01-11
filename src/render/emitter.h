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

#include "texture.h"

namespace reone {

namespace render {

class Emitter {
public:
    enum class UpdateType {
        Invalid,
        Fountain,
        Single,
        Explosion
    };

    enum class RenderType {
        Invalid,
        Normal,
        BillboardToWorldZ,
        MotionBlur
    };

    enum class BlendType {
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

    UpdateType updateType() const { return _updateType; }
    RenderType renderType() const { return _renderType; }
    BlendType blendType() const { return _blendType; }
    int renderOrder() const { return _renderOrder; }

    std::shared_ptr<Texture> texture() const { return _texture; }
    int gridWidth() const { return _gridWidth; }
    int gridHeight() const { return _gridHeight; }
    int frameStart() const { return _frameStart; }
    int frameEnd() const { return _frameEnd; }

    const glm::vec2 &size() const { return _size; }
    const Constraints<float> &particleSize() const { return _particleSize; }
    const Constraints<glm::vec3> &color() const { return _color; }
    const Constraints<float> &alpha() const { return _alpha; }
    int birthrate() const { return _birthrate; }
    int lifeExpectancy() const { return _lifeExpectancy; }
    float velocity() const { return _velocity; }
    float randomVelocity() const { return _randomVelocity; }

private:
    UpdateType _updateType { UpdateType::Invalid };
    RenderType _renderType { RenderType::Invalid };
    BlendType _blendType { BlendType::Invalid };
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

    Constraints<float> _particleSize;
    Constraints<glm::vec3> _color;
    Constraints<float> _alpha;

    friend class MdlFile;
};

} // namespace render

} // namespace reone
