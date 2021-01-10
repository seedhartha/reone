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
        Fountain
    };

    enum class RenderType {
        Invalid,
        Normal,
        BillboardToWorldZ
    };

public:
    UpdateType updateType() const { return _updateType; }
    RenderType renderType() const { return _renderType; }

    std::shared_ptr<Texture> texture() const { return _texture; }
    int gridWidth() const { return _gridWidth; }
    int gridHeight() const { return _gridHeight; }
    int firstFrame() const { return _firstFrame; }
    int lastFrame() const { return _lastFrame; }

    const glm::vec2 &size() const { return _size; }
    float sizeStart() const { return _sizeStart; }
    int birthrate() const { return _birthrate; }
    int lifeExpectancy() const { return _lifeExpectancy; }

private:
    UpdateType _updateType { UpdateType::Invalid };
    RenderType _renderType { RenderType::Invalid };

    std::shared_ptr<Texture> _texture;
    int _gridWidth { 0 };
    int _gridHeight { 0 };
    int _firstFrame { 0 };
    int _lastFrame { 0 };

    glm::vec2 _size { 0.0f };
    float _sizeStart { 0.0f };
    int _birthrate { 0 }; /**< rate of particle birth per second */
    int _lifeExpectancy { 0 }; /**< life of each particle in seconds */

    friend class MdlFile;
};

} // namespace render

} // namespace reone
