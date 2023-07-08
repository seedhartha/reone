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

#include "reone/graphics/di/services.h"
#include "reone/graphics/models.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/ingame.h"
#include "reone/game/object/factory.h"
#include "reone/game/party.h"
#include "reone/game/types.h"

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
    _binding.lbl3dChar = findControl<Label>("LBL_3DCHAR");
    _binding.btn3dChar = findControl<Button>("BTN_3DCHAR");
    _binding.sldAlign = findControl<Slider>("SLD_ALIGN");
    _binding.lblStr = findControl<Label>("LBL_STR");
    _binding.lblFortitudeStat = findControl<Label>("LBL_FORTITUDE_STAT");
    _binding.lblReflexStat = findControl<Label>("LBL_REFLEX_STAT");
    _binding.lblWillStat = findControl<Label>("LBL_WILL_STAT");
    _binding.lblDefenseStat = findControl<Label>("LBL_DEFENSE_STAT");
    _binding.lblForceStat = findControl<Label>("LBL_FORCE_STAT");
    _binding.lblVitalityStat = findControl<Label>("LBL_VITALITY_STAT");
    _binding.lblDex = findControl<Label>("LBL_DEX");
    _binding.lblCon = findControl<Label>("LBL_CON");
    _binding.lblInt = findControl<Label>("LBL_INT");
    _binding.lblCha = findControl<Label>("LBL_CHA");
    _binding.lblWis = findControl<Label>("LBL_WIS");
    _binding.lblStrMod = findControl<Label>("LBL_STR_MOD");
    _binding.lblDexMod = findControl<Label>("LBL_DEX_MOD");
    _binding.lblConMod = findControl<Label>("LBL_CON_MOD");
    _binding.lblIntMod = findControl<Label>("LBL_INT_MOD");
    _binding.lblWisMod = findControl<Label>("LBL_WIS_MOD");
    _binding.lblChaMod = findControl<Label>("LBL_CHA_MOD");
    _binding.lblExperienceStat = findControl<Label>("LBL_EXPERIENCE_STAT");
    _binding.lblNeededXp = findControl<Label>("LBL_NEEDED_XP");
    _binding.lblStrength = findControl<Label>("LBL_STRENGTH");
    _binding.lblDexterity = findControl<Label>("LBL_DEXTERITY");
    _binding.lblConstitution = findControl<Label>("LBL_CONSTITUTION");
    _binding.lblIntelligence = findControl<Label>("LBL_INTELLIGENCE");
    _binding.lblCharisma = findControl<Label>("LBL_CHARISMA");
    _binding.lblReflex = findControl<Label>("LBL_REFLEX");
    _binding.lblWill = findControl<Label>("LBL_WILL");
    _binding.lblExperience = findControl<Label>("LBL_EXPERIENCE");
    _binding.lblNextLevel = findControl<Label>("LBL_NEXT_LEVEL");
    _binding.lblForce = findControl<Label>("LBL_FORCE");
    _binding.lblVitality = findControl<Label>("LBL_VITALITY");
    _binding.lblDefense = findControl<Label>("LBL_DEFENSE");
    _binding.lblFortitude = findControl<Label>("LBL_DEFENSE");
    _binding.lblBevel = findControl<Label>("LBL_BEVEL");
    _binding.lblWisdom = findControl<Label>("LBL_WISDOM");
    _binding.lblBevel2 = findControl<Label>("LBL_BEVEL2");
    _binding.lblLight = findControl<Label>("LBL_LIGHT");
    _binding.lblDark = findControl<Label>("LBL_DARK");
    _binding.btnExit = findControl<Button>("BTN_EXIT");
    _binding.btnAuto = findControl<Button>("BTN_AUTO");
    _binding.btnLevelup = findControl<Button>("BTN_LEVELUP");
    if (!_game.isTSL()) {
        _binding.lblAdorn = findControl<Label>("LBL_ADORN");
        _binding.btnScripts = findControl<Button>("BTN_SCRIPTS");
        _binding.lblClass = findControl<Label>("LBL_CLASS");
        _binding.lblClass1 = findControl<Label>("LBL_CLASS1");
        _binding.lblClass2 = findControl<Label>("LBL_CLASS2");
        for (int i = 0; i < kNumControlsGood; ++i) {
            _binding.lblGood[i] = findControl<Label>("LBL_GOOD" + std::to_string(i + 1));
        }
        _binding.lblLevel = findControl<Label>("LBL_LEVEL");
        _binding.lblLevel1 = findControl<Label>("LBL_LEVEL1");
        _binding.lblLevel2 = findControl<Label>("LBL_LEVEL2");
        _binding.lblMore = findControl<Label>("LBL_MORE");
        _binding.lblName = findControl<Label>("LBL_NAME");
        _binding.btnChange1 = findControl<Button>("BTN_CHANGE1");
        _binding.btnChange2 = findControl<Button>("BTN_CHANGE2");
        _binding.btnCharLeft = findControl<Button>("BTN_CHARLEFT");
        _binding.btnCharRight = findControl<Button>("BTN_CHARRIGHT");
    } else {
        _binding.lblForceMastery = findControl<Label>("LBL_FORCEMASTERY");
        _binding.lblMoreBack = findControl<Label>("LBL_FORCEMASTERY");
        _binding.lblStatsBorder = findControl<Label>("LBL_STATSBORDER");
        _binding.lblTitle = findControl<Label>("LBL_TITLE");
        _binding.lblXpBack = findControl<Label>("LBL_XP_BACK");
        for (int i = 0; i < kNumControlsBar; ++i) {
            _binding.lblBar[i] = findControl<Label>("LBL_BAR" + std::to_string(i + 1));
        }
        _binding.btnChange1 = _inGameMenu.getBtnChange2();
        _binding.btnChange2 = _inGameMenu.getBtnChange3();
    }
}

