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

#include "chargen.h"

#include "../../../di/services/graphics.h"
#include "../../../gui/scenebuilder.h"
#include "../../../graphics/model/models.h"
#include "../../../resource/resources.h"

#include "../../d20/classes.h"
#include "../../game.h"
#include "../../portraits.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 1.1f;

CharacterGeneration::CharacterGeneration(Game *game) : GameGUI(game) {
    _resRef = getResRef("maincg");

    initForGame();
    loadBackground(BackgroundType::Menu);
};

void CharacterGeneration::load() {
    GUI::load();
    bindControls();

    _binding.lblLevelVal->setVisible(false);
    _binding.lblName->setTextMessage("");

    if (_game->isKotOR()) {
        _binding.lblLevel->setVisible(false);
        _binding.vitArrowLbl->setVisible(false);
        _binding.defArrowLbl->setVisible(false);
        _binding.fortArrowLbl->setVisible(false);
        _binding.reflArrowLbl->setVisible(false);
        _binding.willArrowLbl->setVisible(false);
        _binding.oldLbl->setVisible(false);
        _binding.newLbl->setVisible(false);
    }

    loadClassSelection();
    loadQuickOrCustom();
    loadQuick();
    loadPortraitSelection();
    loadNameEntry();
    loadCustom();
    loadAbilities();
    loadSkills();
    loadFeats();
    loadLevelUp();
}

void CharacterGeneration::bindControls() {
    _binding.lblClass = getControl<Label>("LBL_CLASS");
    _binding.lblDef = getControl<Label>("LBL_DEF");
    _binding.lblLevelVal = getControl<Label>("LBL_LEVEL_VAL");
    _binding.lblName = getControl<Label>("LBL_NAME");
    _binding.lblVit = getControl<Label>("LBL_VIT");
    _binding.modelLbl = getControl<Label>("MODEL_LBL");
    _binding.portraitLbl = getControl<Label>("PORTRAIT_LBL");

    _binding.strAbLbl = getControl<Label>("STR_AB_LBL");
    _binding.dexAbLbl = getControl<Label>("DEX_AB_LBL");
    _binding.conAbLbl = getControl<Label>("CON_AB_LBL");
    _binding.intAbLbl = getControl<Label>("INT_AB_LBL");
    _binding.wisAbLbl = getControl<Label>("WIS_AB_LBL");
    _binding.chaAbLbl = getControl<Label>("CHA_AB_LBL");

    if (_game->isKotOR()) {
        _binding.newLbl = getControl<Label>("NEW_LBL");
        _binding.oldLbl = getControl<Label>("OLD_LBL");
        _binding.lblLevel = getControl<Label>("LBL_LEVEL");
        _binding.oldFortLbl = getControl<Label>("OLD_FORT_LBL");
        _binding.oldReflLbl = getControl<Label>("OLD_REFL_LBL");
        _binding.oldWillLbl = getControl<Label>("OLD_WILL_LBL");
        _binding.vitArrowLbl = getControl<Label>("VIT_ARROW_LBL");
        _binding.defArrowLbl = getControl<Label>("DEF_ARROW_LBL");
        _binding.fortArrowLbl = getControl<Label>("FORT_ARROW_LBL");
        _binding.reflArrowLbl = getControl<Label>("REFL_ARROW_LBL");
        _binding.willArrowLbl = getControl<Label>("WILL_ARROW_LBL");
    } else {
        _binding.newFortLbl = getControl<Label>("NEW_FORT_LBL");
        _binding.newReflLbl = getControl<Label>("NEW_REFL_LBL");
        _binding.newWillLbl = getControl<Label>("NEW_WILL_LBL");
    }
}

void CharacterGeneration::loadClassSelection() {
    _classSelection = make_unique<ClassSelection>(_game);
    _classSelection->load();
}

void CharacterGeneration::loadQuickOrCustom() {
    _quickOrCustom = make_unique<QuickOrCustom>(this, _game);
    _quickOrCustom->load();
}

void CharacterGeneration::loadQuick() {
    _quick = make_unique<QuickCharacterGeneration>(this, _game);
    _quick->load();
}

void CharacterGeneration::loadCustom() {
    _custom = make_unique<CustomCharacterGeneration>(this, _game);
    _custom->load();
}

void CharacterGeneration::loadPortraitSelection() {
    _portraitSelection = make_unique<PortraitSelection>(this, _game);
    _portraitSelection->load();
}

void CharacterGeneration::loadAbilities() {
    _abilities = make_unique<CharGenAbilities>(this, _game);
    _abilities->load();
}

void CharacterGeneration::loadSkills() {
    _skills = make_unique<CharGenSkills>(this, _game);
    _skills->load();
}

void CharacterGeneration::loadFeats() {
    _feats = make_unique<CharGenFeats>(this, _game);
    _feats->load();
}

void CharacterGeneration::loadNameEntry() {
    _nameEntry = make_unique<NameEntry>(this, _game);
    _nameEntry->load();
}

void CharacterGeneration::loadLevelUp() {
    _levelUp = make_unique<LevelUpMenu>(this, _game);
    _levelUp->load();
}

