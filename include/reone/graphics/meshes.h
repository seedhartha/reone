/*
 * Copyright (c) 2020-2022 The reone project contributors
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

class IMeshes {
public:
    virtual ~IMeshes() = default;
};

class Meshes : public IMeshes, boost::noncopyable {
public:
    ~Meshes() { deinit(); }

    void init();
    void deinit();

    Mesh &quad() const { return *_quad; }
    Mesh &quadNDC() const { return *_quadNDC; }
    Mesh &billboard() const { return *_billboard; }
    Mesh &grass() const { return *_grass; }

    Mesh &box() const { return *_box; }
    Mesh &cubemap() const { return *_cubemap; }

private:
    bool _inited {false};

    std::unique_ptr<Mesh> _quad;
    std::unique_ptr<Mesh> _quadNDC;
    std::unique_ptr<Mesh> _billboard;
    std::unique_ptr<Mesh> _grass;

    std::unique_ptr<Mesh> _box;
    std::unique_ptr<Mesh> _cubemap;
};

} // namespace graphics

} // namespace reone
