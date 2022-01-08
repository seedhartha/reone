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

#include "progressbar.h"

#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/renderbuffer.h"
#include "../../graphics/shaders.h"
#include "../../graphics/texture.h"
#include "../../graphics/textures.h"
#include "../../graphics/window.h"
#include "../../resource/gffstruct.h"

#include "../gui.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace gui {

void ProgressBar::load(const GffStruct &gffs) {
    Control::load(gffs);

    shared_ptr<GffStruct> dir(gffs.getStruct("PROGRESS"));
    if (dir) {
        string fill(dir->getString("FILL"));
        _progress.fill = _textures.get(fill, TextureUsage::GUI);
    }
}

void ProgressBar::draw(const glm::ivec2 &screenSize, const glm::ivec2 &offset, const vector<string> &text) {
    if (_value == 0 || !_progress.fill) {
        return;
    }
    _textures.bind(*_progress.fill);

    float w = _extent.width * _value / 100.0f;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_extent.left + offset.x, _extent.top + offset.y, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, _extent.height, 1.0f));

    auto &uniforms = _shaders.uniforms();
    uniforms.general.resetLocals();
    uniforms.general.projection = _window.getOrthoProjection();
    uniforms.general.model = move(transform);

    _shaders.use(_shaders.gui(), true);
    _meshes.quad().draw();
}

void ProgressBar::setValue(int value) {
    if (value < 0 || value > 100) {
        throw out_of_range("value out of range: " + to_string(value));
    }
    _value = value;
}

} // namespace gui

} // namespace reone
