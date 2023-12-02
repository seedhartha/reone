/*
 * Copyright (c) 2020-2023 The reone project contributors
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

struct MeshName {
    static constexpr char quad[] = "quad";
    static constexpr char quadNDC[] = "quad_ndc";
    static constexpr char billboard[] = "billboard";
    static constexpr char grass[] = "grass";
    static constexpr char box[] = "box";
    static constexpr char cubemap[] = "cubemap";
};

class IMeshRegistry {
public:
    virtual ~IMeshRegistry() = default;

    virtual Mesh &get(const std::string &name) = 0;
};

class MeshRegistry : public IMeshRegistry, boost::noncopyable {
public:
    ~MeshRegistry() { deinit(); }

    void init();
    void deinit();

    void add(std::string name, std::shared_ptr<Mesh> mesh) {
        _nameToMesh[std::move(name)] = std::move(mesh);
    }

    Mesh &get(const std::string &name) override {
        auto mesh = _nameToMesh.find(name);
        if (mesh == _nameToMesh.end()) {
            throw std::runtime_error("Mesh not found by name: " + name);
        }
        return *mesh->second;
    }

private:
    bool _inited {false};

    std::map<std::string, std::shared_ptr<Mesh>> _nameToMesh;
};

} // namespace graphics

} // namespace reone
