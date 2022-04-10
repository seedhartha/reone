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

#include "hud.h"

#include "../../common/logutil.h"
#include "../../graphics/context.h"
#include "../../graphics/mesh.h"
#include "../../graphics/meshes.h"
#include "../../graphics/shaders.h"
#include "../../graphics/uniforms.h"
#include "../../graphics/window.h"
#include "../../gui/control/label.h"

#include "../action/usefeat.h"
#include "../d20/feat.h"
#include "../d20/feats.h"
#include "../kotor.h"
#include "../object/creature.h"
#include "../party.h"
#include "../services.h"

using namespace std;

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

static string g_attackIcon("i_attack");

HUD::HUD(KotOR &game, GameServices &services) :
    GameGUI(game, services),
    _select(game, services) {
    _resRef = getResRef("mipc28x6");
    _resolutionX = 800;
    _resolutionY = 600;
    _scaling = ScalingMode::PositionRelativeToCenter;

    static string combatControlTags[] = {
        "BTN_CLEARALL", "BTN_CLEARONE", "BTN_CLEARONE2",
        "LBL_CMBTMODEMSG", "LBL_CMBTMSGBG", "LBL_COMBATBG1", "LBL_COMBATBG2", "LBL_COMBATBG3",
        "LBL_QUEUE0", "LBL_QUEUE1", "LBL_QUEUE2", "LBL_QUEUE3"};
    for (auto &tag : combatControlTags) {
        _scalingByControlTag.insert(make_pair(tag, ScalingMode::Stretch));
    }
}

void HUD::load() {
    GUI::load();
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

    _select.load();

    _barkBubble = make_unique<BarkBubble>(_game, _services);
    _barkBubble->load();
}

