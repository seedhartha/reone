/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "selectoverlay.h"

#include <unordered_map>

#include "../../render/font.h"
#include "../../render/fonts.h"
#include "../../render/mesh/quad.h"
#include "../../render/shaders.h"
#include "../../render/texture.h"
#include "../../render/textures.h"
#include "../../resource/resources.h"

#include "../game.h"

#include "colors.h"

using namespace std;

using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

static const int kOffsetToReticle = 8;
static const int kTitleBarWidth = 250;
static const int kTitleBarPadding = 6;
static const int kActionBarMargin = 3;
static const int kActionBarPadding = 3;
static const int kActionWidth = 35;
static const int kActionHeight = 59;

SelectionOverlay::SelectionOverlay(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("game must not be null");
    }
}

void SelectionOverlay::load() {
    _font = Fonts::instance().get("dialogfont16x16");
    _friendlyReticle = Textures::instance().get("friendlyreticle", TextureType::GUI);
    _friendlyReticle2 = Textures::instance().get("friendlyreticle2", TextureType::GUI);
    _friendlyScroll = Textures::instance().get("lbl_miscroll_f", TextureType::GUI);
    _hostileScroll = Textures::instance().get("lbl_miscroll_h", TextureType::GUI);
    _hilightedScroll = Textures::instance().get("lbl_miscroll_hi", TextureType::GUI);
    _reticleHeight = _friendlyReticle2->height();

    addTextureByAction(ContextualAction::Unlock, "isk_security");
}

void SelectionOverlay::addTextureByAction(ContextualAction action, const string &resRef) {
    _textureByAction.insert(make_pair(action, Textures::instance().get(resRef, TextureType::GUI)));
}

void SelectionOverlay::update() {
    shared_ptr<Module> module(_game->module());
    shared_ptr<Area> area(module->area());
    ObjectSelector &selector = area->objectSelector();

    Camera *camera = _game->getActiveCamera();
    glm::mat4 projection(camera->sceneNode()->projection());
    glm::mat4 view(camera->sceneNode()->view());

    int hilightedObjectId = selector.hilightedObjectId();
    if (hilightedObjectId != -1) {
        shared_ptr<SpatialObject> object(area->find(hilightedObjectId));
        _hilightedScreenCoords = area->getSelectableScreenCoords(object, projection, view);
        _hasHilighted = _hilightedScreenCoords.z < 1.0f;
    } else {
        _hasHilighted = false;
    }

    int selectedObjectId = selector.selectedObjectId();
    if (selectedObjectId != -1) {
        shared_ptr<SpatialObject> object(area->find(selectedObjectId));
        _selectedScreenCoords = area->getSelectableScreenCoords(object, projection, view);
        _hasSelected = _selectedScreenCoords.z < 1.0f;
        _selectedTitle = object->title();
        _actions = module->getContextualActions(object);
    } else {
        _hasSelected = false;
    }
}

void SelectionOverlay::render() const {
    if (_hasHilighted) {
        drawReticle(*_friendlyReticle, _hilightedScreenCoords);
    }
    if (_hasSelected) {
        drawReticle(*_friendlyReticle2, _selectedScreenCoords);
        if (!_actions.empty()) {
            drawActionBar();
        }
        drawTitleBar();
    }
}

void SelectionOverlay::drawReticle(Texture &texture, const glm::vec3 &screenCoords) const {
    const GraphicsOptions &opts = _game->options().graphics;
    int width = texture.width();
    int height = texture.height();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3((opts.width * screenCoords.x) - width / 2, (opts.height * (1.0f - screenCoords.y)) - height / 2, 0.0f));
    transform = glm::scale(transform, glm::vec3(width, height, 1.0f));

    LocalUniforms locals;
    locals.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

    texture.bind(0);

    Quad::getDefault().renderTriangles();

    texture.unbind(0);
}

void SelectionOverlay::drawTitleBar() const {
    if (_selectedTitle.empty()) return;

    const GraphicsOptions &opts = _game->options().graphics;
    float barHeight = _font->height() + kTitleBarPadding;
    {
        float x = opts.width * _selectedScreenCoords.x - kTitleBarWidth / 2;
        float y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2 - barHeight - kOffsetToReticle;

        if (!_actions.empty()) {
            y -= kActionHeight + 2 * kActionBarMargin;
        }
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
        transform = glm::scale(transform, glm::vec3(kTitleBarWidth, barHeight, 1.0f));

        LocalUniforms locals;
        locals.general.model = move(transform);
        locals.general.color = glm::vec4(0.0f);
        locals.general.alpha = 0.5f;

        Shaders::instance().activate(ShaderProgram::GUIWhite, locals);

        Quad::getDefault().renderTriangles();
    }
    {
        float x = opts.width * _selectedScreenCoords.x;
        float y = opts.height * (1.0f - _selectedScreenCoords.y) - (_reticleHeight + barHeight) / 2 - kOffsetToReticle;

        if (!_actions.empty()) {
            y -= kActionHeight + 2 * kActionBarMargin;
        }
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(x, y, 0.0f));

        glm::vec3 color(getBaseColor(_game->version()));

        _font->render(_selectedTitle, transform, color);
    }
}

void SelectionOverlay::drawActionBar() const {
    const GraphicsOptions &opts = _game->options().graphics;
    float frameY = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2 - kActionHeight - kOffsetToReticle - kActionBarMargin;

    for (int i = -1; i < 2; ++i) {
        float frameX = opts.width * _selectedScreenCoords.x + (static_cast<float>(i) - 0.5f) * kActionWidth + i * kActionBarMargin;

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(frameX, frameY, 0.0f));
        transform = glm::scale(transform, glm::vec3(kActionWidth, kActionHeight, 1.0f));

        LocalUniforms locals;
        locals.general.model = move(transform);

        Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

        _friendlyScroll->bind(0);

        Quad::getDefault().renderTriangles();

        _friendlyScroll->unbind(0);

        int actionIdx = i + 1;
        if (actionIdx < static_cast<int>(_actions.size())) {
            ContextualAction action = _actions[actionIdx];

            shared_ptr<Texture> texture(_textureByAction.find(action)->second);
            if (texture) {
                float y = opts.height * (1.0f - _selectedScreenCoords.y) - (_reticleHeight + kActionHeight + kActionWidth) / 2 - kOffsetToReticle - kActionBarMargin;

                transform = glm::mat4(1.0f);
                transform = glm::translate(transform, glm::vec3(frameX, y, 0.0f));
                transform = glm::scale(transform, glm::vec3(kActionWidth, kActionWidth, 1.0f));

                LocalUniforms locals;
                locals.general.model = move(transform);

                Shaders::instance().activate(ShaderProgram::GUIGUI, locals);

                texture->bind(0);

                Quad::getDefault().renderTriangles();

                texture->unbind(0);
            }
        }
    }
}

} // namespace game

} // namespace reone
