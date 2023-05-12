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

#include "reone/graphics/models.h"
#include "reone/graphics/services.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/scene/graphs.h"
#include "reone/scene/services.h"

#include "reone/game/d20/classes.h"
#include "reone/game/game.h"
#include "reone/game/object/factory.h"
#include "reone/game/party.h"
#include "reone/game/services.h"
#include "reone/game/types.h"

#include "reone/game/gui/ingame.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;

using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

CharacterMenu::CharacterMenu(
    Game &game,
    InGameMenu &inGameMenu,
    ServicesView &services) :
    GameGUI(game, services),
    _inGameMenu(inGameMenu) {
    _resRef = getResRef("character");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void CharacterMenu::load() {
    GUI::load();
    bindControls();

    _binding.btnAuto->setDisabled(true);
    _binding.btnExit->setOnClick([this]() {
        _game.openInGame();
    });
    _binding.btnLevelup->setOnClick([this]() {
        _game.openLevelUp();
    });

    if (!_game.isTSL()) {
        _binding.btnCharLeft->setVisible(false);
        _binding.btnCharRight->setVisible(false);

        for (auto &control : _binding.lblGood) {
            control->setVisible(false);
        }

        _binding.lblMore->setVisible(false);
        _binding.btnScripts->setDisabled(true);
    }
}

void CharacterMenu::bindControls() {
    _binding.lbl3dChar = getControl<Label>("LBL_3DCHAR");
    _binding.btn3dChar = getControl<Button>("BTN_3DCHAR");
    _binding.sldAlign = getControl<Slider>("SLD_ALIGN");
    _binding.lblStr = getControl<Label>("LBL_STR");
    _binding.lblFortitudeStat = getControl<Label>("LBL_FORTITUDE_STAT");
    _binding.lblReflexStat = getControl<Label>("LBL_REFLEX_STAT");
    _binding.lblWillStat = getControl<Label>("LBL_WILL_STAT");
    _binding.lblDefenseStat = getControl<Label>("LBL_DEFENSE_STAT");
    _binding.lblForceStat = getControl<Label>("LBL_FORCE_STAT");
    _binding.lblVitalityStat = getControl<Label>("LBL_VITALITY_STAT");
    _binding.lblDex = getControl<Label>("LBL_DEX");
    _binding.lblCon = getControl<Label>("LBL_CON");
    _binding.lblInt = getControl<Label>("LBL_INT");
    _binding.lblCha = getControl<Label>("LBL_CHA");
    _binding.lblWis = getControl<Label>("LBL_WIS");
    _binding.lblStrMod = getControl<Label>("LBL_STR_MOD");
    _binding.lblDexMod = getControl<Label>("LBL_DEX_MOD");
    _binding.lblConMod = getControl<Label>("LBL_CON_MOD");
    _binding.lblIntMod = getControl<Label>("LBL_INT_MOD");
    _binding.lblWisMod = getControl<Label>("LBL_WIS_MOD");
    _binding.lblChaMod = getControl<Label>("LBL_CHA_MOD");
    _binding.lblExperienceStat = getControl<Label>("LBL_EXPERIENCE_STAT");
    _binding.lblNeededXp = getControl<Label>("LBL_NEEDED_XP");
    _binding.lblStrength = getControl<Label>("LBL_STRENGTH");
    _binding.lblDexterity = getControl<Label>("LBL_DEXTERITY");
    _binding.lblConstitution = getControl<Label>("LBL_CONSTITUTION");
    _binding.lblIntelligence = getControl<Label>("LBL_INTELLIGENCE");
    _binding.lblCharisma = getControl<Label>("LBL_CHARISMA");
    _binding.lblReflex = getControl<Label>("LBL_REFLEX");
    _binding.lblWill = getControl<Label>("LBL_WILL");
    _binding.lblExperience = getControl<Label>("LBL_EXPERIENCE");
    _binding.lblNextLevel = getControl<Label>("LBL_NEXT_LEVEL");
    _binding.lblForce = getControl<Label>("LBL_FORCE");
    _binding.lblVitality = getControl<Label>("LBL_VITALITY");
    _binding.lblDefense = getControl<Label>("LBL_DEFENSE");
    _binding.lblFortitude = getControl<Label>("LBL_DEFENSE");
    _binding.lblBevel = getControl<Label>("LBL_BEVEL");
    _binding.lblWisdom = getControl<Label>("LBL_WISDOM");
    _binding.lblBevel2 = getControl<Label>("LBL_BEVEL2");
    _binding.lblLight = getControl<Label>("LBL_LIGHT");
    _binding.lblDark = getControl<Label>("LBL_DARK");
    _binding.btnExit = getControl<Button>("BTN_EXIT");
    _binding.btnAuto = getControl<Button>("BTN_AUTO");
    _binding.btnLevelup = getControl<Button>("BTN_LEVELUP");
    if (!_game.isTSL()) {
        _binding.lblAdorn = getControl<Label>("LBL_ADORN");
        _binding.btnScripts = getControl<Button>("BTN_SCRIPTS");
        _binding.lblClass = getControl<Label>("LBL_CLASS");
        _binding.lblClass1 = getControl<Label>("LBL_CLASS1");
        _binding.lblClass2 = getControl<Label>("LBL_CLASS2");
        for (int i = 0; i < kNumControlsGood; ++i) {
            _binding.lblGood[i] = getControl<Label>("LBL_GOOD" + to_string(i + 1));
        }
        _binding.lblLevel = getControl<Label>("LBL_LEVEL");
        _binding.lblLevel1 = getControl<Label>("LBL_LEVEL1");
        _binding.lblLevel2 = getControl<Label>("LBL_LEVEL2");
        _binding.lblMore = getControl<Label>("LBL_MORE");
        _binding.lblName = getControl<Label>("LBL_NAME");
        _binding.btnChange1 = getControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControl<Button>("BTN_CHANGE2");
        _binding.btnCharLeft = getControl<Button>("BTN_CHARLEFT");
        _binding.btnCharRight = getControl<Button>("BTN_CHARRIGHT");
    } else {
        _binding.lblForceMastery = getControl<Label>("LBL_FORCEMASTERY");
        _binding.lblMoreBack = getControl<Label>("LBL_FORCEMASTERY");
        _binding.lblStatsBorder = getControl<Label>("LBL_STATSBORDER");
        _binding.lblTitle = getControl<Label>("LBL_TITLE");
        _binding.lblXpBack = getControl<Label>("LBL_XP_BACK");
        for (int i = 0; i < kNumControlsBar; ++i) {
            _binding.lblBar[i] = getControl<Label>("LBL_BAR" + to_string(i + 1));
        }
        _binding.btnChange1 = _inGameMenu.getBtnChange2();
        _binding.btnChange2 = _inGameMenu.getBtnChange3();
    }
}

void CharacterMenu::update(float dt) {
    shared_ptr<Creature> leader(_game.party().getLeader());
    _binding.btnLevelup->setVisible(leader->isLevelUpPending());
    _binding.btnAuto->setVisible(leader->isLevelUpPending());
    GUI::update(dt);
}

static string toStringOrEmptyIfZero(int value) {
    return value != 0 ? to_string(value) : "";
}

static string describeAbilityModifier(int value) {
    return value > 0 ? "+" + to_string(value) : to_string(value);
}

void CharacterMenu::refreshControls() {
    shared_ptr<Creature> partyLeader(_game.party().getLeader());
    CreatureAttributes &attributes = partyLeader->attributes();

    if (!_game.isTSL()) {
        _binding.lblClass1->setTextMessage(describeClass(attributes.getClassByPosition(1)));
        _binding.lblClass2->setTextMessage(describeClass(attributes.getClassByPosition(2)));
        _binding.lblLevel1->setTextMessage(toStringOrEmptyIfZero(attributes.getLevelByPosition(1)));
        _binding.lblLevel2->setTextMessage(toStringOrEmptyIfZero(attributes.getLevelByPosition(2)));
    }

    _binding.lblVitalityStat->setTextMessage(str(boost::format("%d/%d") % partyLeader->currentHitPoints() % partyLeader->hitPoints()));
    _binding.lblDefenseStat->setTextMessage(to_string(attributes.getDefense()));
    _binding.lblForceStat->setTextMessage("");

    _binding.lblStr->setTextMessage(to_string(attributes.strength()));
    _binding.lblStrMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Strength)));
    _binding.lblDex->setTextMessage(to_string(attributes.dexterity()));
    _binding.lblDexMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Dexterity)));
    _binding.lblCon->setTextMessage(to_string(attributes.constitution()));
    _binding.lblConMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Constitution)));
    _binding.lblInt->setTextMessage(to_string(attributes.intelligence()));
    _binding.lblIntMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Intelligence)));
    _binding.lblWis->setTextMessage(to_string(attributes.wisdom()));
    _binding.lblWisMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Wisdom)));
    _binding.lblCha->setTextMessage(to_string(attributes.charisma()));
    _binding.lblChaMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Charisma)));

    SavingThrows savingThrows(attributes.getAggregateSavingThrows());
    _binding.lblFortitudeStat->setTextMessage(to_string(savingThrows.fortitude));
    _binding.lblReflexStat->setTextMessage(to_string(savingThrows.reflex));
    _binding.lblWillStat->setTextMessage(to_string(savingThrows.will));

    _binding.lblExperienceStat->setTextMessage(to_string(partyLeader->xp()));
    _binding.lblNeededXp->setTextMessage(to_string(partyLeader->getNeededXP()));

    refreshPortraits();
    refresh3D();
}

string CharacterMenu::describeClass(ClassType clazz) const {
    if (clazz == ClassType::Invalid)
        return "";

    return _services.game.defaultClasses().get(clazz)->name();
}

void CharacterMenu::refreshPortraits() {
    if (_game.isTSL())
        return;

    Party &party = _game.party();
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::refresh3D() {
    auto &sceneGraph = _services.scene.defaultGraphs().get(kSceneCharacter);
    float aspect = _binding.lbl3dChar->extent().width / static_cast<float>(_binding.lbl3dChar->extent().height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(kDefaultClipPlaneNear, 10.0f)
        .modelSupplier(bind(&CharacterMenu::getSceneModel, this, _1))
        .modelOffset(glm::vec2(0.0f, 1.7f))
        .cameraFromModelNode("camerahook")
        .invoke();

    _binding.lbl3dChar->setSceneName(kSceneCharacter);
}

shared_ptr<ModelSceneNode> CharacterMenu::getSceneModel(SceneGraph &sceneGraph) const {
    auto partyLeader = _game.party().getLeader();

    auto character = _game.objectFactory().newCreature(sceneGraph.name());
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

    return move(sceneModel);
}

} // namespace game

} // namespace reone
