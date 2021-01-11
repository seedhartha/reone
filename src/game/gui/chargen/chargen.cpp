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

#include <stdexcept>

#include "../../../gui/scenebuilder.h"
#include "../../../resource/resources.h"

#include "../../game.h"
#include "../../portraitutil.h"
#include "../../rp/classes.h"

#include "../colorutil.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static const float kModelScale = 1.05f;
static const float kModelOffsetY = 0.9f;

CharacterGeneration::CharacterGeneration(Game *game) :
    GameGUI(game->version(), game->options().graphics),
    _game(game) {

    _resRef = getResRef("maincg");
    _backgroundType = BackgroundType::Menu;

    initForGame();
};

void CharacterGeneration::load() {
    GUI::load();

    hideControl("VIT_ARROW_LBL");
    hideControl("DEF_ARROW_LBL");
    hideControl("FORT_ARROW_LBL");
    hideControl("REFL_ARROW_LBL");
    hideControl("WILL_ARROW_LBL");
    hideControl("OLD_LBL");
    hideControl("NEW_LBL");

    setControlText("LBL_NAME", "");
    setControlText("LBL_LEVEL_VAL", "1");

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

void CharacterGeneration::loadClassSelection() {
    _classSelection = make_unique<ClassSelection>(_game);
    _classSelection->load();
}

void CharacterGeneration::loadQuickOrCustom() {
    _quickOrCustom = make_unique<QuickOrCustom>(this, _version, _gfxOpts);
    _quickOrCustom->load();
}

void CharacterGeneration::loadQuick() {
    _quick = make_unique<QuickCharacterGeneration>(this, _version, _gfxOpts);
    _quick->load();
}

void CharacterGeneration::loadCustom() {
    _custom = make_unique<CustomCharacterGeneration>(this, _version, _gfxOpts);
    _custom->load();
}

void CharacterGeneration::loadPortraitSelection() {
    _portraitSelection = make_unique<PortraitSelection>(this, _version, _gfxOpts);
    _portraitSelection->load();
}

void CharacterGeneration::loadAbilities() {
    _abilities = make_unique<CharGenAbilities>(this, _version, _gfxOpts);
    _abilities->load();
}

void CharacterGeneration::loadSkills() {
    _skills = make_unique<CharGenSkills>(this, _version, _gfxOpts);
    _skills->load();
}

void CharacterGeneration::loadFeats() {
    _feats = make_unique<CharGenFeats>(this, _version, _gfxOpts);
    _feats->load();
}

void CharacterGeneration::loadNameEntry() {
    _nameEntry = make_unique<NameEntry>(this, _version, _gfxOpts);
    _nameEntry->load();
}

void CharacterGeneration::loadLevelUp() {
    _levelUp = make_unique<LevelUpMenu>(this, _version, _gfxOpts);
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
            throw logic_error("CharGen: invalid screen: " + to_string(static_cast<int>(_screen)));
    }
}

void CharacterGeneration::update(float dt) {
    GUI::update(dt);
    getSubGUI()->update(dt);
}

void CharacterGeneration::render() const {
    GUI::render();
    getSubGUI()->render();
}

void CharacterGeneration::render3D() const {
    GUI::render3D();
    getSubGUI()->render3D();
}

void CharacterGeneration::openClassSelection() {
    hideControl("MODEL_LBL");
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
    showControl("MODEL_LBL");
    changeScreen(CharGenScreen::QuickOrCustom);
}

void CharacterGeneration::startCustom() {
    _type = Type::Custom;
    _custom->setStep(0);
    openSteps();
}

void CharacterGeneration::startQuick() {
    _type = Type::Quick;
    _quick->setStep(0);
    openSteps();
}

void CharacterGeneration::startLevelUp() {
    _type = Type::LevelUp;

    shared_ptr<Creature> partyLeader(_game->party().leader());
    const CreatureAttributes &attributes = partyLeader->attributes();
    StaticCreatureBlueprint character;
    character.setAppearance(partyLeader->appearance());
    character.setGender(partyLeader->gender());
    character.setAttributes(attributes);
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
    showControl("MODEL_LBL");
    changeScreen(CharGenScreen::Quick);
}

void CharacterGeneration::openCustom() {
    showControl("MODEL_LBL");
    changeScreen(CharGenScreen::Custom);
}

void CharacterGeneration::openPortraitSelection() {
    hideControl("MODEL_LBL");
    changeScreen(CharGenScreen::PortraitSelection);
}

void CharacterGeneration::openAbilities() {
    _abilities->reset(_type != Type::LevelUp);
    hideControl("MODEL_LBL");
    changeScreen(CharGenScreen::Abilities);
}

void CharacterGeneration::openSkills() {
    _skills->reset(_type != Type::LevelUp);
    hideControl("MODEL_LBL");
    changeScreen(CharGenScreen::Skills);
}

void CharacterGeneration::openFeats() {
    hideControl("MODEL_LBL");
    changeScreen(CharGenScreen::Feats);
}

