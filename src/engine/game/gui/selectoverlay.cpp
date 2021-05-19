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

#include "selectoverlay.h"

#include <unordered_map>

#include "../../graphics/font.h"
#include "../../graphics/fonts.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/shader/shaders.h"
#include "../../graphics/stateutil.h"
#include "../../graphics/texture/texture.h"
#include "../../graphics/texture/textures.h"
#include "../../graphics/window.h"
#include "../../resource/resources.h"

#include "../d20/feats.h"
#include "../game.h"
#include "../objectconverter.h"
#include "../reputes.h"

#include "colorutil.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static constexpr int kOffsetToReticle = 8;
static constexpr int kTitleBarWidth = 250;
static constexpr int kTitleBarPadding = 6;
static constexpr int kHealthBarHeight = 6;
static constexpr int kNumActionSlots = 3;
static constexpr int kActionBarMargin = 3;
static constexpr int kActionBarPadding = 3;
static constexpr int kActionWidth = 35;
static constexpr int kActionHeight = 59;

SelectionOverlay::SelectionOverlay(Game *game) : _game(game) {
    if (!game) {
        throw invalid_argument("game must not be null");
    }
    _actionSlots.resize(kNumActionSlots);
}

void SelectionOverlay::load() {
    _font = Fonts::instance().get("dialogfont16x16");
    _friendlyReticle = Textures::instance().get("friendlyreticle", TextureUsage::GUI);
    _friendlyReticle2 = Textures::instance().get("friendlyreticle2", TextureUsage::GUI);
    _hostileReticle = Textures::instance().get("hostilereticle", TextureUsage::GUI);
    _hostileReticle2 = Textures::instance().get("hostilereticle2", TextureUsage::GUI);
    _friendlyScroll = Textures::instance().get("lbl_miscroll_f", TextureUsage::GUI);
    _hostileScroll = Textures::instance().get("lbl_miscroll_h", TextureUsage::GUI);
    _hilightedScroll = Textures::instance().get("lbl_miscroll_hi", TextureUsage::GUI);
    _reticleHeight = _friendlyReticle2->height();

    addTextureByAction(ContextualAction::Unlock, "isk_security");
    addTextureByAction(ContextualAction::Attack, "i_attack");

    // TODO: different icons per feat level
    _textureByAction.insert(make_pair(ContextualAction::PowerAttack, _game->feats().get(FeatType::PowerAttack)->icon));
    _textureByAction.insert(make_pair(ContextualAction::CriticalStrike, _game->feats().get(FeatType::CriticalStrike)->icon));
    _textureByAction.insert(make_pair(ContextualAction::Flurry, _game->feats().get(FeatType::Flurry)->icon));
    _textureByAction.insert(make_pair(ContextualAction::PowerShot, _game->feats().get(FeatType::PowerBlast)->icon));
    _textureByAction.insert(make_pair(ContextualAction::SniperShot, _game->feats().get(FeatType::SniperShot)->icon));
    _textureByAction.insert(make_pair(ContextualAction::RapidShot, _game->feats().get(FeatType::RapidShot)->icon));
}

void SelectionOverlay::addTextureByAction(ContextualAction action, const string &resRef) {
    _textureByAction.insert(make_pair(action, Textures::instance().get(resRef, TextureUsage::GUI)));
}

bool SelectionOverlay::handle(const SDL_Event &event) {
    switch (event.type) {
        case SDL_MOUSEMOTION:
            return handleMouseMotion(event.motion);
        case SDL_MOUSEBUTTONDOWN:
            return handleMouseButtonDown(event.button);
        case SDL_MOUSEWHEEL:
            return handleMouseWheel(event.wheel);
        default:
            return false;
    }
}

bool SelectionOverlay::handleMouseMotion(const SDL_MouseMotionEvent &event) {
    _selectedActionSlot = -1;

    if (!_selectedObject) return false;

    for (int i = 0; i < kNumActionSlots; ++i) {
        float x, y;
        getActionScreenCoords(i, x, y);
        if (event.x >= x && event.y >= y && event.x < x + kActionWidth && event.y < y + kActionHeight) {
            _selectedActionSlot = i;
            return true;
        }
    }

    return false;
}

bool SelectionOverlay::handleMouseButtonDown(const SDL_MouseButtonEvent &event) {
    if (event.button != SDL_BUTTON_LEFT) return false;
    if (_selectedActionSlot == -1 || _selectedActionSlot >= _actionSlots.size()) return false;

    shared_ptr<Creature> leader(_game->party().getLeader());
    if (!leader) return false;

    shared_ptr<Area> area(_game->module()->area());
    auto selectedObject = area->selectedObject();
    if (!selectedObject) return false;

    const ActionSlot &slot = _actionSlots[_selectedActionSlot];
    if (slot.indexSelected >= slot.actions.size()) return false;

    switch (slot.actions[slot.indexSelected]) {
        case ContextualAction::Unlock: {
            leader->addAction(make_unique<ObjectAction>(ActionType::OpenLock, selectedObject));
            break;
        }
        case ContextualAction::Attack:
        case ContextualAction::PowerAttack:
        case ContextualAction::CriticalStrike:
        case ContextualAction::Flurry:
        case ContextualAction::PowerShot:
        case ContextualAction::SniperShot:
        case ContextualAction::RapidShot: {
            leader->addAction(make_unique<AttackAction>(static_pointer_cast<Creature>(selectedObject), leader->getAttackRange(), true));
            break;
        }
        default:
            break;
    }

    return true;
}

