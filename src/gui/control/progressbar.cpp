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

#include "../../render/meshes.h"
#include "../../render/stateutil.h"
#include "../../render/textures.h"
#include "../../render/window.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace gui {

ProgressBar::ProgressBar(GUI *gui) : Control(gui, ControlType::ScrollBar) {
}

void ProgressBar::load(const GffStruct &gffs) {
    Control::load(gffs);

    shared_ptr<GffStruct> dir(gffs.getStruct("PROGRESS"));
    if (dir) {
        string fill(dir->getString("FILL"));
        _progress.fill = Textures::instance().get(fill, TextureUsage::GUI);
    }
}

void ProgressBar::draw(const glm::ivec2 &offset, const vector<string> &text) {
    if (_value == 0 || !_progress.fill) return;

    setActiveTextureUnit(TextureUnits::diffuse);
    _progress.fill->bind();

    float w = _extent.width * _value / 100.0f;

    ShaderUniforms uniforms;
    uniforms.general.projection = RenderWindow::instance().getOrthoProjection();
    uniforms.general.model = glm::translate(glm::mat4(1.0f), glm::vec3(_extent.left + offset.x, _extent.top + offset.y, 0.0f));
    uniforms.general.model = glm::scale(uniforms.general.model, glm::vec3(w, _extent.height, 1.0f));
    Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
    Meshes::instance().getQuad()->draw();
}

void ProgressBar::setValue(int value) {
    if (value < 0 || value > 100) {
        throw out_of_range("value out of range: " + to_string(value));
    }
    _value = value;
}

} // namespace gui

} // namespace reone
