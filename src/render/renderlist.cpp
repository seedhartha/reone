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

#include "renderlist.h"

#include <algorithm>

#include "glm/gtx/norm.hpp"

#include "modelinstance.h"

using namespace std;

namespace reone {

namespace render {

void RenderList::sortByDistanceToCamera(const glm::vec3 &cameraPosition) {
    sort(begin(), end(), [&cameraPosition](const RenderListItem &left, const RenderListItem &right) {
        return glm::distance2(left.origin, cameraPosition) > glm::distance2(right.origin, cameraPosition);
    });
}

void RenderList::render(bool debug) const {
    for (auto &item : *this) {
        item.model->render(*item.node, item.transform, debug);
    }
}

} // namespace render

} // namespace reone
