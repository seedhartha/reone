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

#include "reone/game/gui/selectoverlay.h"

#include "reone/graphics/context.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/font.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/texture.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/resource/provider/fonts.h"
#include "reone/resource/provider/textures.h"
#include "reone/resource/resources.h"

#include "reone/game/action/attackobject.h"
#include "reone/game/action/usefeat.h"
#include "reone/game/action/useskill.h"
#include "reone/game/d20/feat.h"
#include "reone/game/d20/feats.h"
#include "reone/game/d20/skill.h"
#include "reone/game/d20/skills.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/party.h"
#include "reone/game/reputes.h"

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

static std::string g_attackIcon("i_attack");

SelectionOverlay::SelectionOverlay(
    Game &game,
    ServicesView &services) :
    _game(game),
    _services(services) {
    _actionSlots.resize(kNumActionSlots);
}

void SelectionOverlay::init() {
    _font = _services.resource.fonts.get("dialogfont16x16");
    _friendlyReticle = _services.resource.textures.get("friendlyreticle", TextureUsage::GUI);
    _friendlyReticle2 = _services.resource.textures.get("friendlyreticle2", TextureUsage::GUI);
    _hostileReticle = _services.resource.textures.get("hostilereticle", TextureUsage::GUI);
    _hostileReticle2 = _services.resource.textures.get("hostilereticle2", TextureUsage::GUI);
    _friendlyScroll = _services.resource.textures.get("lbl_miscroll_f", TextureUsage::GUI);
    _hostileScroll = _services.resource.textures.get("lbl_miscroll_h", TextureUsage::GUI);
    _hilightedScroll = _services.resource.textures.get("lbl_miscroll_hi", TextureUsage::GUI);
    _reticleHeight = _friendlyReticle2->height();
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

    if (!_selectedObject)
        return false;

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
    if (event.button != SDL_BUTTON_LEFT)
        return false;
    if (_selectedActionSlot == -1 || _selectedActionSlot >= _actionSlots.size())
        return false;

    std::shared_ptr<Creature> leader(_game.party().getLeader());
    if (!leader)
        return false;

    std::shared_ptr<Area> area(_game.module()->area());
    auto selectedObject = area->selectedObject();
    if (!selectedObject)
        return false;

    const ActionSlot &slot = _actionSlots[_selectedActionSlot];
    if (slot.indexSelected >= slot.actions.size())
        return false;

    const ContextAction &ctxAction = slot.actions[slot.indexSelected];
    std::shared_ptr<Action> action;
    switch (ctxAction.type) {
    case ActionType::AttackObject:
        action = _game.newAction<AttackObjectAction>(selectedObject);
        break;
    case ActionType::UseFeat:
        action = _game.newAction<UseFeatAction>(ctxAction.feat, selectedObject);
        break;
    case ActionType::UseSkill:
        action = _game.newAction<UseSkillAction>(ctxAction.skill, selectedObject);
        break;
    default:
        break;
    }
    if (action) {
        action->setUserAction(true);
        leader->addAction(std::move(action));
    }

    return true;
}

