/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include <atomic>
#include <map>
#include <memory>
#include <set>
#include <queue>

#include "../render/walkmesh.h"

namespace reone {

namespace game {

class NavMesh {
public:
    NavMesh() = default;

    void add(const std::shared_ptr<render::Walkmesh> &walkmesh, const glm::mat4 &transform);
    void compute(const std::atomic_bool &cancel);

    const std::vector<glm::vec3> findPath(const glm::vec3 &from, const glm::vec3 &to) const;

private:
    struct WalkmeshWrapper {
        std::shared_ptr<render::Walkmesh> walkmesh;
        glm::mat4 transform;

        WalkmeshWrapper(const std::shared_ptr<render::Walkmesh> &walkmesh, const glm::mat4 &transform);
    };

    struct Edge {
        uint16_t toIndex { 0 };
        float length { 0 };

        Edge(uint16_t toIndex, float length);
    };

    struct FindPathContext {
        std::map<uint16_t, std::pair<uint16_t, float>> distToOrigin;
        std::set<uint16_t> visited;
        std::queue<uint16_t> queue;
    };

    std::vector<WalkmeshWrapper> _walkmeshes;
    std::vector<glm::vec3> _vertices;
    std::map<uint16_t, std::vector<Edge>> _edges;
    std::atomic_bool _computed { false };

    NavMesh(const NavMesh &) = delete;
    NavMesh &operator=(const NavMesh &) = delete;

    uint16_t getNearestVertex(const glm::vec3 &point) const;
    void visit(uint16_t index, FindPathContext &ctx) const;
};

} // namespace game

} // namespace reone
