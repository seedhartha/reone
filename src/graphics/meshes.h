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

#include "mesh.h"

namespace reone {

namespace graphics {

/**
 * Registry of common reusable meshes.
 */
class Meshes : boost::noncopyable {
public:
    Meshes() = default;
    ~Meshes();

    void init();
    void deinit();

    Mesh &quad() const { return *_quad; }
    Mesh &quadFlipX() const { return *_quadFlipX; }
    Mesh &quadFlipY() const { return *_quadFlipY; }
    Mesh &quadFlipXY() const { return *_quadFlipXY; }
    Mesh &quadSwap() const { return *_quadSwap; }
    Mesh &quadSwapFlipX() const { return *_quadSwapFlipX; }
    Mesh &quadNDC() const { return *_quadNDC; }
    Mesh &quadNDCFlipY() const { return *_quadNDCFlipY; }
    Mesh &billboard() const { return *_billboard; }
    Mesh &grass() const { return *_grass; }
    Mesh &cube() const { return *_cube; }
    Mesh &sphere() const { return *_sphere; }
    Mesh &cubemap() const { return *_cubemap; }
    Mesh &aabb() const { return *_aabb; }

private:
    bool _inited {false};

    std::unique_ptr<Mesh> _quad;
    std::unique_ptr<Mesh> _quadFlipX;
    std::unique_ptr<Mesh> _quadFlipY;
    std::unique_ptr<Mesh> _quadFlipXY;
    std::unique_ptr<Mesh> _quadSwap;
    std::unique_ptr<Mesh> _quadSwapFlipX;
    std::unique_ptr<Mesh> _quadNDC;
    std::unique_ptr<Mesh> _quadNDCFlipY;
    std::unique_ptr<Mesh> _billboard;
    std::unique_ptr<Mesh> _grass;
    std::unique_ptr<Mesh> _cube;
    std::unique_ptr<Mesh> _sphere;
    std::unique_ptr<Mesh> _cubemap;
    std::unique_ptr<Mesh> _aabb;
};

} // namespace graphics

} // namespace reone