bool SelectionOverlay::handleMouseWheel(const SDL_MouseWheelEvent &event) {
    if (_selectedActionSlot == -1 || _selectedActionSlot >= _actionSlots.size())
        return false;

    ActionSlot &slot = _actionSlots[_selectedActionSlot];
    size_t numSlotActions = slot.actions.size();

    if (event.y > 0) {
        if (slot.indexSelected-- == 0) {
            slot.indexSelected = static_cast<uint32_t>(numSlotActions - 1);
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

    std::shared_ptr<Module> module(_game.module());
    std::shared_ptr<Area> area(module->area());

    auto camera = _game.getActiveCamera();
    glm::mat4 projection(camera->cameraSceneNode()->camera()->projection());
    glm::mat4 view(camera->cameraSceneNode()->camera()->view());

    auto hilightedObject = area->hilightedObject();
    if (hilightedObject) {
        _hilightedScreenCoords = area->getSelectableScreenCoords(hilightedObject, projection, view);

        if (_hilightedScreenCoords.z < 1.0f) {
            _hilightedObject = hilightedObject;

            auto hilightedCreature = std::dynamic_pointer_cast<Creature>(hilightedObject);
            if (hilightedCreature) {
                _hilightedHostile = !hilightedCreature->isDead() && _services.game.reputes.getIsEnemy(*(_game.party().getLeader()), *hilightedCreature);
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
            std::vector<ContextAction> actions(module->getContextActions(selectedObject));
            _hasActions = !actions.empty();
            if (_hasActions) {
                for (auto &action : actions) {
                    switch (action.type) {
                    case ActionType::AttackObject:
                    case ActionType::UseFeat:
                        _actionSlots[0].actions.push_back(action);
                        break;
                    case ActionType::UseSkill:
                        _actionSlots[1].actions.push_back(action);
                        break;
                    default:
                        break;
                    }
                }
            }
            for (int i = 0; i < kNumActionSlots; ++i) {
                if (_actionSlots[i].indexSelected >= _actionSlots[i].actions.size()) {
                    _actionSlots[i].indexSelected = 0;
                }
            }

            auto selectedCreature = std::dynamic_pointer_cast<Creature>(selectedObject);
            if (selectedCreature) {
                _selectedHostile = !selectedCreature->isDead() && _services.game.reputes.getIsEnemy(*_game.party().getLeader(), *selectedCreature);
            }
        }
    }
}

void SelectionOverlay::draw() {
    _services.graphics.context.withBlending(BlendMode::Normal, [this]() {
        if (_hilightedObject) {
            drawReticle(_hilightedHostile ? _hostileReticle : _friendlyReticle, _hilightedScreenCoords);
        }
        if (_selectedObject) {
            drawReticle(_selectedHostile ? _hostileReticle2 : _friendlyReticle2, _selectedScreenCoords);
            drawActionBar();
            drawTitleBar();
            drawHealthBar();
        }
    });
}

void SelectionOverlay::drawReticle(std::shared_ptr<Texture> texture, const glm::vec3 &screenCoords) {
    _services.graphics.context.bind(*texture);

    const GraphicsOptions &opts = _game.options().graphics;
    int width = texture->width();
    int height = texture->height();

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3((opts.width * screenCoords.x) - width / 2, (opts.height * (1.0f - screenCoords.y)) - height / 2, 0.0f));
    transform = glm::scale(transform, glm::vec3(width, height, 1.0f));

    _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _services.graphics.window.getOrthoProjection();
    });
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture2D));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

void SelectionOverlay::drawTitleBar() {
    if (_selectedObject->name().empty())
        return;

    const GraphicsOptions &opts = _game.options().graphics;
    float barHeight = _font->height() + kTitleBarPadding;
    {
        float x = opts.width * _selectedScreenCoords.x - kTitleBarWidth / 2;
        float y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2.0f - barHeight - kOffsetToReticle - kHealthBarHeight - 1.0f;

        if (_hasActions) {
            y -= kActionHeight + 2 * kActionBarMargin;
        }
        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
        transform = glm::scale(transform, glm::vec3(kTitleBarWidth, barHeight, 1.0f));

        _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
            globals.projection = _services.graphics.window.getOrthoProjection();
        });
        _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
            locals.reset();
            locals.model = std::move(transform);
            locals.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            locals.color.a = 0.5f;
        });
        _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::color2D));
        _services.graphics.meshRegistry.get(MeshName::quad).draw();
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
    const GraphicsOptions &opts = _game.options().graphics;
    float x = opts.width * _selectedScreenCoords.x - kTitleBarWidth / 2;
    float y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2.0f - kHealthBarHeight - kOffsetToReticle;
    float w = glm::clamp(_selectedObject->currentHitPoints() / static_cast<float>(_selectedObject->hitPoints()), 0.0f, 1.0f) * kTitleBarWidth;

    if (_hasActions) {
        y -= kActionHeight + 2 * kActionBarMargin;
    }
    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, kHealthBarHeight, 1.0f));

    _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _services.graphics.window.getOrthoProjection();
    });
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
        locals.color = glm::vec4(getColorFromSelectedObject(), 1.0f);
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::color2D));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

