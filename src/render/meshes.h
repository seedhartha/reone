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

#include <boost/noncopyable.hpp>

#include "mesh.h"

namespace reone {

namespace render {

/**
 * Registry of common reusable meshes.
 */
class Meshes : boost::noncopyable {
public:
    static Meshes &instance();

    ~Meshes();

    void init();
    void deinit();

    Mesh &getQuad() const { return *_quad; }
    Mesh &getQuadFlipX() const { return *_quadFlipX; }
    Mesh &getQuadFlipY() const { return *_quadFlipY; }
    Mesh &getQuadFlipXY() const { return *_quadFlipXY; }
    Mesh &getQuadNDC() const { return *_quadNDC; }
    Mesh &getBillboard() const { return *_billboard; }
    Mesh &getCube() const { return *_cube; }
    Mesh &getCubemap() const { return *_cubemap; }
    Mesh &getAABB() const { return *_aabb; }

private:
    bool _inited { false };

    std::unique_ptr<Mesh> _quad;
    std::unique_ptr<Mesh> _quadFlipX;
    std::unique_ptr<Mesh> _quadFlipY;
    std::unique_ptr<Mesh> _quadFlipXY;
    std::unique_ptr<Mesh> _quadNDC;
    std::unique_ptr<Mesh> _cube;
    std::unique_ptr<Mesh> _billboard;
    std::unique_ptr<Mesh> _cubemap;
    std::unique_ptr<Mesh> _aabb;
};

} // namespace render

} // namespace reone
