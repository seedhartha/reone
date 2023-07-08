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
#include "reone/graphics/meshes.h"
#include "reone/graphics/shaders.h"
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

    _binding.btnClearAll->setVisible(false);
    _binding.btnTarget0->setVisible(false);
    _binding.btnTarget1->setVisible(false);
    _binding.btnTarget2->setVisible(false);
    _binding.btnTargetDown0->setVisible(false);
    _binding.btnTargetDown1->setVisible(false);
    _binding.btnTargetDown2->setVisible(false);
    _binding.btnTargetUp0->setVisible(false);
    _binding.btnTargetUp1->setVisible(false);
    _binding.btnTargetUp2->setVisible(false);
    _binding.lblActionDescBg->setVisible(false);
    _binding.lblActionDesc->setVisible(false);
    _binding.lblArrowMargin->setVisible(false);
    _binding.lblCash->setVisible(false);
    _binding.lblCmbtEfctInc1->setVisible(false);
    _binding.lblCmbtEfctInc2->setVisible(false);
    _binding.lblCmbtEfctInc3->setVisible(false);
    _binding.lblCmbtEfctRed1->setVisible(false);
    _binding.lblCmbtEfctRed2->setVisible(false);
    _binding.lblCmbtEfctRed3->setVisible(false);
    _binding.lblCmbtModeMsg->setVisible(false);
    _binding.lblCmbtMsgBg->setVisible(false);
    _binding.lblCombatBg3->setVisible(false);
    _binding.lblDarkShift->setVisible(false);
    _binding.lblDebilatated1->setVisible(false);
    _binding.lblDebilatated2->setVisible(false);
    _binding.lblDebilatated3->setVisible(false);
    _binding.lblDisable1->setVisible(false);
    _binding.lblDisable2->setVisible(false);
    _binding.lblDisable3->setVisible(false);
    _binding.lblJournal->setVisible(false);
    _binding.lblHealthBg->setVisible(false);
    _binding.lblItemRcvd->setVisible(false);
    _binding.lblItemLost->setVisible(false);
    _binding.lblLightShift->setVisible(false);
    _binding.lblMap->setVisible(false);
    _binding.lblMoulding1->setVisible(false);
    _binding.lblMoulding3->setVisible(false);
    _binding.lblName->setVisible(false);
    _binding.lblNameBg->setVisible(false);
    _binding.lblPlotXp->setVisible(false);
    _binding.lblStealthXp->setVisible(false);
    _binding.btnAction0->setVisible(false);
    _binding.btnAction1->setVisible(false);
    _binding.btnAction2->setVisible(false);
    _binding.btnAction3->setVisible(false);
    _binding.btnActionDown0->setVisible(false);
    _binding.btnActionDown1->setVisible(false);
    _binding.btnActionDown2->setVisible(false);
    _binding.btnActionDown3->setVisible(false);
    _binding.btnActionUp0->setVisible(false);
    _binding.btnActionUp1->setVisible(false);
    _binding.btnActionUp2->setVisible(false);
    _binding.btnActionUp3->setVisible(false);
    _binding.lblArrow->setVisible(false);
    _binding.btnTarget0->setVisible(false);
    _binding.tbPause->setVisible(false);
    _binding.tbSolo->setVisible(false);
    _binding.tbStealth->setVisible(false);

    if (_game.isTSL()) {
        _binding.btnSwapWeapons->setVisible(false);
        _binding.btnAction4->setVisible(false);
        _binding.btnAction5->setVisible(false);
        _binding.btnActionDown4->setVisible(false);
        _binding.btnActionDown5->setVisible(false);
        _binding.btnActionUp4->setVisible(false);
        _binding.btnActionUp5->setVisible(false);
    } else {
        _binding.lblCombatBg1->setVisible(false);
        _binding.lblCombatBg2->setVisible(false);
        _binding.lblMoulding2->setVisible(false);
    }

    _binding.btnEqu->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Equipment);
    });
    _binding.btnInv->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Inventory);
    });
    _binding.btnChar->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Character);
    });
    _binding.btnAbi->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Abilities);
    });
    _binding.btnMsg->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Messages);
    });
    _binding.btnJou->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Journal);
    });
    _binding.btnMap->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Map);
    });
    _binding.btnOpt->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Options);
    });
    _binding.btnClearAll->setOnClick([this]() {
        _game.party().getLeader()->clearAllActions();
    });
    _binding.btnClearOne->setOnClick([this]() {
        for (auto &action : _game.party().getLeader()->actions()) {
            if (action->type() == ActionType::AttackObject) {
                action->complete();
                break;
            }
        }
    });
    _binding.btnClearOne2->setOnClick([this]() {
        for (auto &action : _game.party().getLeader()->actions()) {
            if (action->type() == ActionType::AttackObject) {
                action->complete();
                break;
            }
        }
    });
    _binding.btnChar1->setOnClick([this]() {
        _game.openInGameMenu(InGameMenuTab::Equipment);
    });
    _binding.btnChar2->setOnClick([this]() {
        _game.party().setPartyLeaderByIndex(1);
    });
    _binding.btnChar3->setOnClick([this]() {
        _game.party().setPartyLeaderByIndex(2);
    });

    _select.init();

    _barkBubble = std::make_unique<BarkBubble>(_game, _services);
    _barkBubble->init();
}

