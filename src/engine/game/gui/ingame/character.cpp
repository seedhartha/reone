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

#include "character.h"

#include <boost/format.hpp>

#include "../../../graphics/model/models.h"
#include "../../../gui/scenebuilder.h"

#include "../../d20/classes.h"
#include "../../game.h"
#include "../../gameidutil.h"

#include "../colorutil.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

CharacterMenu::CharacterMenu(Game *game, InGameMenu &inGameMenu) : GameGUI(game), _inGameMenu(inGameMenu) {
    _resRef = getResRef("character");

    initForGame();
    loadBackground(BackgroundType::Menu);
}

void CharacterMenu::load() {
    GUI::load();
    bindControls();

    _binding.btnAuto->setDisabled(true);
    if (!isTSL(_game->gameId())) {
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
    _binding.lbl3dChar = getControlPtr<Label>("LBL_3DCHAR");
    _binding.btn3dChar = getControlPtr<Button>("BTN_3DCHAR");
    _binding.sldAlign = getControlPtr<Slider>("SLD_ALIGN");
    _binding.lblStr = getControlPtr<Label>("LBL_STR");
    _binding.lblFortitudeStat = getControlPtr<Label>("LBL_FORTITUDE_STAT");
    _binding.lblReflexStat = getControlPtr<Label>("LBL_REFLEX_STAT");
    _binding.lblWillStat = getControlPtr<Label>("LBL_WILL_STAT");
    _binding.lblDefenseStat = getControlPtr<Label>("LBL_DEFENSE_STAT");
    _binding.lblForceStat = getControlPtr<Label>("LBL_FORCE_STAT");
    _binding.lblVitalityStat = getControlPtr<Label>("LBL_VITALITY_STAT");
    _binding.lblDex = getControlPtr<Label>("LBL_DEX");
    _binding.lblCon = getControlPtr<Label>("LBL_CON");
    _binding.lblInt = getControlPtr<Label>("LBL_INT");
    _binding.lblCha = getControlPtr<Label>("LBL_CHA");
    _binding.lblWis = getControlPtr<Label>("LBL_WIS");
    _binding.lblStrMod = getControlPtr<Label>("LBL_STR_MOD");
    _binding.lblDexMod = getControlPtr<Label>("LBL_DEX_MOD");
    _binding.lblConMod = getControlPtr<Label>("LBL_CON_MOD");
    _binding.lblIntMod = getControlPtr<Label>("LBL_INT_MOD");
    _binding.lblWisMod = getControlPtr<Label>("LBL_WIS_MOD");
    _binding.lblChaMod = getControlPtr<Label>("LBL_CHA_MOD");
    _binding.lblExperienceStat = getControlPtr<Label>("LBL_EXPERIENCE_STAT");
    _binding.lblNeededXp = getControlPtr<Label>("LBL_NEEDED_XP");
    _binding.lblStrength = getControlPtr<Label>("LBL_STRENGTH");
    _binding.lblDexterity = getControlPtr<Label>("LBL_DEXTERITY");
    _binding.lblConstitution = getControlPtr<Label>("LBL_CONSTITUTION");
    _binding.lblIntelligence = getControlPtr<Label>("LBL_INTELLIGENCE");
    _binding.lblCharisma = getControlPtr<Label>("LBL_CHARISMA");
    _binding.lblReflex = getControlPtr<Label>("LBL_REFLEX");
    _binding.lblWill = getControlPtr<Label>("LBL_WILL");
    _binding.lblExperience = getControlPtr<Label>("LBL_EXPERIENCE");
    _binding.lblNextLevel = getControlPtr<Label>("LBL_NEXT_LEVEL");
    _binding.lblForce = getControlPtr<Label>("LBL_FORCE");
    _binding.lblVitality = getControlPtr<Label>("LBL_VITALITY");
    _binding.lblDefense = getControlPtr<Label>("LBL_DEFENSE");
    _binding.lblFortitude = getControlPtr<Label>("LBL_DEFENSE");
    _binding.lblBevel = getControlPtr<Label>("LBL_BEVEL");
    _binding.lblWisdom = getControlPtr<Label>("LBL_WISDOM");
    _binding.lblBevel2 = getControlPtr<Label>("LBL_BEVEL2");
    _binding.lblLight = getControlPtr<Label>("LBL_LIGHT");
    _binding.lblDark = getControlPtr<Label>("LBL_DARK");
    _binding.btnExit = getControlPtr<Button>("BTN_EXIT");
    _binding.btnAuto = getControlPtr<Button>("BTN_AUTO");
    _binding.btnLevelup = getControlPtr<Button>("BTN_LEVELUP");
    if (!isTSL(_game->gameId())) {
        _binding.lblAdorn = getControlPtr<Label>("LBL_ADORN");
        _binding.btnScripts = getControlPtr<Button>("BTN_SCRIPTS");
        _binding.lblClass = getControlPtr<Label>("LBL_CLASS");
        _binding.lblClass1 = getControlPtr<Label>("LBL_CLASS1");
        _binding.lblClass2 = getControlPtr<Label>("LBL_CLASS2");
        for (int i = 0; i < 10; ++i) {
            _binding.lblGood[i] = getControlPtr<Label>("LBL_GOOD" + to_string(i + 1));
        }
        _binding.lblLevel = getControlPtr<Label>("LBL_LEVEL");
        _binding.lblLevel1 = getControlPtr<Label>("LBL_LEVEL1");
        _binding.lblLevel2 = getControlPtr<Label>("LBL_LEVEL2");
        _binding.lblMore = getControlPtr<Label>("LBL_MORE");
        _binding.lblName = getControlPtr<Label>("LBL_NAME");
        _binding.btnChange1 = getControlPtr<Button>("BTN_CHANGE1");
        _binding.btnChange2 = getControlPtr<Button>("BTN_CHANGE2");
        _binding.btnCharLeft = getControlPtr<Button>("BTN_CHARLEFT");
        _binding.btnCharRight = getControlPtr<Button>("BTN_CHARRIGHT");
     } else {
         _binding.lblForceMastery = getControlPtr<Label>("LBL_FORCEMASTERY");
         _binding.lblMoreBack = getControlPtr<Label>("LBL_FORCEMASTERY");
         _binding.lblStatsBorder = getControlPtr<Label>("LBL_STATSBORDER");
         _binding.lblTitle = getControlPtr<Label>("LBL_TITLE");
         _binding.lblXpBack = getControlPtr<Label>("LBL_XP_BACK");
        for (int i = 0; i < 6; ++i) {
            _binding.lblBar[i] = getControlPtr<Label>("LBL_BAR" + to_string(i + 1));
        }
        _binding.btnChange1 = _inGameMenu.getBtnChange2();
        _binding.btnChange2 = _inGameMenu.getBtnChange3();
    }
}

void CharacterMenu::update(float dt) {
    shared_ptr<Creature> leader(_game->services().party().getLeader());
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
    shared_ptr<Creature> partyLeader(_game->services().party().getLeader());
    CreatureAttributes &attributes = partyLeader->attributes();

    if (!isTSL(_game->gameId())) {
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
    if (clazz == ClassType::Invalid) return "";

    return _game->services().classes().get(clazz)->name();
}

void CharacterMenu::refreshPortraits() {
    if (_game->gameId() != GameID::KotOR) return;

    Party &party = _game->services().party();
    shared_ptr<Creature> partyMember1(party.getMember(1));
    shared_ptr<Creature> partyMember2(party.getMember(2));

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::refresh3D() {
    float aspect = _binding.lbl3dChar->extent().width / static_cast<float>(_binding.lbl3dChar->extent().height);

    auto scene = SceneBuilder(_options, _game->services().graphics())
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&CharacterMenu::getSceneModel, this, _1))
        .modelOffset(glm::vec2(0.0f, 1.7f))
        .cameraFromModelNode("camerahook")
        .lightingRefFromModelNode("charmain_light")
        .build();

    _binding.lbl3dChar->setScene(move(scene));
}

shared_ptr<ModelSceneNode> CharacterMenu::getSceneModel(SceneGraph &sceneGraph) const {
    auto partyLeader = _game->services().party().getLeader();
    auto objectFactory = make_shared<ObjectFactory>(*_game, sceneGraph);
    auto character = objectFactory->newCreature();
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

    auto sceneModel = make_shared<ModelSceneNode>(_game->services().graphics().models().get("charmain_light"), ModelUsage::GUI, &sceneGraph);
    sceneModel->attach("charmain_light", character->sceneNode());

    return move(sceneModel);
}

void CharacterMenu::onClick(const string &control) {
    GameGUI::onClick(control);

    if (control == "BTN_EXIT") {
        _game->openInGame();
    } else if (control == "BTN_LEVELUP") {
        _game->openLevelUp();
    }
}

} // namespace game

} // namespace reone
