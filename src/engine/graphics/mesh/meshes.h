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

    std::shared_ptr<Mesh> getQuad() const { return _quad; }
    std::shared_ptr<Mesh> getQuadFlipX() const { return _quadFlipX; }
    std::shared_ptr<Mesh> getQuadFlipY() const { return _quadFlipY; }
    std::shared_ptr<Mesh> getQuadFlipXY() const { return _quadFlipXY; }
    std::shared_ptr<Mesh> getQuadSwap() const { return _quadSwap; }
    std::shared_ptr<Mesh> getQuadSwapFlipX() const { return _quadSwapFlipX; }
    std::shared_ptr<Mesh> getQuadNDC() const { return _quadNDC; }
    std::shared_ptr<Mesh> getQuadNDCFlipY() const { return _quadNDCFlipY; }
    std::shared_ptr<Mesh> getBillboard() const { return _billboard; }
    std::shared_ptr<Mesh> getGrass() const { return _grass; }
    std::shared_ptr<Mesh> getCube() const { return _cube; }
    std::shared_ptr<Mesh> getSphere() const { return _sphere; }
    std::shared_ptr<Mesh> getCubemap() const { return _cubemap; }
    std::shared_ptr<Mesh> getAABB() const { return _aabb; }

private:
    bool _inited { false };

    std::shared_ptr<Mesh> _quad;
    std::shared_ptr<Mesh> _quadFlipX;
    std::shared_ptr<Mesh> _quadFlipY;
    std::shared_ptr<Mesh> _quadFlipXY;
    std::shared_ptr<Mesh> _quadSwap;
    std::shared_ptr<Mesh> _quadSwapFlipX;
    std::shared_ptr<Mesh> _quadNDC;
    std::shared_ptr<Mesh> _quadNDCFlipY;
    std::shared_ptr<Mesh> _billboard;
    std::shared_ptr<Mesh> _grass;
    std::shared_ptr<Mesh> _cube;
    std::shared_ptr<Mesh> _sphere;
    std::shared_ptr<Mesh> _cubemap;
    std::shared_ptr<Mesh> _aabb;
};

} // namespace graphics

} // namespace reone