bool CharacterGeneration::handle(const SDL_Event &event) {
    if (getSubGUI()->handle(event)) {
        return true;
    }
    return GUI::handle(event);
}

GUI *CharacterGeneration::getSubGUI() const {
    switch (_screen) {
        case CharGenScreen::ClassSelection:
            return _classSelection.get();
        case CharGenScreen::QuickOrCustom:
            return _quickOrCustom.get();
        case CharGenScreen::Quick:
            return _quick.get();
        case CharGenScreen::Custom:
            return _custom.get();
        case CharGenScreen::PortraitSelection:
            return _portraitSelection.get();
        case CharGenScreen::Abilities:
            return _abilities.get();
        case CharGenScreen::Skills:
            return _skills.get();
        case CharGenScreen::Feats:
            return _feats.get();
        case CharGenScreen::Name:
            return _nameEntry.get();
        case CharGenScreen::LevelUp:
            return _levelUp.get();
        default:
            throw logic_error("Invalid screen: " + to_string(static_cast<int>(_screen)));
    }
}

void CharacterGeneration::update(float dt) {
    GUI::update(dt);
    getSubGUI()->update(dt);
}

void CharacterGeneration::draw() {
    GUI::draw();
    getSubGUI()->draw();
}

void CharacterGeneration::draw3D() {
    GUI::draw3D();
    getSubGUI()->draw3D();
}

void CharacterGeneration::openClassSelection() {
    _binding.modelLbl->setVisible(false);
    changeScreen(CharGenScreen::ClassSelection);
}

void CharacterGeneration::changeScreen(CharGenScreen screen) {
    GUI *gui = getSubGUI();
    if (gui) {
        gui->resetFocus();
    }
    _screen = screen;
}

void CharacterGeneration::openQuickOrCustom() {
    setAttributesVisible(false);
    _binding.modelLbl->setVisible(true);
    changeScreen(CharGenScreen::QuickOrCustom);
}

void CharacterGeneration::setAttributesVisible(bool visible) {
    vector<Label *> attributesLabels {
        _binding.lblVit.get(),
        _binding.lblDef.get(),
        _binding.strAbLbl.get(),
        _binding.dexAbLbl.get(),
        _binding.conAbLbl.get(),
        _binding.intAbLbl.get(),
        _binding.wisAbLbl.get(),
        _binding.chaAbLbl.get()
    };
    if (_game->isKotOR()) {
        attributesLabels.push_back(_binding.oldFortLbl.get());
        attributesLabels.push_back(_binding.oldReflLbl.get());
        attributesLabels.push_back(_binding.oldWillLbl.get());
    } else {
        attributesLabels.push_back(_binding.newFortLbl.get());
        attributesLabels.push_back(_binding.newReflLbl.get());
        attributesLabels.push_back(_binding.newWillLbl.get());
    }
    for (auto &label : attributesLabels) {
        label->setVisible(visible);
    }
}

void CharacterGeneration::startCustom() {
    _type = Type::Custom;
    _custom->setStep(0);
    openSteps();
}

void CharacterGeneration::startQuick() {
    setAttributesVisible(true);
    _type = Type::Quick;
    _quick->setStep(0);
    openSteps();
}

void CharacterGeneration::startLevelUp() {
    _type = Type::LevelUp;

    shared_ptr<Creature> partyLeader(_game->services().party().getLeader());

    Character character;
    character.appearance = partyLeader->appearance();
    character.gender = partyLeader->gender();
    character.attributes = partyLeader->attributes();
    setCharacter(move(character));

    int nextLevel = partyLeader->attributes().getAggregateLevel() + 1;
    bool hasAttributes = nextLevel % 4 == 0;
    _levelUp->setStep(hasAttributes ? 0 : 1);

    openSteps();
}

void CharacterGeneration::openSteps() {
    switch (_type) {
        case Type::LevelUp:
            openLevelUp();
            break;
        case Type::Custom:
            openCustom();
            break;
        default:
            openQuick();
            break;
    }
}

void CharacterGeneration::openQuick() {
    _binding.modelLbl->setVisible(true);
    changeScreen(CharGenScreen::Quick);
}

void CharacterGeneration::openCustom() {
    setAttributesVisible(_custom->step() > 1);
    _binding.modelLbl->setVisible(true);
    changeScreen(CharGenScreen::Custom);
}

void CharacterGeneration::openPortraitSelection() {
    _binding.modelLbl->setVisible(false);
    changeScreen(CharGenScreen::PortraitSelection);
}

void CharacterGeneration::openAbilities() {
    _abilities->reset(_type != Type::LevelUp);
    _binding.modelLbl->setVisible(false);
    changeScreen(CharGenScreen::Abilities);
}

void CharacterGeneration::openSkills() {
    _skills->reset(_type != Type::LevelUp);
    _binding.modelLbl->setVisible(false);
    changeScreen(CharGenScreen::Skills);
}

void CharacterGeneration::openFeats() {
    _binding.modelLbl->setVisible(false);
    changeScreen(CharGenScreen::Feats);
}

void CharacterGeneration::openNameEntry() {
    _binding.modelLbl->setVisible(false);
    changeScreen(CharGenScreen::Name);
}

