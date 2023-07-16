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

#include "reone/game/gui/ingame/character.h"

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/ingame.h"
#include "reone/game/party.h"
#include "reone/game/types.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/models.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

void CharacterMenu::onGUILoaded() {
    loadBackground(BackgroundType::Menu);
    bindControls();

    if (_controls.LBL_GOOD1)
        _lblGood.push_back(_controls.LBL_GOOD1);
    if (_controls.LBL_GOOD2)
        _lblGood.push_back(_controls.LBL_GOOD2);
    if (_controls.LBL_GOOD3)
        _lblGood.push_back(_controls.LBL_GOOD3);
    if (_controls.LBL_GOOD4)
        _lblGood.push_back(_controls.LBL_GOOD4);
    if (_controls.LBL_GOOD5)
        _lblGood.push_back(_controls.LBL_GOOD5);
    if (_controls.LBL_GOOD6)
        _lblGood.push_back(_controls.LBL_GOOD6);
    if (_controls.LBL_GOOD7)
        _lblGood.push_back(_controls.LBL_GOOD7);
    if (_controls.LBL_GOOD8)
        _lblGood.push_back(_controls.LBL_GOOD8);
    if (_controls.LBL_GOOD9)
        _lblGood.push_back(_controls.LBL_GOOD9);
    if (_controls.LBL_GOOD10)
        _lblGood.push_back(_controls.LBL_GOOD10);

    if (_controls.LBL_BAR1)
        _lblBar.push_back(_controls.LBL_BAR1);
    if (_controls.LBL_BAR2)
        _lblBar.push_back(_controls.LBL_BAR2);
    if (_controls.LBL_BAR3)
        _lblBar.push_back(_controls.LBL_BAR3);
    if (_controls.LBL_BAR4)
        _lblBar.push_back(_controls.LBL_BAR4);
    if (_controls.LBL_BAR5)
        _lblBar.push_back(_controls.LBL_BAR5);
    if (_controls.LBL_BAR6)
        _lblBar.push_back(_controls.LBL_BAR6);

    if (_game.isTSL()) {
        _controls.BTN_CHANGE1 = _inGameMenu.getBtnChange2();
        _controls.BTN_CHANGE2 = _inGameMenu.getBtnChange3();
    }

    _controls.BTN_AUTO->setDisabled(true);
    _controls.BTN_EXIT->setOnClick([this]() {
        _game.openInGame();
    });
    _controls.BTN_LEVELUP->setOnClick([this]() {
        _game.openLevelUp();
    });

    if (!_game.isTSL()) {
        // _controls.btnCharLeft->setVisible(false);
        // _controls.btnCharRight->setVisible(false);

        for (auto &control : _lblGood) {
            control->setVisible(false);
        }

        _controls.LBL_MORE->setVisible(false);
        _controls.BTN_SCRIPTS->setDisabled(true);
    }
}

void CharacterMenu::update(float dt) {
    std::shared_ptr<Creature> leader(_game.party().getLeader());
    _controls.BTN_LEVELUP->setVisible(leader->isLevelUpPending());
    _controls.BTN_AUTO->setVisible(leader->isLevelUpPending());
    GameGUI::update(dt);
}

static std::string toStringOrEmptyIfZero(int value) {
    return value != 0 ? std::to_string(value) : "";
}

static std::string describeAbilityModifier(int value) {
    return value > 0 ? "+" + std::to_string(value) : std::to_string(value);
}

