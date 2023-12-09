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

#include "reone/game/gui/hud.h"

#include "reone/graphics/context.h"
#include "reone/graphics/mesh.h"
#include "reone/graphics/meshregistry.h"
#include "reone/graphics/shaderregistry.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/window.h"
#include "reone/gui/control/label.h"
#include "reone/system/logutil.h"

#include "reone/game/action/usefeat.h"
#include "reone/game/d20/feat.h"
#include "reone/game/d20/feats.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/object/creature.h"
#include "reone/game/party.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static std::string g_attackIcon("i_attack");

void HUD::preload(IGUI &gui) {
    gui.setResolution(800, 600);
    gui.setScaling(GUI::ScalingMode::PositionRelativeToCenter);

    static std::string combatControlTags[] = {
        "BTN_CLEARALL", "BTN_CLEARONE", "BTN_CLEARONE2",
        "LBL_CMBTMODEMSG", "LBL_CMBTMSGBG", "LBL_COMBATBG1", "LBL_COMBATBG2", "LBL_COMBATBG3",
        "LBL_QUEUE0", "LBL_QUEUE1", "LBL_QUEUE2", "LBL_QUEUE3"};
    for (auto &tag : combatControlTags) {
        gui.setControlScaling(tag, GUI::ScalingMode::Stretch);
    }
}

void HUD::onGUILoaded() {
    bindControls();

    _controls.BTN_CLEARALL->setVisible(false);
    _controls.BTN_TARGET0->setVisible(false);
    _controls.BTN_TARGET1->setVisible(false);
    _controls.BTN_TARGET2->setVisible(false);
    _controls.BTN_TARGETDOWN0->setVisible(false);
    _controls.BTN_TARGETDOWN1->setVisible(false);
    _controls.BTN_TARGETDOWN2->setVisible(false);
    _controls.BTN_TARGETUP0->setVisible(false);
    _controls.BTN_TARGETUP1->setVisible(false);
    _controls.BTN_TARGETUP2->setVisible(false);
    _controls.LBL_ACTIONDESCBG->setVisible(false);
    _controls.LBL_ACTIONDESC->setVisible(false);
    _controls.LBL_ARROW_MARGIN->setVisible(false);
    _controls.LBL_CASH->setVisible(false);
    _controls.LBL_CMBTEFCTINC1->setVisible(false);
    _controls.LBL_CMBTEFCTINC2->setVisible(false);
    _controls.LBL_CMBTEFCTINC3->setVisible(false);
    _controls.LBL_CMBTEFCTRED1->setVisible(false);
    _controls.LBL_CMBTEFCTRED2->setVisible(false);
    _controls.LBL_CMBTEFCTRED3->setVisible(false);
    _controls.LBL_CMBTMODEMSG->setVisible(false);
    _controls.LBL_CMBTMSGBG->setVisible(false);
    _controls.LBL_COMBATBG3->setVisible(false);
    _controls.LBL_DARKSHIFT->setVisible(false);
    _controls.LBL_DEBILATATED1->setVisible(false);
    _controls.LBL_DEBILATATED2->setVisible(false);
    _controls.LBL_DEBILATATED3->setVisible(false);
    _controls.LBL_DISABLE1->setVisible(false);
    _controls.LBL_DISABLE2->setVisible(false);
    _controls.LBL_DISABLE3->setVisible(false);
    _controls.LBL_JOURNAL->setVisible(false);
    _controls.LBL_HEALTHBG->setVisible(false);
    _controls.LBL_ITEMRCVD->setVisible(false);
    _controls.LBL_ITEMLOST->setVisible(false);
    _controls.LBL_LIGHTSHIFT->setVisible(false);
    _controls.LBL_MAP->setVisible(false);
    _controls.LBL_MOULDING1->setVisible(false);
    _controls.LBL_MOULDING3->setVisible(false);
    _controls.LBL_NAME->setVisible(false);
    _controls.LBL_NAMEBG->setVisible(false);
    _controls.LBL_PLOTXP->setVisible(false);
    _controls.LBL_STEALTHXP->setVisible(false);
    _controls.BTN_ACTION0->setVisible(false);
    _controls.BTN_ACTION1->setVisible(false);
    _controls.BTN_ACTION2->setVisible(false);
    _controls.BTN_ACTION3->setVisible(false);
    _controls.BTN_ACTIONDOWN0->setVisible(false);
    _controls.BTN_ACTIONDOWN1->setVisible(false);
    _controls.BTN_ACTIONDOWN2->setVisible(false);
    _controls.BTN_ACTIONDOWN3->setVisible(false);
    _controls.BTN_ACTIONUP0->setVisible(false);
    _controls.BTN_ACTIONUP1->setVisible(false);
    _controls.BTN_ACTIONUP2->setVisible(false);
    _controls.BTN_ACTIONUP3->setVisible(false);
    _controls.LBL_ARROW->setVisible(false);
    _controls.BTN_TARGET0->setVisible(false);
    _controls.TB_PAUSE->setVisible(false);
    _controls.TB_SOLO->setVisible(false);
    _controls.TB_STEALTH->setVisible(false);

    if (_game.isTSL()) {
        _controls.BTN_SWAPWEAPONS->setVisible(false);
        _controls.BTN_ACTION4->setVisible(false);
        _controls.BTN_ACTION5->setVisible(false);
        _controls.BTN_ACTIONDOWN4->setVisible(false);
        _controls.BTN_ACTIONDOWN5->setVisible(false);
        _controls.BTN_ACTIONUP4->setVisible(false);
        _controls.BTN_ACTIONUP5->setVisible(false);
    } else {
        _controls.LBL_COMBATBG1->setVisible(false);
        _controls.LBL_COMBATBG2->setVisible(false);
        _controls.LBL_MOULDING2->setVisible(false);
    }

    _controls.BTN_EQU->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Equipment);
    });
    _controls.BTN_INV->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Inventory);
    });
    _controls.BTN_CHAR->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Character);
    });
    _controls.BTN_ABI->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Abilities);
    });
    _controls.BTN_MSG->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Messages);
    });
    _controls.BTN_JOU->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Journal);
    });
    _controls.BTN_MAP->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Map);
    });
    _controls.BTN_OPT->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Options);
    });
    _controls.BTN_CLEARALL->setOnClick([this]() {
        _game.party().getLeader()->clearAllActions();
    });
    _controls.BTN_CLEARONE->setOnClick([this]() {
        for (auto &action : _game.party().getLeader()->actions()) {
            if (action->type() == ActionType::AttackObject) {
                action->complete();
                break;
            }
        }
    });
    _controls.BTN_CLEARONE2->setOnClick([this]() {
        for (auto &action : _game.party().getLeader()->actions()) {
            if (action->type() == ActionType::AttackObject) {
                action->complete();
                break;
            }
        }
    });
    _controls.BTN_CHAR1->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Equipment);
    });
    _controls.BTN_CHAR2->setOnClick([this]() {
        _game.party().setPartyLeaderByIndex(1);
    });
    _controls.BTN_CHAR3->setOnClick([this]() {
        _game.party().setPartyLeaderByIndex(2);
    });

    _select.init();

    _barkBubble = std::make_unique<BarkBubble>(_game, _services);
    _barkBubble->init();
}