void CharacterGeneration::openLevelUp() {
    _levelUp->reset();
    _binding.modelLbl->setVisible(true);
    changeScreen(CharGenScreen::LevelUp);
}

void CharacterGeneration::cancel() {
    if (_type == Type::LevelUp) {
        _game->openInGame();
    } else {
        _game->openMainMenu();
    }
}

void CharacterGeneration::finish() {
    if (_type == Type::LevelUp) {
        ClassType classType = _character.attributes.getEffectiveClass();
        shared_ptr<CreatureClass> clazz(_game->services().classes().get(classType));
        _character.attributes.addClassLevels(clazz.get(), 1);
        shared_ptr<Creature> partyLeader(_game->services().party().getLeader());
        partyLeader->attributes() = _character.attributes;
        _game->openInGame();
    } else {
        shared_ptr<Creature> player(_game->services().objectFactory().newCreature());
        player->setTag(kObjectTagPlayer);
        player->setGender(_character.gender);
        player->setAppearance(_character.appearance);
        player->loadAppearance();
        player->setFaction(Faction::Friendly1);
        player->setImmortal(true);
        player->attributes() = _character.attributes;

        Party &party = _game->services().party();
        party.clear();
        party.addMember(kNpcPlayer, player);
        party.setPlayer(player);

        string moduleName(_game->isKotOR() ? "end_m01aa" : "001ebo");
        _game->loadModule(moduleName);
    }
}

void CharacterGeneration::setCharacter(Character character) {
    bool appearanceChanged = character.appearance != _character.appearance;

    _character = move(character);

    if (appearanceChanged) {
        reloadCharacterModel();
        _portraitSelection->updatePortraits();
        _nameEntry->loadRandomName();
    }

    updateAttributes();
}

void CharacterGeneration::reloadCharacterModel() {
    const Control::Extent &extent = _binding.modelLbl->extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    unique_ptr<SceneGraph> scene(SceneBuilder(_options, _game->services().graphics())
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&CharacterGeneration::getCharacterModel, this, _1))
        .modelScale(kModelScale)
        .cameraFromModelNode("camerahook")
        .lightingRefFromModelNode("cgbody_light")
        .build());

    _binding.modelLbl->setScene(move(scene));
    _binding.portraitLbl->setBorderFill(_game->services().portraits().getTextureByAppearance(_character.appearance));
}

shared_ptr<ModelSceneNode> CharacterGeneration::getCharacterModel(SceneGraph &sceneGraph) {
    auto objectFactory = make_unique<ObjectFactory>(*_game, sceneGraph);

    shared_ptr<Creature> creature(objectFactory->newCreature());
    creature->setFacing(-glm::half_pi<float>());
    creature->setAppearance(_character.appearance);
    creature->equip("g_a_clothes01");
    creature->loadAppearance();
    creature->sceneNode()->setCullable(false);
    creature->updateModelAnimation();

    auto model = make_shared<ModelSceneNode>(_game->services().graphics().models().get("cgbody_light"), ModelUsage::GUI, &sceneGraph);
    model->attach("cgbody_light", creature->sceneNode());

    return move(model);
}

void CharacterGeneration::updateAttributes() {
    shared_ptr<CreatureClass> clazz(_game->services().classes().get(_character.attributes.getEffectiveClass()));
    _binding.lblClass->setTextMessage(clazz->name());

    int vitality = clazz->hitdie() + _character.attributes.getAbilityModifier(Ability::Constitution);
    _binding.lblVit->setTextMessage(to_string(vitality));

    int defense = _character.attributes.getDefense();
    _binding.lblDef->setTextMessage(to_string(defense));

    _binding.strAbLbl->setTextMessage(to_string(_character.attributes.strength()));
    _binding.dexAbLbl->setTextMessage(to_string(_character.attributes.dexterity()));
    _binding.conAbLbl->setTextMessage(to_string(_character.attributes.constitution()));
    _binding.intAbLbl->setTextMessage(to_string(_character.attributes.intelligence()));
    _binding.wisAbLbl->setTextMessage(to_string(_character.attributes.wisdom()));
    _binding.chaAbLbl->setTextMessage(to_string(_character.attributes.charisma()));

    const SavingThrows &throws = clazz->getSavingThrows(1);

    if (_game->isTSL()) {
        _binding.newFortLbl->setTextMessage(to_string(throws.fortitude));
        _binding.newReflLbl->setTextMessage(to_string(throws.reflex));
        _binding.newWillLbl->setTextMessage(to_string(throws.will));
    } else {
        _binding.oldFortLbl->setTextMessage(to_string(throws.fortitude));
        _binding.oldReflLbl->setTextMessage(to_string(throws.reflex));
        _binding.oldWillLbl->setTextMessage(to_string(throws.will));
    }
}

void CharacterGeneration::goToNextStep() {
    switch (_type) {
        case Type::LevelUp:
            _levelUp->goToNextStep();
            break;
        case Type::Custom:
            _custom->goToNextStep();
            break;
        default:
            _quick->goToNextStep();
            break;
    }
}

} // namespace game

} // namespace reone