void HUD::bindControls() {
    _binding.btnAbi = getControl<Button>("BTN_ABI");
    _binding.btnAction0 = getControl<Button>("BTN_ACTION0");
    _binding.btnAction1 = getControl<Button>("BTN_ACTION1");
    _binding.btnAction2 = getControl<Button>("BTN_ACTION2");
    _binding.btnAction3 = getControl<Button>("BTN_ACTION3");
    _binding.btnActionDown0 = getControl<Button>("BTN_ACTIONDOWN0");
    _binding.btnActionDown1 = getControl<Button>("BTN_ACTIONDOWN1");
    _binding.btnActionDown2 = getControl<Button>("BTN_ACTIONDOWN2");
    _binding.btnActionDown3 = getControl<Button>("BTN_ACTIONDOWN3");
    _binding.btnActionUp0 = getControl<Button>("BTN_ACTIONUP0");
    _binding.btnActionUp1 = getControl<Button>("BTN_ACTIONUP1");
    _binding.btnActionUp2 = getControl<Button>("BTN_ACTIONUP2");
    _binding.btnActionUp3 = getControl<Button>("BTN_ACTIONUP3");
    _binding.btnChar = getControl<Button>("BTN_CHAR");
    _binding.btnChar1 = getControl<Button>("BTN_CHAR1");
    _binding.btnChar2 = getControl<Button>("BTN_CHAR2");
    _binding.btnChar3 = getControl<Button>("BTN_CHAR3");
    _binding.btnClearAll = getControl<Button>("BTN_CLEARALL");
    _binding.btnClearOne = getControl<Button>("BTN_CLEARONE");
    _binding.btnClearOne2 = getControl<Button>("BTN_CLEARONE2");
    _binding.btnEqu = getControl<Button>("BTN_EQU");
    _binding.btnInv = getControl<Button>("BTN_INV");
    _binding.btnJou = getControl<Button>("BTN_JOU");
    _binding.btnMap = getControl<Button>("BTN_MAP");
    _binding.btnMinimap = getControl<Button>("BTN_MINIMAP");
    _binding.btnMsg = getControl<Button>("BTN_MSG");
    _binding.btnOpt = getControl<Button>("BTN_OPT");
    _binding.btnTarget0 = getControl<Button>("BTN_TARGET0");
    _binding.btnTarget1 = getControl<Button>("BTN_TARGET1");
    _binding.btnTarget2 = getControl<Button>("BTN_TARGET2");
    _binding.btnTargetDown0 = getControl<Button>("BTN_TARGETDOWN0");
    _binding.btnTargetDown1 = getControl<Button>("BTN_TARGETDOWN1");
    _binding.btnTargetDown2 = getControl<Button>("BTN_TARGETDOWN2");
    _binding.btnTargetUp0 = getControl<Button>("BTN_TARGETUP0");
    _binding.btnTargetUp1 = getControl<Button>("BTN_TARGETUP1");
    _binding.btnTargetUp2 = getControl<Button>("BTN_TARGETUP2");
    _binding.lblAction0 = getControl<Label>("LBL_ACTION0");
    _binding.lblAction1 = getControl<Label>("LBL_ACTION1");
    _binding.lblAction2 = getControl<Label>("LBL_ACTION2");
    _binding.lblAction3 = getControl<Label>("LBL_ACTION3");
    _binding.lblActionDesc = getControl<Label>("LBL_ACTIONDESC");
    _binding.lblActionDescBg = getControl<Label>("LBL_ACTIONDESCBG");
    _binding.lblArrow = getControl<Label>("LBL_ARROW");
    _binding.lblArrowMargin = getControl<Label>("LBL_ARROW_MARGIN");
    _binding.lblBack1 = getControl<Label>("LBL_BACK1");
    _binding.lblBack2 = getControl<Label>("LBL_BACK2");
    _binding.lblBack3 = getControl<Label>("LBL_BACK3");
    _binding.lblCash = getControl<Label>("LBL_CASH");
    _binding.lblChar1 = getControl<Label>("LBL_CHAR1");
    _binding.lblChar2 = getControl<Label>("LBL_CHAR2");
    _binding.lblChar3 = getControl<Label>("LBL_CHAR3");
    _binding.lblCmbtEfctInc1 = getControl<Label>("LBL_CMBTEFCTINC1");
    _binding.lblCmbtEfctInc2 = getControl<Label>("LBL_CMBTEFCTINC2");
    _binding.lblCmbtEfctInc3 = getControl<Label>("LBL_CMBTEFCTINC3");
    _binding.lblCmbtEfctRed1 = getControl<Label>("LBL_CMBTEFCTRED1");
    _binding.lblCmbtEfctRed2 = getControl<Label>("LBL_CMBTEFCTRED2");
    _binding.lblCmbtEfctRed3 = getControl<Label>("LBL_CMBTEFCTRED3");
    _binding.lblCmbtModeMsg = getControl<Label>("LBL_CMBTMODEMSG");
    _binding.lblCmbtMsgBg = getControl<Label>("LBL_CMBTMSGBG");
    _binding.lblCombatBg3 = getControl<Label>("LBL_COMBATBG3");
    _binding.lblDarkShift = getControl<Label>("LBL_DARKSHIFT");
    _binding.lblDebilatated1 = getControl<Label>("LBL_DEBILATATED1");
    _binding.lblDebilatated2 = getControl<Label>("LBL_DEBILATATED2");
    _binding.lblDebilatated3 = getControl<Label>("LBL_DEBILATATED3");
    _binding.lblDisable1 = getControl<Label>("LBL_DISABLE1");
    _binding.lblDisable2 = getControl<Label>("LBL_DISABLE2");
    _binding.lblDisable3 = getControl<Label>("LBL_DISABLE3");
    _binding.lblHealthBg = getControl<Label>("LBL_HEALTHBG");
    _binding.lblItemLost = getControl<Label>("LBL_ITEMLOST");
    _binding.lblItemRcvd = getControl<Label>("LBL_ITEMRCVD");
    _binding.lblJournal = getControl<Label>("LBL_JOURNAL");
    _binding.lblLevelUp1 = getControl<Label>("LBL_LEVELUP1");
    _binding.lblLevelUp2 = getControl<Label>("LBL_LEVELUP2");
    _binding.lblLevelUp3 = getControl<Label>("LBL_LEVELUP3");
    _binding.lblLightShift = getControl<Label>("LBL_LIGHTSHIFT");
    _binding.lblMap = getControl<Label>("LBL_MAP");
    _binding.lblMapBorder = getControl<Label>("LBL_MAPBORDER");
    _binding.lblMapView = getControl<Label>("LBL_MAPVIEW");
    _binding.lblMenuBg = getControl<Label>("LBL_MENUBG");
    _binding.lblMoulding1 = getControl<Label>("LBL_MOULDING1");
    _binding.lblMoulding3 = getControl<Label>("LBL_MOULDING3");
    _binding.lblName = getControl<Label>("LBL_NAME");
    _binding.lblNameBg = getControl<Label>("LBL_NAMEBG");
    _binding.lblPlotXp = getControl<Label>("LBL_PLOTXP");
    _binding.lblQueue0 = getControl<Label>("LBL_QUEUE0");
    _binding.lblQueue1 = getControl<Label>("LBL_QUEUE1");
    _binding.lblQueue2 = getControl<Label>("LBL_QUEUE2");
    _binding.lblQueue3 = getControl<Label>("LBL_QUEUE3");
    _binding.lblStealthXp = getControl<Label>("LBL_STEALTHXP");
    _binding.lblTarget0 = getControl<Label>("LBL_TARGET0");
    _binding.lblTarget1 = getControl<Label>("LBL_TARGET1");
    _binding.lblTarget2 = getControl<Label>("LBL_TARGET2");
    _binding.pbForce1 = getControl<ProgressBar>("PB_FORCE1");
    _binding.pbForce2 = getControl<ProgressBar>("PB_FORCE2");
    _binding.pbForce3 = getControl<ProgressBar>("PB_FORCE3");
    _binding.pbHealth = getControl<ProgressBar>("PB_HEALTH");
    _binding.pbVit1 = getControl<ProgressBar>("PB_VIT1");
    _binding.pbVit2 = getControl<ProgressBar>("PB_VIT2");
    _binding.pbVit3 = getControl<ProgressBar>("PB_VIT3");
    _binding.tbPause = getControl<ToggleButton>("TB_PAUSE");
    _binding.tbSolo = getControl<ToggleButton>("TB_SOLO");
    _binding.tbStealth = getControl<ToggleButton>("TB_STEALTH");

    if (_game.isTSL()) {
        _binding.btnAction4 = getControl<Button>("BTN_ACTION4");
        _binding.btnAction5 = getControl<Button>("BTN_ACTION5");
        _binding.btnActionDown4 = getControl<Button>("BTN_ACTIONDOWN4");
        _binding.btnActionDown5 = getControl<Button>("BTN_ACTIONDOWN5");
        _binding.btnActionUp4 = getControl<Button>("BTN_ACTIONUP4");
        _binding.btnActionUp5 = getControl<Button>("BTN_ACTIONUP5");
        _binding.btnSwapWeapons = getControl<Button>("BTN_SWAPWEAPONS");
        _binding.lblAction4 = getControl<Label>("LBL_ACTION4");
        _binding.lblAction5 = getControl<Label>("LBL_ACTION5");
    } else {
        _binding.lblCombatBg1 = getControl<Label>("LBL_COMBATBG1");
        _binding.lblCombatBg2 = getControl<Label>("LBL_COMBATBG2");
        _binding.lblLvlUpBg1 = getControl<Label>("LBL_LVLUPBG1");
        _binding.lblLvlUpBg2 = getControl<Label>("LBL_LVLUPBG2");
        _binding.lblLvlUpBg3 = getControl<Label>("LBL_LVLUPBG3");
        _binding.lblMoulding2 = getControl<Label>("LBL_MOULDING2");
    }
}