void CharacterGeneration::openNameEntry() {
    hideControl("MODEL_LBL");
    changeScreen(CharGenScreen::Name);
}

void CharacterGeneration::openLevelUp() {
    _levelUp->reset();
    showControl("MODEL_LBL");
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
        shared_ptr<Creature> partyLeader(_game->party().leader());
        partyLeader->attributes() = _character->attributes();
        _game->openInGame();

    } else {
        string moduleName(_version == GameVersion::KotOR ? "end_m01aa" : "001ebo");

        auto character = make_shared<StaticCreatureBlueprint>(*_character);
        character->clearEquipment();

        shared_ptr<Creature> player(_game->objectFactory().newCreature());
        player->load(character);
        player->setTag(kObjectTagPlayer);
        player->setFaction(Faction::Friendly1);
        player->setImmortal(true);

        Party &party = _game->party();
        party.clear();
        party.addMember(kNpcPlayer, player);
        party.setPlayer(player);

        _game->loadModule(moduleName);
    }
}

void CharacterGeneration::setCharacter(StaticCreatureBlueprint character) {
    int currentAppearance = _character ? _character->appearance() : -1;
    Gender currentGender = _character ? _character->gender() : Gender::None;

    _character = make_unique<StaticCreatureBlueprint>(character);

    if (currentAppearance != character.appearance()) {
        loadCharacterModel();
        _portraitSelection->updatePortraits();
    }
    if (currentGender != character.gender()) {
        _nameEntry->loadRandomName();
    }

    updateAttributes();
}

void CharacterGeneration::setAbilities(CreatureAbilities abilities) {
    _character->attributes().setAbilities(move(abilities));
    updateAttributes();
}

void CharacterGeneration::setSkills(CreatureSkills skills) {
    _character->attributes().setSkills(move(skills));
}

void CharacterGeneration::loadCharacterModel() {
    Control &lblModel = getControl("MODEL_LBL");
    const Control::Extent &extent = lblModel.extent();
    float aspect = extent.width / static_cast<float>(extent.height);

    glm::mat4 cameraTransform(1.0f);
    cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 1.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));

    unique_ptr<Control::Scene3D> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier(bind(&CharacterGeneration::getCharacterModel, this, _1))
        .modelScale(kModelScale)
        .modelOffset(glm::vec2(0.0f, kModelOffsetY))
        .cameraTransform(cameraTransform)
        .ambientLightColor(glm::vec3(1.0f))
        .build());

    lblModel.setScene3D(move(scene));

    string portrait(getPortraitByAppearance(_character->appearance()));
    if (!portrait.empty()) {
        Control &lblPortrait = getControl("PORTRAIT_LBL");
        lblPortrait.setBorderFill(portrait);
    }
}

shared_ptr<ModelSceneNode> CharacterGeneration::getCharacterModel(SceneGraph &sceneGraph) {
    unique_ptr<ObjectFactory> objectFactory(new ObjectFactory(_game, &sceneGraph));

    unique_ptr<Creature> creature(objectFactory->newCreature());
    creature->load(_character);
    creature->updateModelAnimation();

    return creature->model();
}

void CharacterGeneration::updateAttributes() {
    shared_ptr<CreatureClass> clazz(Classes::instance().get(_character->attributes().getEffectiveClass()));
    setControlText("LBL_CLASS", clazz->name());

    const CreatureAbilities &abilities =  _character->attributes().abilities();

    int vitality = clazz->hitdie() + abilities.getModifier(Ability::Constitution);
    setControlText("LBL_VIT", to_string(vitality));

    int defense = 10 + clazz->getDefenseBonus(1) + abilities.getModifier(Ability::Dexterity);
    setControlText("LBL_DEF", to_string(defense));

    setControlText("STR_AB_LBL", to_string(abilities.strength()));
    setControlText("DEX_AB_LBL", to_string(abilities.dexterity()));
    setControlText("CON_AB_LBL", to_string(abilities.constitution()));
    setControlText("INT_AB_LBL", to_string(abilities.intelligence()));
    setControlText("WIS_AB_LBL", to_string(abilities.wisdom()));
    setControlText("CHA_AB_LBL", to_string(abilities.charisma()));

    const SavingThrows &throws = clazz->getSavingThrows(1);

    if (_version == GameVersion::TheSithLords) {
        setControlText("NEW_FORT_LBL", to_string(throws.fortitude));
        setControlText("NEW_REFL_LBL", to_string(throws.reflex));
        setControlText("NEW_WILL_LBL", to_string(throws.will));
    } else {
        setControlText("OLD_FORT_LBL", to_string(throws.fortitude));
        setControlText("OLD_REFL_LBL", to_string(throws.reflex));
        setControlText("OLD_WILL_LBL", to_string(throws.will));
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

StaticCreatureBlueprint &CharacterGeneration::character() {
    return *_character;
}

} // namespace game

} // namespace reone