bool HUD::handle(const SDL_Event &event) {
    if (_select.handle(event)) {
        return true;
    }
    return _gui->handle(event);
}

void HUD::update(float dt) {
    _gui->update(dt);

    Party &party = _game.party();
    std::vector<Label *> charLabels {
        _controls.LBL_CHAR1.get(),
        _controls.LBL_CHAR2.get(),
        _controls.LBL_CHAR3.get()};
    std::vector<Label *> backLabels {
        _controls.LBL_BACK1.get(),
        _controls.LBL_BACK2.get(),
        _controls.LBL_BACK3.get()};
    std::vector<Label *> lvlUpBgLabels {
        _controls.LBL_LVLUPBG1.get(),
        _controls.LBL_LVLUPBG2.get(),
        _controls.LBL_LVLUPBG3.get()};
    std::vector<Label *> levevlUpLabels {
        _controls.LBL_LEVELUP1.get(),
        _controls.LBL_LEVELUP2.get(),
        _controls.LBL_LEVELUP3.get()};

    for (int i = 0; i < 3; ++i) {
        Label &LBL_CHAR = *charLabels[i];
        Label &LBL_BACK = *backLabels[i];
        Label &LBL_LEVELUP = *levevlUpLabels[i];

        Label *LBL_LVLUPBG;
        if (!_game.isTSL()) {
            LBL_LVLUPBG = lvlUpBgLabels[i];
        }

        std::shared_ptr<Creature> member(party.getMember(i));
        if (member) {
            LBL_CHAR.setVisible(true);
            LBL_CHAR.setBorderFill(member->portrait());
            LBL_BACK.setVisible(true);
            LBL_LEVELUP.setVisible(member->isLevelUpPending());
            if (!_game.isTSL()) {
                LBL_LVLUPBG->setVisible(member->isLevelUpPending());
            }
        } else {
            LBL_CHAR.setVisible(false);
            LBL_BACK.setVisible(false);
            LBL_LEVELUP.setVisible(false);
            if (!_game.isTSL()) {
                LBL_LVLUPBG->setVisible(false);
            }
        }
    }

    if (party.getLeader()->isInCombat()) {
        toggleCombat(true);
        refreshActionQueueItems();
    } else {
        toggleCombat(false);
    }

    _select.update();
    _barkBubble->update(dt);

    // Hide minimap when there is no image to display
    _controls.LBL_MAPBORDER->setVisible(_game.map().isLoaded());
}