bool SelectionOverlay::handleMouseWheel(const SDL_MouseWheelEvent &event) {
    if (_selectedActionSlot == -1 || _selectedActionSlot >= _actionSlots.size()) return false;

    ActionSlot &slot = _actionSlots[_selectedActionSlot];
    size_t numSlotActions = slot.actions.size();

    if (event.y > 0) {
        if (slot.indexSelected-- == 0) {
            slot.indexSelected = numSlotActions - 1;
        }
    } else {
        if (++slot.indexSelected == numSlotActions) {
            slot.indexSelected = 0;
        }
    }

    return true;
}

void SelectionOverlay::update() {
    // TODO: update on selection change only

    _hilightedObject.reset();
    _hilightedHostile = false;

    _selectedObject.reset();
    _selectedHostile = false;

    shared_ptr<Module> module(_game->module());
    shared_ptr<Area> area(module->area());

    Camera *camera = _game->getActiveCamera();
    glm::mat4 projection(camera->sceneNode()->projection());
    glm::mat4 view(camera->sceneNode()->view());

    auto hilightedObject = area->hilightedObject();
    if (hilightedObject) {
        _hilightedScreenCoords = area->getSelectableScreenCoords(hilightedObject, projection, view);

        if (_hilightedScreenCoords.z < 1.0f) {
            _hilightedObject = hilightedObject;

            auto hilightedCreature = ObjectConverter::toCreature(hilightedObject);
            if (hilightedCreature) {
                _hilightedHostile = !hilightedCreature->isDead() && _game->reputes().getIsEnemy(*(_game->party().getLeader()), *hilightedCreature);
            }
        }
    }

    auto selectedObject = area->selectedObject();
    if (selectedObject) {
        _selectedScreenCoords = area->getSelectableScreenCoords(selectedObject, projection, view);

        if (_selectedScreenCoords.z < 1.0f) {
            _selectedObject = selectedObject;

            for (int i = 0; i < kNumActionSlots; ++i) {
                _actionSlots[i].actions.clear();
            }
            set<ContextualAction> actions(module->getContextualActions(selectedObject));
            _hasActions = !actions.empty();
            if (_hasActions) {
                if (actions.count(ContextualAction::Attack) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::Attack);
                }
                if (actions.count(ContextualAction::PowerAttack) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::PowerAttack);
                }
                if (actions.count(ContextualAction::CriticalStrike) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::CriticalStrike);
                }
                if (actions.count(ContextualAction::Flurry) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::Flurry);
                }
                if (actions.count(ContextualAction::PowerShot) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::PowerShot);
                }
                if (actions.count(ContextualAction::SniperShot) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::SniperShot);
                }
                if (actions.count(ContextualAction::RapidShot) > 0) {
                    _actionSlots[0].actions.push_back(ContextualAction::RapidShot);
                }
                if (actions.count(ContextualAction::Unlock) > 0) {
                    _actionSlots[1].actions.push_back(ContextualAction::Unlock);
                }
            }
            for (int i = 0; i < kNumActionSlots; ++i) {
                if (_actionSlots[i].indexSelected >= _actionSlots[i].actions.size()) {
                    _actionSlots[i].indexSelected = 0;
                }
            }

            auto selectedCreature = ObjectConverter::toCreature(selectedObject);
            if (selectedCreature) {
                _selectedHostile = !selectedCreature->isDead() && _game->reputes().getIsEnemy(*_game->party().getLeader(), *selectedCreature);
            }
        }
    }
}

void SelectionOverlay::draw() {
    if (_hilightedObject) {
        drawReticle(_hilightedHostile ? *_hostileReticle : *_friendlyReticle, _hilightedScreenCoords);
    }
    if (_selectedObject) {
        drawReticle(_selectedHostile ? *_hostileReticle2 : *_friendlyReticle2, _selectedScreenCoords);
        drawActionBar();
        drawTitleBar();
        drawHealthBar();
    }
}