void SelectionOverlay::drawActionBar() {
    if (!_hasActions)
        return;

    for (int i = 0; i < kNumActionSlots; ++i) {
        drawActionFrame(i);
        drawActionIcon(i);
    }
}

void SelectionOverlay::drawActionFrame(int index) {
    std::shared_ptr<Texture> frameTexture;
    if (index == _selectedActionSlot) {
        frameTexture = _hilightedScroll;
    } else if (_selectedHostile) {
        frameTexture = _hostileScroll;
    } else {
        frameTexture = _friendlyScroll;
    }
    _services.graphics.context.bind(*frameTexture);

    float frameX, frameY;
    getActionScreenCoords(index, frameX, frameY);

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(frameX, frameY, 0.0f));
    transform = glm::scale(transform, glm::vec3(kActionWidth, kActionHeight, 1.0f));

    _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _services.graphics.window.getOrthoProjection();
    });
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture2D));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

bool SelectionOverlay::getActionScreenCoords(int index, float &x, float &y) const {
    if (!_selectedObject)
        return false;

    const GraphicsOptions &opts = _game.options().graphics;
    x = opts.width * _selectedScreenCoords.x + (static_cast<float>(index - 1) - 0.5f) * kActionWidth + (index - 1) * kActionBarMargin;
    y = opts.height * (1.0f - _selectedScreenCoords.y) - _reticleHeight / 2.0f - kActionHeight - kOffsetToReticle - kActionBarMargin;

    return true;
}

void SelectionOverlay::drawActionIcon(int index) {
    const ActionSlot &slot = _actionSlots[index];
    if (slot.indexSelected >= slot.actions.size())
        return;

    std::shared_ptr<Texture> texture;
    const ContextAction &action = slot.actions[slot.indexSelected];
    switch (action.type) {
    case ActionType::AttackObject:
        texture = _services.resource.textures.get(g_attackIcon, TextureUsage::GUI);
        break;
    case ActionType::UseFeat: {
        std::shared_ptr<Feat> feat(_services.game.feats.get(action.feat));
        if (feat) {
            texture = feat->icon;
        }
        break;
    }
    case ActionType::UseSkill: {
        std::shared_ptr<Skill> skill(_services.game.skills.get(action.skill));
        if (skill) {
            texture = skill->icon;
        }
        break;
    }
    default:
        break;
    }
    if (!texture)
        return;

    _services.graphics.context.bind(*texture);

    float frameX, frameY;
    getActionScreenCoords(index, frameX, frameY);

    const GraphicsOptions &opts = _game.options().graphics;
    float y = opts.height * (1.0f - _selectedScreenCoords.y) - (_reticleHeight + kActionHeight + kActionWidth) / 2.0f - kOffsetToReticle - kActionBarMargin;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(frameX, y, 0.0f));
    transform = glm::scale(transform, glm::vec3(kActionWidth, kActionWidth, 1.0f));

    _services.graphics.uniforms.setGlobals([this, transform](auto &globals) {
        globals.projection = _services.graphics.window.getOrthoProjection();
    });
    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::texture2D));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

glm::vec3 SelectionOverlay::getColorFromSelectedObject() const {
    static glm::vec3 red(1.0f, 0.0f, 0.0f);

    auto guiColorBase = _game.isTSL() ? kTSLGUIColorBase : kGeometryUIColorBase;

    return (_selectedObject && _selectedHostile) ? red : guiColorBase;
}

} // namespace game

} // namespace reone