bool HUD::handle(const SDL_Event &event) {
    if (_select.handle(event))
        return true;

    return GUI::handle(event);
}

void HUD::update(float dt) {
    GUI::update(dt);

    Party &party = _game.party();
    vector<Label *> charLabels {
        _binding.lblChar1.get(),
        _binding.lblChar2.get(),
        _binding.lblChar3.get()};
    vector<Label *> backLabels {
        _binding.lblBack1.get(),
        _binding.lblBack2.get(),
        _binding.lblBack3.get()};
    vector<Label *> lvlUpBgLabels {
        _binding.lblLvlUpBg1.get(),
        _binding.lblLvlUpBg2.get(),
        _binding.lblLvlUpBg3.get()};
    vector<Label *> levevlUpLabels {
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

        shared_ptr<Creature> member(party.getMember(i));
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
    GUI::draw();

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
    bounds[0] = static_cast<float>(_controlOffset.x + extent.left);
    bounds[1] = static_cast<float>(_controlOffset.y + extent.top);
    bounds[2] = static_cast<float>(extent.width);
    bounds[3] = static_cast<float>(extent.height);

    shared_ptr<Area> area(_game.module()->area());
    _game.map().draw(Map::Mode::Minimap, bounds);
}

void HUD::drawHealth(int memberIndex) {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    shared_ptr<Creature> member(party.getMember(memberIndex));
    vector<Label *> backLabels {
        _binding.lblBack1.get(),
        _binding.lblBack2.get(),
        _binding.lblBack3.get()};

    Label &lblChar = *backLabels[memberIndex];
    const Control::Extent &extent = lblChar.extent();

    float w = 5.0f;
    float h = glm::clamp(member->currentHitPoints() / static_cast<float>(member->hitPoints()), 0.0f, 1.0f) * extent.height;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_controlOffset.x + extent.left + extent.width - 14.0f, _controlOffset.y + extent.top + extent.height - h, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    _uniforms.setGeneral([this, transform](auto &general) {
        general.resetLocals();
        general.projection = _window.getOrthoProjection();
        general.model = move(transform);
        general.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    });
    _shaders.use(_shaders.simpleColor());
    _meshes.quad().draw();
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
    vector<Label *> queueLabels {
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
                auto featAction = static_pointer_cast<UseFeatAction>(actions[i]);
                shared_ptr<Feat> feat(_services.feats.get(featAction->feat()));
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
