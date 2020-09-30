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

#include "target.h"

#include "GL/glew.h"

#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../resources/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

TargetOverlay::TargetOverlay(const GraphicsOptions &opts) : _opts(opts) {
}

void TargetOverlay::load() {
    _friendlyReticle = Resources.findTexture("friendlyreticle", TextureType::GUI);
    _friendlyReticle2 = Resources.findTexture("friendlyreticle2", TextureType::GUI);
}

void TargetOverlay::render() const {
    if (_context.hasHilighted) {
        drawReticle(*_friendlyReticle, _context.hilightedScreenCoords);
    }
    if (_context.hasSelected) {
        drawReticle(*_friendlyReticle2, _context.selectedScreenCoords);
    }
}

void TargetOverlay::drawReticle(Texture &texture, const glm::vec3 &screenCoords) const {
    int width = texture.width();
    int height = texture.height();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3((_opts.width * screenCoords.x) - width / 2, (_opts.height * (1.0f - screenCoords.y)) - height / 2, 0.0f));
    transform = glm::scale(transform, glm::vec3(width, height, 1.0f));

    LocalUniforms locals;
    locals.model = move(transform);

    Shaders.activate(ShaderProgram::GUIGUI, locals);

    glActiveTexture(GL_TEXTURE0);
    texture.bind();

    DefaultQuad.render(GL_TRIANGLES);

    texture.unbind();
}

void TargetOverlay::setContext(const TargetContext &ctx) {
    _context = ctx;
}

} // namespace game

} // namespace reone