void CharacterMenu::refreshControls() {
    std::shared_ptr<Creature> partyLeader(_game.party().getLeader());
    CreatureAttributes &attributes = partyLeader->attributes();

    if (!_game.isTSL()) {
        _controls.LBL_CLASS1->setTextMessage(describeClass(attributes.getClassByPosition(1)));
        _controls.LBL_CLASS2->setTextMessage(describeClass(attributes.getClassByPosition(2)));
        _controls.LBL_LEVEL1->setTextMessage(toStringOrEmptyIfZero(attributes.getLevelByPosition(1)));
        _controls.LBL_LEVEL2->setTextMessage(toStringOrEmptyIfZero(attributes.getLevelByPosition(2)));
    }

    _controls.LBL_VITALITY_STAT->setTextMessage(str(boost::format("%d/%d") % partyLeader->currentHitPoints() % partyLeader->hitPoints()));
    _controls.LBL_DEFENSE_STAT->setTextMessage(std::to_string(attributes.getDefense()));
    _controls.LBL_FORCE_STAT->setTextMessage("");

    _controls.LBL_STR->setTextMessage(std::to_string(attributes.strength()));
    _controls.LBL_STR_MOD->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Strength)));
    _controls.LBL_DEX->setTextMessage(std::to_string(attributes.dexterity()));
    _controls.LBL_DEX_MOD->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Dexterity)));
    _controls.LBL_CON->setTextMessage(std::to_string(attributes.constitution()));
    _controls.LBL_CON_MOD->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Constitution)));
    _controls.LBL_INT->setTextMessage(std::to_string(attributes.intelligence()));
    _controls.LBL_INT_MOD->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Intelligence)));
    _controls.LBL_WIS->setTextMessage(std::to_string(attributes.wisdom()));
    _controls.LBL_WIS_MOD->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Wisdom)));
    _controls.LBL_CHA->setTextMessage(std::to_string(attributes.charisma()));
    _controls.LBL_CHA_MOD->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Charisma)));

    SavingThrows savingThrows(attributes.getAggregateSavingThrows());
    _controls.LBL_FORTITUDE_STAT->setTextMessage(std::to_string(savingThrows.fortitude));
    _controls.LBL_REFLEX_STAT->setTextMessage(std::to_string(savingThrows.reflex));
    _controls.LBL_WILL_STAT->setTextMessage(std::to_string(savingThrows.will));

    _controls.LBL_EXPERIENCE_STAT->setTextMessage(std::to_string(partyLeader->xp()));
    _controls.LBL_NEEDED_XP->setTextMessage(std::to_string(partyLeader->getNeededXP()));

    refreshPortraits();
    refresh3D();
}

std::string CharacterMenu::describeClass(ClassType clazz) const {
    if (clazz == ClassType::Invalid)
        return "";

    return _services.game.classes.get(clazz)->name();
}

void CharacterMenu::refreshPortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    std::shared_ptr<Creature> partyMember1(party.getMember(1));
    std::shared_ptr<Creature> partyMember2(party.getMember(2));

    _controls.BTN_CHANGE1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _controls.BTN_CHANGE1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _controls.BTN_CHANGE2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _controls.BTN_CHANGE2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::refresh3D() {
    auto &sceneGraph = _services.scene.graphs.get(kSceneCharacter);
    float aspect = _controls.LBL_3DCHAR->extent().width / static_cast<float>(_controls.LBL_3DCHAR->extent().height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(kDefaultClipPlaneNear, 10.0f)
        .modelSupplier(bind(&CharacterMenu::getSceneModel, this, std::placeholders::_1))
        .modelOffset(glm::vec2(0.0f, 1.7f))
        .cameraFromModelNode("camerahook")
        .invoke();

    _controls.LBL_3DCHAR->setSceneName(kSceneCharacter);
}

std::shared_ptr<ModelSceneNode> CharacterMenu::getSceneModel(ISceneGraph &sceneGraph) const {
    auto partyLeader = _game.party().getLeader();

    std::shared_ptr<Creature> character = _game.newCreature(sceneGraph.name());
    character->setFacing(-glm::half_pi<float>());
    character->setAppearance(partyLeader->appearance());

    for (auto &item : partyLeader->equipment()) {
        switch (item.first) {
        case InventorySlot::body:
            character->equip(item.first, item.second);
            break;
        default:
            break;
        }
    }

    character->loadAppearance();
    character->updateModelAnimation();

    auto sceneModel = sceneGraph.newModel(*_services.graphics.models.get("charmain_light"), ModelUsage::GUI);
    sceneModel->attach("charmain_light", *character->sceneNode());

    return sceneModel;
}

} // namespace game

} // namespace reone