void CharacterMenu::update(float dt) {
    std::shared_ptr<Creature> leader(_game.party().getLeader());
    _binding.btnLevelup->setVisible(leader->isLevelUpPending());
    _binding.btnAuto->setVisible(leader->isLevelUpPending());
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
        _binding.lblClass1->setTextMessage(describeClass(attributes.getClassByPosition(1)));
        _binding.lblClass2->setTextMessage(describeClass(attributes.getClassByPosition(2)));
        _binding.lblLevel1->setTextMessage(toStringOrEmptyIfZero(attributes.getLevelByPosition(1)));
        _binding.lblLevel2->setTextMessage(toStringOrEmptyIfZero(attributes.getLevelByPosition(2)));
    }

    _binding.lblVitalityStat->setTextMessage(str(boost::format("%d/%d") % partyLeader->currentHitPoints() % partyLeader->hitPoints()));
    _binding.lblDefenseStat->setTextMessage(std::to_string(attributes.getDefense()));
    _binding.lblForceStat->setTextMessage("");

    _binding.lblStr->setTextMessage(std::to_string(attributes.strength()));
    _binding.lblStrMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Strength)));
    _binding.lblDex->setTextMessage(std::to_string(attributes.dexterity()));
    _binding.lblDexMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Dexterity)));
    _binding.lblCon->setTextMessage(std::to_string(attributes.constitution()));
    _binding.lblConMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Constitution)));
    _binding.lblInt->setTextMessage(std::to_string(attributes.intelligence()));
    _binding.lblIntMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Intelligence)));
    _binding.lblWis->setTextMessage(std::to_string(attributes.wisdom()));
    _binding.lblWisMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Wisdom)));
    _binding.lblCha->setTextMessage(std::to_string(attributes.charisma()));
    _binding.lblChaMod->setTextMessage(describeAbilityModifier(attributes.getAbilityModifier(Ability::Charisma)));

    SavingThrows savingThrows(attributes.getAggregateSavingThrows());
    _binding.lblFortitudeStat->setTextMessage(std::to_string(savingThrows.fortitude));
    _binding.lblReflexStat->setTextMessage(std::to_string(savingThrows.reflex));
    _binding.lblWillStat->setTextMessage(std::to_string(savingThrows.will));

    _binding.lblExperienceStat->setTextMessage(std::to_string(partyLeader->xp()));
    _binding.lblNeededXp->setTextMessage(std::to_string(partyLeader->getNeededXP()));

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

    _binding.btnChange1->setBorderFill(partyMember1 ? partyMember1->portrait() : nullptr);
    _binding.btnChange1->setHilightFill(partyMember1 ? partyMember1->portrait() : nullptr);

    _binding.btnChange2->setBorderFill(partyMember2 ? partyMember2->portrait() : nullptr);
    _binding.btnChange2->setHilightFill(partyMember2 ? partyMember2->portrait() : nullptr);
}

void CharacterMenu::refresh3D() {
    auto &sceneGraph = _services.scene.graphs.get(kSceneCharacter);
    float aspect = _binding.lbl3dChar->extent().width / static_cast<float>(_binding.lbl3dChar->extent().height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(kDefaultClipPlaneNear, 10.0f)
        .modelSupplier(bind(&CharacterMenu::getSceneModel, this, std::placeholders::_1))
        .modelOffset(glm::vec2(0.0f, 1.7f))
        .cameraFromModelNode("camerahook")
        .invoke();

    _binding.lbl3dChar->setSceneName(kSceneCharacter);
}

std::shared_ptr<ModelSceneNode> CharacterMenu::getSceneModel(ISceneGraph &sceneGraph) const {
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

    return std::move(sceneModel);
}

} // namespace game

} // namespace reone
