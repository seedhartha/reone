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

#include "../../common/log.h"
#include "../../gui/control/label.h"
#include "../../graphics/mesh/mesh.h"
#include "../../graphics/mesh/meshes.h"
#include "../../graphics/window.h"

#include "../action/usefeat.h"
#include "../game.h"
#include "../party.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

static string g_attackIcon("i_attack");

HUD::HUD(Game *game) : GameGUI(game), _select(game) {
    _resRef = getResRef("mipc28x6");
    _resolutionX = 800;
    _resolutionY = 600;
    _scaling = ScalingMode::PositionRelativeToCenter;

    static string combatControlTags[] = {
        "BTN_CLEARALL", "BTN_CLEARONE", "BTN_CLEARONE2",
        "LBL_CMBTMODEMSG", "LBL_CMBTMSGBG", "LBL_COMBATBG1", "LBL_COMBATBG2", "LBL_COMBATBG3",
        "LBL_QUEUE0", "LBL_QUEUE1", "LBL_QUEUE2", "LBL_QUEUE3"
    };
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

    if (_game->isTSL()) {
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

    _select.load();

    _barkBubble = make_unique<BarkBubble>(_game);
    _barkBubble->load();
}

void HUD::bindControls() {
    _binding.btnAbi = getControlPtr<Button>("BTN_ABI");
    _binding.btnAction0 = getControlPtr<Button>("BTN_ACTION0");
    _binding.btnAction1 = getControlPtr<Button>("BTN_ACTION1");
    _binding.btnAction2 = getControlPtr<Button>("BTN_ACTION2");
    _binding.btnAction3 = getControlPtr<Button>("BTN_ACTION3");
    _binding.btnActionDown0 = getControlPtr<Button>("BTN_ACTIONDOWN0");
    _binding.btnActionDown1 = getControlPtr<Button>("BTN_ACTIONDOWN1");
    _binding.btnActionDown2 = getControlPtr<Button>("BTN_ACTIONDOWN2");
    _binding.btnActionDown3 = getControlPtr<Button>("BTN_ACTIONDOWN3");
    _binding.btnActionUp0 = getControlPtr<Button>("BTN_ACTIONUP0");
    _binding.btnActionUp1 = getControlPtr<Button>("BTN_ACTIONUP1");
    _binding.btnActionUp2 = getControlPtr<Button>("BTN_ACTIONUP2");
    _binding.btnActionUp3 = getControlPtr<Button>("BTN_ACTIONUP3");
    _binding.btnChar = getControlPtr<Button>("BTN_CHAR");
    _binding.btnChar1 = getControlPtr<Button>("BTN_CHAR1");
    _binding.btnChar2 = getControlPtr<Button>("BTN_CHAR2");
    _binding.btnChar3 = getControlPtr<Button>("BTN_CHAR3");
    _binding.btnClearAll = getControlPtr<Button>("BTN_CLEARALL");
    _binding.btnClearOne = getControlPtr<Button>("BTN_CLEARONE");
    _binding.btnClearOne2 = getControlPtr<Button>("BTN_CLEARONE2");
    _binding.btnEqu = getControlPtr<Button>("BTN_EQU");
    _binding.btnInv = getControlPtr<Button>("BTN_INV");
    _binding.btnJou = getControlPtr<Button>("BTN_JOU");
    _binding.btnMap = getControlPtr<Button>("BTN_MAP");
    _binding.btnMinimap = getControlPtr<Button>("BTN_MINIMAP");
    _binding.btnMsg = getControlPtr<Button>("BTN_MSG");
    _binding.btnOpt = getControlPtr<Button>("BTN_OPT");
    _binding.btnTarget0 = getControlPtr<Button>("BTN_TARGET0");
    _binding.btnTarget1 = getControlPtr<Button>("BTN_TARGET1");
    _binding.btnTarget2 = getControlPtr<Button>("BTN_TARGET2");
    _binding.btnTargetDown0 = getControlPtr<Button>("BTN_TARGETDOWN0");
    _binding.btnTargetDown1 = getControlPtr<Button>("BTN_TARGETDOWN1");
    _binding.btnTargetDown2 = getControlPtr<Button>("BTN_TARGETDOWN2");
    _binding.btnTargetUp0 = getControlPtr<Button>("BTN_TARGETUP0");
    _binding.btnTargetUp1 = getControlPtr<Button>("BTN_TARGETUP1");
    _binding.btnTargetUp2 = getControlPtr<Button>("BTN_TARGETUP2");
    _binding.lblAction0 = getControlPtr<Label>("LBL_ACTION0");
    _binding.lblAction1 = getControlPtr<Label>("LBL_ACTION1");
    _binding.lblAction2 = getControlPtr<Label>("LBL_ACTION2");
    _binding.lblAction3 = getControlPtr<Label>("LBL_ACTION3");
    _binding.lblActionDesc = getControlPtr<Label>("LBL_ACTIONDESC");
    _binding.lblActionDescBg = getControlPtr<Label>("LBL_ACTIONDESCBG");
    _binding.lblArrow = getControlPtr<Label>("LBL_ARROW");
    _binding.lblArrowMargin = getControlPtr<Label>("LBL_ARROW_MARGIN");
    _binding.lblBack1 = getControlPtr<Label>("LBL_BACK1");
    _binding.lblBack2 = getControlPtr<Label>("LBL_BACK2");
    _binding.lblBack3 = getControlPtr<Label>("LBL_BACK3");
    _binding.lblCash = getControlPtr<Label>("LBL_CASH");
    _binding.lblChar1 = getControlPtr<Label>("LBL_CHAR1");
    _binding.lblChar2 = getControlPtr<Label>("LBL_CHAR2");
    _binding.lblChar3 = getControlPtr<Label>("LBL_CHAR3");
    _binding.lblCmbtEfctInc1 = getControlPtr<Label>("LBL_CMBTEFCTINC1");
    _binding.lblCmbtEfctInc2 = getControlPtr<Label>("LBL_CMBTEFCTINC2");
    _binding.lblCmbtEfctInc3 = getControlPtr<Label>("LBL_CMBTEFCTINC3");
    _binding.lblCmbtEfctRed1 = getControlPtr<Label>("LBL_CMBTEFCTRED1");
    _binding.lblCmbtEfctRed2 = getControlPtr<Label>("LBL_CMBTEFCTRED2");
    _binding.lblCmbtEfctRed3 = getControlPtr<Label>("LBL_CMBTEFCTRED3");
    _binding.lblCmbtModeMsg = getControlPtr<Label>("LBL_CMBTMODEMSG");
    _binding.lblCmbtMsgBg = getControlPtr<Label>("LBL_CMBTMSGBG");
    _binding.lblCombatBg3 = getControlPtr<Label>("LBL_COMBATBG3");
    _binding.lblDarkShift = getControlPtr<Label>("LBL_DARKSHIFT");
    _binding.lblDebilatated1 = getControlPtr<Label>("LBL_DEBILATATED1");
    _binding.lblDebilatated2 = getControlPtr<Label>("LBL_DEBILATATED2");
    _binding.lblDebilatated3 = getControlPtr<Label>("LBL_DEBILATATED3");
    _binding.lblDisable1 = getControlPtr<Label>("LBL_DISABLE1");
    _binding.lblDisable2 = getControlPtr<Label>("LBL_DISABLE2");
    _binding.lblDisable3 = getControlPtr<Label>("LBL_DISABLE3");
    _binding.lblHealthBg = getControlPtr<Label>("LBL_HEALTHBG");
    _binding.lblItemLost = getControlPtr<Label>("LBL_ITEMLOST");
    _binding.lblItemRcvd = getControlPtr<Label>("LBL_ITEMRCVD");
    _binding.lblJournal = getControlPtr<Label>("LBL_JOURNAL");
    _binding.lblLevelUp1 = getControlPtr<Label>("LBL_LEVELUP1");
    _binding.lblLevelUp2 = getControlPtr<Label>("LBL_LEVELUP2");
    _binding.lblLevelUp3 = getControlPtr<Label>("LBL_LEVELUP3");
    _binding.lblLightShift = getControlPtr<Label>("LBL_LIGHTSHIFT");
    _binding.lblMap = getControlPtr<Label>("LBL_MAP");
    _binding.lblMapBorder = getControlPtr<Label>("LBL_MAPBORDER");
    _binding.lblMapView = getControlPtr<Label>("LBL_MAPVIEW");
    _binding.lblMenuBg = getControlPtr<Label>("LBL_MENUBG");
    _binding.lblMoulding1 = getControlPtr<Label>("LBL_MOULDING1");
    _binding.lblMoulding3 = getControlPtr<Label>("LBL_MOULDING3");
    _binding.lblName = getControlPtr<Label>("LBL_NAME");
    _binding.lblNameBg = getControlPtr<Label>("LBL_NAMEBG");
    _binding.lblPlotXp = getControlPtr<Label>("LBL_PLOTXP");
    _binding.lblQueue0 = getControlPtr<Label>("LBL_QUEUE0");
    _binding.lblQueue1 = getControlPtr<Label>("LBL_QUEUE1");
    _binding.lblQueue2 = getControlPtr<Label>("LBL_QUEUE2");
    _binding.lblQueue3 = getControlPtr<Label>("LBL_QUEUE3");
    _binding.lblStealthXp = getControlPtr<Label>("LBL_STEALTHXP");
    _binding.lblTarget0 = getControlPtr<Label>("LBL_TARGET0");
    _binding.lblTarget1 = getControlPtr<Label>("LBL_TARGET1");
    _binding.lblTarget2 = getControlPtr<Label>("LBL_TARGET2");
    _binding.pbForce1 = getControlPtr<ProgressBar>("PB_FORCE1");
    _binding.pbForce2 = getControlPtr<ProgressBar>("PB_FORCE2");
    _binding.pbForce3 = getControlPtr<ProgressBar>("PB_FORCE3");
    _binding.pbHealth = getControlPtr<ProgressBar>("PB_HEALTH");
    _binding.pbVit1 = getControlPtr<ProgressBar>("PB_VIT1");
    _binding.pbVit2 = getControlPtr<ProgressBar>("PB_VIT2");
    _binding.pbVit3 = getControlPtr<ProgressBar>("PB_VIT3");
    _binding.tbPause = getControlPtr<ToggleButton>("TB_PAUSE");
    _binding.tbSolo = getControlPtr<ToggleButton>("TB_SOLO");
    _binding.tbStealth = getControlPtr<ToggleButton>("TB_STEALTH");

    if (_game->isTSL()) {
        _binding.btnAction4 = getControlPtr<Button>("BTN_ACTION4");
        _binding.btnAction5 = getControlPtr<Button>("BTN_ACTION5");
        _binding.btnActionDown4 = getControlPtr<Button>("BTN_ACTIONDOWN4");
        _binding.btnActionDown5 = getControlPtr<Button>("BTN_ACTIONDOWN5");
        _binding.btnActionUp4 = getControlPtr<Button>("BTN_ACTIONUP4");
        _binding.btnActionUp5 = getControlPtr<Button>("BTN_ACTIONUP5");
        _binding.btnSwapWeapons = getControlPtr<Button>("BTN_SWAPWEAPONS");
        _binding.lblAction4 = getControlPtr<Label>("LBL_ACTION4");
        _binding.lblAction5 = getControlPtr<Label>("LBL_ACTION5");
    } else {
        _binding.lblCombatBg1 = getControlPtr<Label>("LBL_COMBATBG1");
        _binding.lblCombatBg2 = getControlPtr<Label>("LBL_COMBATBG2");
        _binding.lblLvlUpBg1 = getControlPtr<Label>("LBL_LVLUPBG1");
        _binding.lblLvlUpBg2 = getControlPtr<Label>("LBL_LVLUPBG2");
        _binding.lblLvlUpBg3 = getControlPtr<Label>("LBL_LVLUPBG3");
        _binding.lblMoulding2 = getControlPtr<Label>("LBL_MOULDING2");
    }
}

bool HUD::handle(const SDL_Event &event) {
    if (_select.handle(event)) return true;

    return GUI::handle(event);
}

void HUD::update(float dt) {
    GUI::update(dt);

    Party &party = _game->services().party();

    for (int i = 0; i < 3; ++i) {
        int charIdx = (i == 0) ? 1 : (4 - i);
        Control &lblChar = getControl("LBL_CHAR" + to_string(charIdx));
        Control &lblBack = getControl("LBL_BACK" + to_string(charIdx));

        shared_ptr<Creature> member(party.getMember(i));
        if (member) {
            lblChar.setVisible(true);
            lblChar.setBorderFill(member->portrait());
            lblBack.setVisible(true);
            setControlVisible("LBL_LVLUPBG" + to_string(charIdx), member->isLevelUpPending());
            setControlVisible("LBL_LEVELUP" + to_string(charIdx), member->isLevelUpPending());
        } else {
            lblChar.setVisible(false);
            lblBack.setVisible(false);
            hideControl("LBL_LVLUPBG" + to_string(charIdx));
            hideControl("LBL_LEVELUP" + to_string(charIdx));
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
    _binding.lblMapBorder->setVisible(_game->module()->area()->map().isLoaded());
}

void HUD::draw() {
    GUI::draw();

    drawMinimap();

    Party &party = _game->services().party();
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

    shared_ptr<Area> area(_game->module()->area());
    area->map().draw(Map::Mode::Minimap, bounds);
}

void HUD::drawHealth(int memberIndex) {
    if (_game->isTSL()) return;

    Party &party = _game->services().party();
    shared_ptr<Creature> member(party.getMember(memberIndex));

    int charIdx = (memberIndex == 0) ? 1 : (4 - memberIndex);
    Control &lblChar = getControl("LBL_BACK" + to_string(charIdx));
    const Control::Extent &extent = lblChar.extent();

    float w = 5.0f;
    float h = glm::clamp(member->currentHitPoints() / static_cast<float>(member->hitPoints()), 0.0f, 1.0f) * extent.height;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(_controlOffset.x + extent.left + extent.width - 14.0f, _controlOffset.y + extent.top + extent.height - h, 0.0f));
    transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

    ShaderUniforms uniforms;
    uniforms.combined.general.projection = _game->services().graphics().window().getOrthoProjection();
    uniforms.combined.general.model = move(transform);
    uniforms.combined.general.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

    _game->services().graphics().shaders().activate(ShaderProgram::SimpleColor, uniforms);
    _game->services().graphics().meshes().quad().draw();
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

    if (_game->isKotOR()) {
        _binding.lblCombatBg1->setVisible(enabled);
        _binding.lblCombatBg2->setVisible(enabled);
        _binding.lblCombatBg3->setVisible(enabled);
    }
}

void HUD::refreshActionQueueItems() const {
    auto &actions = _game->services().party().getLeader()->actions();

    for (int i = 0; i < 4; ++i) {
        Control &item = getControl("LBL_QUEUE" + to_string(i));
        if (i < static_cast<int>(actions.size())) {
            switch (actions[i]->type()) {
                case ActionType::AttackObject:
                    item.setBorderFill(g_attackIcon);
                    break;
                case ActionType::UseFeat: {
                    auto featAction = static_pointer_cast<UseFeatAction>(actions[i]);
                    shared_ptr<Feat> feat(_game->services().feats().get(featAction->feat()));
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

void HUD::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EQU") {
        _game->openInGameMenu(InGameMenu::Tab::Equipment);
    } else if (control == "BTN_INV") {
        _game->openInGameMenu(InGameMenu::Tab::Inventory);
    } else if (control == "BTN_CHAR") {
        _game->openInGameMenu(InGameMenu::Tab::Character);
    } else if (control == "BTN_ABI") {
        _game->openInGameMenu(InGameMenu::Tab::Abilities);
    } else if (control == "BTN_MSG") {
        _game->openInGameMenu(InGameMenu::Tab::Messages);
    } else if (control == "BTN_JOU") {
        _game->openInGameMenu(InGameMenu::Tab::Journal);
    } else if (control == "BTN_MAP") {
        _game->openInGameMenu(InGameMenu::Tab::Map);
    } else if (control == "BTN_OPT") {
        _game->openInGameMenu(InGameMenu::Tab::Options);
    } else if (control == "BTN_CLEARALL") {
        _game->services().party().getLeader()->clearAllActions();
    } else if (control == "BTN_CLEARONE" || control == "BTN_CLEARONE2") {
        for (auto &action : _game->services().party().getLeader()->actions()) {
            if (action->type() == ActionType::AttackObject) {
                action->complete();
                break;
            }
        }
    } else if (control == "BTN_CHAR1") {
        _game->openInGameMenu(InGameMenu::Tab::Equipment);
    } else if (control == "BTN_CHAR2" || control == "BTN_CHAR3") {
        int memberIdx = 4 - stoi(&control[8]);
        _game->services().party().setPartyLeaderByIndex(memberIdx);
    }
}

} // namespace game

} // namespace reone
