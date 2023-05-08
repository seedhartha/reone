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

#include "reone/gui/control/plotter.h"

#include "reone/graphics/meshes.h"
#include "reone/graphics/services.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/uniforms.h"

using namespace reone::graphics;

namespace reone {

namespace gui {

static constexpr float kPointSize = 2.0f;

void Plotter::render() {
    if (!_enabled) {
        return;
    }
    _graphicsSvc.shaders.use(_graphicsSvc.shaders.points());
    for (auto &figure : _figures) {
        _graphicsSvc.uniforms.setGeneral([&figure](GeneralUniforms &u) {
            u.resetLocals();
            u.color = glm::vec4(figure.color, 1.0f);
        });
        _graphicsSvc.uniforms.setPoints([this, &figure](PointsUniforms &u) {
            for (size_t i = 0; i < figure.points.size(); ++i) {
                float x = glm::clamp((figure.points[i].x - _axes[0]) / (_axes[2] - _axes[0]), 0.0f, 1.0f);
                float y = glm::clamp((figure.points[i].y - _axes[1]) / (_axes[3] - _axes[1]), 0.0f, 1.0f);
                u.points[i] = glm::vec4(
                    static_cast<float>(_extent[0]) + kPointSize * x * static_cast<float>(_extent[2]),
                    static_cast<float>(_extent[1] + _extent[3]) - y * static_cast<float>(_extent[3]),
                    kPointSize,
                    kPointSize);
            }
        });
        _graphicsSvc.meshes.quad().drawInstanced(static_cast<int>(figure.points.size()));
    }
}

} // namespace gui

} // namespace reone