void SelectionOverlay::drawReticle(Texture &texture, const glm::vec3 &screenCoords) {
    setActiveTextureUnit(TextureUnits::diffuseMap);
    texture.bind();

    const GraphicsOptions &opts = _game->options().graphics;
    int width = texture.width();
    int height = texture.height();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3((opts.width * screenCoords.x) - width / 2, (opts.height * (1.0f - screenCoords.y)) - height / 2, 0.0f));
    transform = glm::scale(transform, glm::vec3(width, height, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _game->window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
    Meshes::instance().getQuad()->draw();
}

void SelectionOverlay::drawTitleBar() {
    if (_selectedObject->name().empty()) return;

    const GraphicsOptions &opts = _game->options().graphics;
    float barHeight = _font->height() + kTitleBarPadding;
    {
        float x = opts.width * _selectedScreenCoords.x - kTitleBarWidth / 2;
        float y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2 - barHeight - kOffsetToReticle - kHealthBarHeight - 1.0f;

        if (_hasActions) {
            y -= kActionHeight + 2 * kActionBarMargin;
        }
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
        transform = glm::scale(transform, glm::vec3(kTitleBarWidth, barHeight, 1.0f));

        ShaderUniforms uniforms;
        uniforms.combined.general.projection = _game->window().getOrthoProjection();
        uniforms.combined.general.model = move(transform);
        uniforms.combined.general.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        uniforms.combined.general.alpha = 0.5f;

        Shaders::instance().activate(ShaderProgram::SimpleColor, uniforms);
        Meshes::instance().getQuad()->draw();
    }
    {
        float x = opts.width * _selectedScreenCoords.x;
        float y = opts.height * (1.0f - _selectedScreenCoords.y) - (_reticleHeight + barHeight) / 2 - kOffsetToReticle - kHealthBarHeight - 1.0f;
        if (_hasActions) {
            y -= kActionHeight + 2 * kActionBarMargin;
        }
        glm::vec3 position(x, y, 0.0f);
        _font->draw(_selectedObject->name(), position, getColorFromSelectedObject());
    }
}

void SelectionOverlay::drawHealthBar() {
    const GraphicsOptions &opts = _game->options().graphics;
    float x = opts.width * _selectedScreenCoords.x - kTitleBarWidth / 2;
    float y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2 - kHealthBarHeight - kOffsetToReticle;
    float w = glm::clamp(_selectedObject->currentHitPoints() / static_cast<float>(_selectedObject->hitPoints()), 0.0f, 1.0f) * kTitleBarWidth;

    if (_hasActions) {
        y -= kActionHeight + 2 * kActionBarMargin;
    }
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, kHealthBarHeight, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _game->window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);
    uniforms.combined.general.color = glm::vec4(getColorFromSelectedObject(), 1.0f);

    Shaders::instance().activate(ShaderProgram::SimpleColor, uniforms);
    Meshes::instance().getQuad()->draw();
}

void SelectionOverlay::drawActionBar() {
    if (!_hasActions) return;

    for (int i = 0; i < kNumActionSlots; ++i) {
        drawActionFrame(i);
        drawActionIcon(i);
    }
}

void SelectionOverlay::drawActionFrame(int index) {
    shared_ptr<Texture> frameTexture;
    if (index == _selectedActionSlot) {
        frameTexture = _hilightedScroll;
    } else if (_selectedHostile) {
        frameTexture = _hostileScroll;
    } else {
        frameTexture = _friendlyScroll;
    }
    setActiveTextureUnit(TextureUnits::diffuseMap);
    frameTexture->bind();

    float frameX, frameY;
    getActionScreenCoords(index, frameX, frameY);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(frameX, frameY, 0.0f));
    transform = glm::scale(transform, glm::vec3(kActionWidth, kActionHeight, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _game->window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
    Meshes::instance().getQuad()->draw();
}

bool SelectionOverlay::getActionScreenCoords(int index, float &x, float &y) const {
    if (!_selectedObject) return false;

    const GraphicsOptions &opts = _game->options().graphics;
    x = opts.width * _selectedScreenCoords.x + (static_cast<float>(index - 1) - 0.5f) * kActionWidth + (index - 1) * kActionBarMargin;
    y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2 - kActionHeight - kOffsetToReticle - kActionBarMargin;

    return true;
}

void SelectionOverlay::drawActionIcon(int index) {
    const ActionSlot &slot = _actionSlots[index];
    if (slot.indexSelected >= slot.actions.size()) return;

    ContextualAction action = slot.actions[slot.indexSelected];
    shared_ptr<Texture> texture(_textureByAction.find(action)->second);
    if (!texture) return;

    setActiveTextureUnit(TextureUnits::diffuseMap);
    texture->bind();

    float frameX, frameY;
    getActionScreenCoords(index, frameX, frameY);

    const GraphicsOptions &opts = _game->options().graphics;
    float y = opts.height * (1.0f - _selectedScreenCoords.y) - (_reticleHeight + kActionHeight + kActionWidth) / 2.0f - kOffsetToReticle - kActionBarMargin;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(frameX, y, 0.0f));
    transform = glm::scale(transform, glm::vec3(kActionWidth, kActionWidth, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _game->window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);

    Shaders::instance().activate(ShaderProgram::SimpleGUI, uniforms);
    Meshes::instance().getQuad()->draw();
}

glm::vec3 SelectionOverlay::getColorFromSelectedObject() const {
    static glm::vec3 red(1.0f, 0.0f, 0.0f);

    return (_selectedObject && _selectedHostile) ?
        red :
        getBaseColor(_game->gameId());
}

} // namespace game

} // namespace reone