void HUD::bindControls() {
    _binding.btnAbi = findControl<Button>("BTN_ABI");
    _binding.btnAction0 = findControl<Button>("BTN_ACTION0");
    _binding.btnAction1 = findControl<Button>("BTN_ACTION1");
    _binding.btnAction2 = findControl<Button>("BTN_ACTION2");
    _binding.btnAction3 = findControl<Button>("BTN_ACTION3");
    _binding.btnActionDown0 = findControl<Button>("BTN_ACTIONDOWN0");
    _binding.btnActionDown1 = findControl<Button>("BTN_ACTIONDOWN1");
    _binding.btnActionDown2 = findControl<Button>("BTN_ACTIONDOWN2");
    _binding.btnActionDown3 = findControl<Button>("BTN_ACTIONDOWN3");
    _binding.btnActionUp0 = findControl<Button>("BTN_ACTIONUP0");
    _binding.btnActionUp1 = findControl<Button>("BTN_ACTIONUP1");
    _binding.btnActionUp2 = findControl<Button>("BTN_ACTIONUP2");
    _binding.btnActionUp3 = findControl<Button>("BTN_ACTIONUP3");
    _binding.btnChar = findControl<Button>("BTN_CHAR");
    _binding.btnChar1 = findControl<Button>("BTN_CHAR1");
    _binding.btnChar2 = findControl<Button>("BTN_CHAR2");
    _binding.btnChar3 = findControl<Button>("BTN_CHAR3");
    _binding.btnClearAll = findControl<Button>("BTN_CLEARALL");
    _binding.btnClearOne = findControl<Button>("BTN_CLEARONE");
    _binding.btnClearOne2 = findControl<Button>("BTN_CLEARONE2");
    _binding.btnEqu = findControl<Button>("BTN_EQU");
    _binding.btnInv = findControl<Button>("BTN_INV");
    _binding.btnJou = findControl<Button>("BTN_JOU");
    _binding.btnMap = findControl<Button>("BTN_MAP");
    _binding.btnMinimap = findControl<Button>("BTN_MINIMAP");
    _binding.btnMsg = findControl<Button>("BTN_MSG");
    _binding.btnOpt = findControl<Button>("BTN_OPT");
    _binding.btnTarget0 = findControl<Button>("BTN_TARGET0");
    _binding.btnTarget1 = findControl<Button>("BTN_TARGET1");
    _binding.btnTarget2 = findControl<Button>("BTN_TARGET2");
    _binding.btnTargetDown0 = findControl<Button>("BTN_TARGETDOWN0");
    _binding.btnTargetDown1 = findControl<Button>("BTN_TARGETDOWN1");
    _binding.btnTargetDown2 = findControl<Button>("BTN_TARGETDOWN2");
    _binding.btnTargetUp0 = findControl<Button>("BTN_TARGETUP0");
    _binding.btnTargetUp1 = findControl<Button>("BTN_TARGETUP1");
    _binding.btnTargetUp2 = findControl<Button>("BTN_TARGETUP2");
    _binding.lblAction0 = findControl<Label>("LBL_ACTION0");
    _binding.lblAction1 = findControl<Label>("LBL_ACTION1");
    _binding.lblAction2 = findControl<Label>("LBL_ACTION2");
    _binding.lblAction3 = findControl<Label>("LBL_ACTION3");
    _binding.lblActionDesc = findControl<Label>("LBL_ACTIONDESC");
    _binding.lblActionDescBg = findControl<Label>("LBL_ACTIONDESCBG");
    _binding.lblArrow = findControl<Label>("LBL_ARROW");
    _binding.lblArrowMargin = findControl<Label>("LBL_ARROW_MARGIN");
    _binding.lblBack1 = findControl<Label>("LBL_BACK1");
    _binding.lblBack2 = findControl<Label>("LBL_BACK2");
    _binding.lblBack3 = findControl<Label>("LBL_BACK3");
    _binding.lblCash = findControl<Label>("LBL_CASH");
    _binding.lblChar1 = findControl<Label>("LBL_CHAR1");
    _binding.lblChar2 = findControl<Label>("LBL_CHAR2");
    _binding.lblChar3 = findControl<Label>("LBL_CHAR3");
    _binding.lblCmbtEfctInc1 = findControl<Label>("LBL_CMBTEFCTINC1");
    _binding.lblCmbtEfctInc2 = findControl<Label>("LBL_CMBTEFCTINC2");
    _binding.lblCmbtEfctInc3 = findControl<Label>("LBL_CMBTEFCTINC3");
    _binding.lblCmbtEfctRed1 = findControl<Label>("LBL_CMBTEFCTRED1");
    _binding.lblCmbtEfctRed2 = findControl<Label>("LBL_CMBTEFCTRED2");
    _binding.lblCmbtEfctRed3 = findControl<Label>("LBL_CMBTEFCTRED3");
    _binding.lblCmbtModeMsg = findControl<Label>("LBL_CMBTMODEMSG");
    _binding.lblCmbtMsgBg = findControl<Label>("LBL_CMBTMSGBG");
    _binding.lblCombatBg3 = findControl<Label>("LBL_COMBATBG3");
    _binding.lblDarkShift = findControl<Label>("LBL_DARKSHIFT");
    _binding.lblDebilatated1 = findControl<Label>("LBL_DEBILATATED1");
    _binding.lblDebilatated2 = findControl<Label>("LBL_DEBILATATED2");
    _binding.lblDebilatated3 = findControl<Label>("LBL_DEBILATATED3");
    _binding.lblDisable1 = findControl<Label>("LBL_DISABLE1");
    _binding.lblDisable2 = findControl<Label>("LBL_DISABLE2");
    _binding.lblDisable3 = findControl<Label>("LBL_DISABLE3");
    _binding.lblHealthBg = findControl<Label>("LBL_HEALTHBG");
    _binding.lblItemLost = findControl<Label>("LBL_ITEMLOST");
    _binding.lblItemRcvd = findControl<Label>("LBL_ITEMRCVD");
    _binding.lblJournal = findControl<Label>("LBL_JOURNAL");
    _binding.lblLevelUp1 = findControl<Label>("LBL_LEVELUP1");
    _binding.lblLevelUp2 = findControl<Label>("LBL_LEVELUP2");
    _binding.lblLevelUp3 = findControl<Label>("LBL_LEVELUP3");
    _binding.lblLightShift = findControl<Label>("LBL_LIGHTSHIFT");
    _binding.lblMap = findControl<Label>("LBL_MAP");
    _binding.lblMapBorder = findControl<Label>("LBL_MAPBORDER");
    _binding.lblMapView = findControl<Label>("LBL_MAPVIEW");
    _binding.lblMenuBg = findControl<Label>("LBL_MENUBG");
    _binding.lblMoulding1 = findControl<Label>("LBL_MOULDING1");
    _binding.lblMoulding3 = findControl<Label>("LBL_MOULDING3");
    _binding.lblName = findControl<Label>("LBL_NAME");
    _binding.lblNameBg = findControl<Label>("LBL_NAMEBG");
    _binding.lblPlotXp = findControl<Label>("LBL_PLOTXP");
    _binding.lblQueue0 = findControl<Label>("LBL_QUEUE0");
    _binding.lblQueue1 = findControl<Label>("LBL_QUEUE1");
    _binding.lblQueue2 = findControl<Label>("LBL_QUEUE2");
    _binding.lblQueue3 = findControl<Label>("LBL_QUEUE3");
    _binding.lblStealthXp = findControl<Label>("LBL_STEALTHXP");
    _binding.lblTarget0 = findControl<Label>("LBL_TARGET0");
    _binding.lblTarget1 = findControl<Label>("LBL_TARGET1");
    _binding.lblTarget2 = findControl<Label>("LBL_TARGET2");
    _binding.pbForce1 = findControl<ProgressBar>("PB_FORCE1");
    _binding.pbForce2 = findControl<ProgressBar>("PB_FORCE2");
    _binding.pbForce3 = findControl<ProgressBar>("PB_FORCE3");
    _binding.pbHealth = findControl<ProgressBar>("PB_HEALTH");
    _binding.pbVit1 = findControl<ProgressBar>("PB_VIT1");
    _binding.pbVit2 = findControl<ProgressBar>("PB_VIT2");
    _binding.pbVit3 = findControl<ProgressBar>("PB_VIT3");
    _binding.tbPause = findControl<ToggleButton>("TB_PAUSE");
    _binding.tbSolo = findControl<ToggleButton>("TB_SOLO");
    _binding.tbStealth = findControl<ToggleButton>("TB_STEALTH");

    if (_game.isTSL()) {
        _binding.btnAction4 = findControl<Button>("BTN_ACTION4");
        _binding.btnAction5 = findControl<Button>("BTN_ACTION5");
        _binding.btnActionDown4 = findControl<Button>("BTN_ACTIONDOWN4");
        _binding.btnActionDown5 = findControl<Button>("BTN_ACTIONDOWN5");
        _binding.btnActionUp4 = findControl<Button>("BTN_ACTIONUP4");
        _binding.btnActionUp5 = findControl<Button>("BTN_ACTIONUP5");
        _binding.btnSwapWeapons = findControl<Button>("BTN_SWAPWEAPONS");
        _binding.lblAction4 = findControl<Label>("LBL_ACTION4");
        _binding.lblAction5 = findControl<Label>("LBL_ACTION5");
    } else {
        _binding.lblCombatBg1 = findControl<Label>("LBL_COMBATBG1");
        _binding.lblCombatBg2 = findControl<Label>("LBL_COMBATBG2");
        _binding.lblLvlUpBg1 = findControl<Label>("LBL_LVLUPBG1");
        _binding.lblLvlUpBg2 = findControl<Label>("LBL_LVLUPBG2");
        _binding.lblLvlUpBg3 = findControl<Label>("LBL_LVLUPBG3");
        _binding.lblMoulding2 = findControl<Label>("LBL_MOULDING2");
    }
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
        _binding.lblChar1.get(),
        _binding.lblChar2.get(),
        _binding.lblChar3.get()};
    std::vector<Label *> backLabels {
        _binding.lblBack1.get(),
        _binding.lblBack2.get(),
        _binding.lblBack3.get()};
    std::vector<Label *> lvlUpBgLabels {
        _binding.lblLvlUpBg1.get(),
        _binding.lblLvlUpBg2.get(),
        _binding.lblLvlUpBg3.get()};
    std::vector<Label *> levevlUpLabels {
        _binding.lblLevelUp1.get(),
        _binding.lblLevelUp2.get(),
        _binding.lblLevelUp3.get()};

    for (int i = 0; i < 3; ++i) {
        Label &lblChar = *charLabels[i];
        Label &lblBack = *backLabels[i];
        Label &lblLevelUp = *levevlUpLabels[i];

        Label *lblLvlUpBg;
        if (!_game.isTSL()) {
            lblLvlUpBg = lvlUpBgLabels[i];
        }

        std::shared_ptr<Creature> member(party.getMember(i));
        if (member) {
            lblChar.setVisible(true);
            lblChar.setBorderFill(member->portrait());
            lblBack.setVisible(true);
            lblLevelUp.setVisible(member->isLevelUpPending());
            if (!_game.isTSL()) {
                lblLvlUpBg->setVisible(member->isLevelUpPending());
            }
        } else {
            lblChar.setVisible(false);
            lblBack.setVisible(false);
            lblLevelUp.setVisible(false);
            if (!_game.isTSL()) {
                lblLvlUpBg->setVisible(false);
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
    _binding.lblMapBorder->setVisible(_game.map().isLoaded());
}

void HUD::draw() {
    _gui->draw();

    drawMinimap();

    Party &party = _game.party();
    for (int i = 0; i < party.getSize(); ++i) {
        drawHealth(i);
    }

    _barkBubble->draw();
    _select.draw();
}

void HUD::drawMinimap() {
    const Control::Extent &extent = _binding.lblMapView->extent();

    glm::vec4 bounds;
    bounds[0] = static_cast<float>(_gui->controlOffset().x + extent.left);
    bounds[1] = static_cast<float>(_gui->controlOffset().y + extent.top);
    bounds[2] = static_cast<float>(extent.width);
    bounds[3] = static_cast<float>(extent.height);

    std::shared_ptr<Area> area(_game.module()->area());
    _game.map().draw(Map::Mode::Minimap, bounds);
}

void HUD::drawHealth(int memberIndex) {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> member(party.getMember(memberIndex));
    std::vector<Label *> backLabels {
        _binding.lblBack1.get(),
        _binding.lblBack2.get(),
        _binding.lblBack3.get()};

    Label &lblChar = *backLabels[memberIndex];
    const Control::Extent &extent = lblChar.extent();

    float w = 5.0f;
    float h = glm::clamp(member->currentHitPoints() / static_cast<float>(member->hitPoints()), 0.0f, 1.0f) * extent.height;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_gui->controlOffset().x + extent.left + extent.width - 14.0f, _gui->controlOffset().y + extent.top + extent.height - h, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    _services.graphics.uniforms.setGeneral([this, transform](auto &general) {
        general.resetLocals();
        general.projection = _services.graphics.window.getOrthoProjection();
        general.model = std::move(transform);
        general.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    });
    _services.graphics.shaders.use(ShaderProgramId::SimpleColor);
    _services.graphics.meshes.quad().draw();
}

void HUD::toggleCombat(bool enabled) {
    _binding.btnClearAll->setVisible(enabled);
    _binding.btnClearOne->setVisible(enabled);
    _binding.btnClearOne2->setVisible(enabled);
    _binding.lblCmbtModeMsg->setVisible(enabled);
    _binding.lblCmbtMsgBg->setVisible(enabled);
    _binding.lblQueue0->setVisible(enabled);
    _binding.lblQueue1->setVisible(enabled);
    _binding.lblQueue2->setVisible(enabled);
    _binding.lblQueue3->setVisible(enabled);

    if (!_game.isTSL()) {
        _binding.lblCombatBg1->setVisible(enabled);
        _binding.lblCombatBg2->setVisible(enabled);
        _binding.lblCombatBg3->setVisible(enabled);
    }
}

void HUD::refreshActionQueueItems() const {
    auto &actions = _game.party().getLeader()->actions();
    std::vector<Label *> queueLabels {
        _binding.lblQueue0.get(),
        _binding.lblQueue1.get(),
        _binding.lblQueue2.get(),
        _binding.lblQueue3.get()};

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