void HUD::render() {
    _gui->render();

    renderMinimap();

    Party &party = _game.party();
    for (int i = 0; i < party.getSize(); ++i) {
        renderHealth(i);
    }

    _barkBubble->render();
    _select.render();
}

void HUD::renderMinimap() {
    const Control::Extent &extent = _controls.LBL_MAPVIEW->extent();

    glm::vec4 bounds;
    bounds[0] = static_cast<float>(_gui->controlOffset().x + extent.left);
    bounds[1] = static_cast<float>(_gui->controlOffset().y + extent.top);
    bounds[2] = static_cast<float>(extent.width);
    bounds[3] = static_cast<float>(extent.height);

    std::shared_ptr<Area> area(_game.module()->area());
    _game.map().render(Map::Mode::Minimap, bounds);
}

void HUD::renderHealth(int memberIndex) {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> member(party.getMember(memberIndex));
    std::vector<Label *> backLabels {
        _controls.LBL_BACK1.get(),
        _controls.LBL_BACK2.get(),
        _controls.LBL_BACK3.get()};

    Label &LBL_CHAR = *backLabels[memberIndex];
    const Control::Extent &extent = LBL_CHAR.extent();

    float w = 5.0f;
    float h = glm::clamp(member->currentHitPoints() / static_cast<float>(member->hitPoints()), 0.0f, 1.0f) * extent.height;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_gui->controlOffset().x + extent.left + extent.width - 14.0f, _gui->controlOffset().y + extent.top + extent.height - h, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    _services.graphics.uniforms.setLocals([this, transform](auto &locals) {
        locals.reset();
        locals.model = std::move(transform);
        locals.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    });
    _services.graphics.context.useProgram(_services.graphics.shaderRegistry.get(ShaderProgramId::mvpColor));
    _services.graphics.meshRegistry.get(MeshName::quad).draw();
}

void HUD::toggleCombat(bool enabled) {
    _controls.BTN_CLEARALL->setVisible(enabled);
    _controls.BTN_CLEARONE->setVisible(enabled);
    _controls.BTN_CLEARONE2->setVisible(enabled);
    _controls.LBL_CMBTMODEMSG->setVisible(enabled);
    _controls.LBL_CMBTMSGBG->setVisible(enabled);
    _controls.LBL_QUEUE0->setVisible(enabled);
    _controls.LBL_QUEUE1->setVisible(enabled);
    _controls.LBL_QUEUE2->setVisible(enabled);
    _controls.LBL_QUEUE3->setVisible(enabled);

    if (!_game.isTSL()) {
        _controls.LBL_COMBATBG1->setVisible(enabled);
        _controls.LBL_COMBATBG2->setVisible(enabled);
        _controls.LBL_COMBATBG3->setVisible(enabled);
    }
}

void HUD::refreshActionQueueItems() const {
    auto &actions = _game.party().getLeader()->actions();
    std::vector<Label *> queueLabels {
        _controls.LBL_QUEUE0.get(),
        _controls.LBL_QUEUE1.get(),
        _controls.LBL_QUEUE2.get(),
        _controls.LBL_QUEUE3.get()};

    for (int i = 0; i < 4; ++i) {
        Label &item = *queueLabels[i];
        if (i < static_cast<int>(actions.size())) {
            switch (actions[i]->type()) {
            case ActionType::AttackObject:
                item.setBorderFill(g_attackIcon);
                break;
            case ActionType::UseFeat: {
                auto featAction = std::static_pointer_cast<UseFeatAction>(actions[i]);
                std::shared_ptr<Feat> feat(_services.game.feats.get(featAction->feat()));
                if (feat) {
                    item.setBorderFill(feat->icon);
                }
                break;
            }
            default:
                break;
            }
        } else {
            item.setBorderFill("");
        }
    }
}

} // namespace game

} // namespace reone
