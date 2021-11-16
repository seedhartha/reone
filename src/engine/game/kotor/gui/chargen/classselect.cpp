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

#include "classselect.h"

#include "../../../../common/randomutil.h"
#include "../../../../graphics/model/models.h"
#include "../../../../gui/control/button.h"
#include "../../../../gui/control/label.h"
#include "../../../../gui/scenebuilder.h"
#include "../../../../resource/strings.h"

#include "../../../core/d20/classes.h"
#include "../../../core/object/creature.h"
#include "../../../core/object/factory.h"
#include "../../../core/party.h"
#include "../../../core/portraits.h"

#include "../../kotor.h"

#include "chargen.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 1.1f;

static map<Gender, int> g_genderStrRefs {
    {Gender::Male, 646},
    {Gender::Female, 647}};

static map<ClassType, int> g_classDescStrRefs {
    {ClassType::Scoundrel, 32109},
    {ClassType::Scout, 32110},
    {ClassType::Soldier, 32111},
    {ClassType::JediConsular, 48031},
    {ClassType::JediSentinel, 48032},
    {ClassType::JediGuardian, 48033}};

ClassSelection::ClassSelection(
    KotOR *game,
    CharacterGeneration &charGen,
    ActionFactory &actionFactory,
    Classes &classes,
    Combat &combat,
    Feats &feats,
    FootstepSounds &footstepSounds,
    GUISounds &guiSounds,
    ObjectFactory &objectFactory,
    Party &party,
    Portraits &portraits,
    Reputes &reputes,
    ScriptRunner &scriptRunner,
    SoundSets &soundSets,
    Surfaces &surfaces,
    AudioFiles &audioFiles,
    AudioPlayer &audioPlayer,
    Context &context,
    Features &features,
    Fonts &fonts,
    Lips &lips,
    Materials &materials,
    Meshes &meshes,
    Models &models,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    Walkmeshes &walkmeshes,
    Window &window,
    Resources &resources,
    Strings &strings,
    TwoDas &twoDas) :
    GameGUI(
        game,
        actionFactory,
        classes,
        combat,
        feats,
        footstepSounds,
        guiSounds,
        objectFactory,
        party,
        portraits,
        reputes,
        scriptRunner,
        soundSets,
        surfaces,
        audioFiles,
        audioPlayer,
        context,
        features,
        fonts,
        lips,
        materials,
        meshes,
        models,
        pbrIbl,
        shaders,
        textures,
        walkmeshes,
        window,
        resources,
        strings,
        twoDas),
    _charGen(charGen) {
    _resRef = getResRef("classsel");

    if (!game->isTSL()) {
        loadBackground(BackgroundType::Menu);
    }

    initForGame();
}

void ClassSelection::load() {
    GUI::load();
    bindControls();
    setupClassButtons();
    setButtonColors(*_binding.btnBack);

    _binding.btnBack->setOnClick([this]() {
        _charGen.cancel();
    });
}

void ClassSelection::bindControls() {
    _binding.btnBack = getControl<Button>("BTN_BACK");
    _binding.btnSel1 = getControl<Button>("BTN_SEL1");
    _binding.btnSel2 = getControl<Button>("BTN_SEL2");
    _binding.btnSel3 = getControl<Button>("BTN_SEL3");
    _binding.btnSel4 = getControl<Button>("BTN_SEL4");
    _binding.btnSel5 = getControl<Button>("BTN_SEL5");
    _binding.btnSel6 = getControl<Button>("BTN_SEL6");
    _binding.lblClass = getControl<Label>("LBL_CLASS");
    _binding.lblDesc = getControl<Label>("LBL_DESC");
    _binding.threeDModel1 = getControl<Label>("3D_MODEL1");
    _binding.threeDModel2 = getControl<Label>("3D_MODEL2");
    _binding.threeDModel3 = getControl<Label>("3D_MODEL3");
    _binding.threeDModel4 = getControl<Label>("3D_MODEL4");
    _binding.threeDModel5 = getControl<Label>("3D_MODEL5");
    _binding.threeDModel6 = getControl<Label>("3D_MODEL6");
}

void ClassSelection::setupClassButtons() {
    _enlargedButtonSize = glm::vec2(_binding.btnSel1->extent().width, _binding.btnSel1->extent().height);
    _defaultButtonSize = glm::vec2(_binding.btnSel2->extent().width, _binding.btnSel2->extent().height);

    setupClassButton(0, Gender::Male, !_game->isTSL() ? ClassType::Scoundrel : ClassType::JediConsular);
    setupClassButton(1, Gender::Male, !_game->isTSL() ? ClassType::Scout : ClassType::JediSentinel);
    setupClassButton(2, Gender::Male, !_game->isTSL() ? ClassType::Soldier : ClassType::JediGuardian);
    setupClassButton(3, Gender::Female, !_game->isTSL() ? ClassType::Soldier : ClassType::JediGuardian);
    setupClassButton(4, Gender::Female, !_game->isTSL() ? ClassType::Scout : ClassType::JediSentinel);
    setupClassButton(5, Gender::Female, !_game->isTSL() ? ClassType::Scoundrel : ClassType::JediConsular);

    setClassButtonEnlarged(0, false);
}

void ClassSelection::setupClassButton(int index, Gender gender, ClassType clazz) {
    int appearance = getRandomCharacterAppearance(gender, clazz);

    Character character;
    character.gender = gender;
    character.appearance = appearance;
    character.attributes = _classes.get(clazz)->defaultAttributes();

    // Button control

    vector<Button *> selButtons {
        _binding.btnSel1.get(),
        _binding.btnSel2.get(),
        _binding.btnSel3.get(),
        _binding.btnSel4.get(),
        _binding.btnSel5.get(),
        _binding.btnSel6.get(),
    };
    Button &selButton = *selButtons[index];
    setButtonColors(selButton);

    glm::ivec2 center;
    selButton.extent().getCenter(center.x, center.y);

    Control::Extent extent;
    extent.left = center.x - _defaultButtonSize.x / 2;
    extent.top = center.y - _defaultButtonSize.y / 2;
    extent.width = _defaultButtonSize.x;
    extent.height = _defaultButtonSize.y;
    selButton.setExtent(move(extent));
    selButton.setOnClick([this, character]() {
        _charGen.setCharacter(character);
        _charGen.openQuickOrCustom();
    });
    selButton.setOnFocusChanged([this, index](bool focus) {
        onClassButtonFocusChanged(index, focus);
    });

    // 3D control

    float aspect = extent.width / static_cast<float>(extent.height);
    unique_ptr<SceneGraph> scene(SceneBuilder(
                                     _options,
                                     _context,
                                     _features,
                                     _materials,
                                     _meshes,
                                     _pbrIbl,
                                     _shaders,
                                     _textures)
                                     .aspect(aspect)
                                     .depth(0.1f, 10.0f)
                                     .modelSupplier([&](SceneGraph &sceneGraph) { return getCharacterModel(appearance, sceneGraph); })
                                     .modelScale(kModelScale)
                                     .cameraFromModelNode("camerahook")
                                     .lightingRefFromModelNode("cgbody_light")
                                     .build());

    vector<Label *> threeDModels {
        _binding.threeDModel1.get(),
        _binding.threeDModel2.get(),
        _binding.threeDModel3.get(),
        _binding.threeDModel4.get(),
        _binding.threeDModel5.get(),
        _binding.threeDModel6.get(),
    };
    threeDModels[index]->setScene(move(scene));

    ClassButton classButton;
    classButton.control = &selButton;
    classButton.center = center;
    classButton.character = character;
    _classButtons.push_back(move(classButton));
}

vector<Portrait> ClassSelection::getPCPortraitsByGender(Gender gender) {
    vector<Portrait> result;
    int sex = gender == Gender::Female ? 1 : 0;
    for (auto &portrait : _portraits.portraits()) {
        if (portrait.forPC && portrait.sex == sex) {
            result.push_back(portrait);
        }
    }
    return move(result);
}

int ClassSelection::getRandomCharacterAppearance(Gender gender, ClassType clazz) {
    int result = 0;
    vector<Portrait> portraits(getPCPortraitsByGender(gender));
    int portraitIdx = random(0, static_cast<int>(portraits.size()) - 1);
    const Portrait &portrait = portraits[portraitIdx];

    switch (clazz) {
    case ClassType::Scoundrel:
    case ClassType::JediConsular:
        result = portrait.appearanceS;
        break;
    case ClassType::Soldier:
    case ClassType::JediGuardian:
        result = portrait.appearanceL;
        break;
    default:
        result = portrait.appearanceNumber;
        break;
    }

    return result;
}

shared_ptr<ModelSceneNode> ClassSelection::getCharacterModel(int appearance, SceneGraph &sceneGraph) {
    auto objectFactory = make_unique<ObjectFactory>(
        _actionFactory,
        _classes,
        _combat,
        _footstepSounds,
        _party,
        _portraits,
        _reputes,
        _scriptRunner,
        _soundSets,
        _surfaces,
        _audioFiles,
        _audioPlayer,
        _context,
        _meshes,
        _models,
        _shaders,
        _textures,
        _walkmeshes,
        _window,
        _resources,
        _strings,
        _twoDas,
        sceneGraph);
    objectFactory->setGame(*_game);

    shared_ptr<Creature> character(objectFactory->newCreature());
    character->setFacing(-glm::half_pi<float>());
    character->setAppearance(appearance);
    character->equip("g_a_clothes01");
    character->loadAppearance();
    character->sceneNode()->setCullable(false);
    character->updateModelAnimation();

    auto model = sceneGraph.newModel(_models.get("cgbody_light"), ModelUsage::GUI);
    model->attach("cgbody_light", character->sceneNode());

    return move(model);
}

void ClassSelection::setButtonColors(Control &control) {
    control.setBorderColor(_game->getGUIColorBase());
    control.setHilightColor(_game->getGUIColorHilight());
}

void ClassSelection::setClassButtonEnlarged(int index, bool enlarged) {
    ClassButton &button = _classButtons[index];

    Control::Extent extent;
    extent.width = static_cast<int>(enlarged ? _enlargedButtonSize.x : _defaultButtonSize.x);
    extent.height = static_cast<int>(enlarged ? _enlargedButtonSize.y : _defaultButtonSize.y);
    extent.left = static_cast<int>(button.center.x - 0.5f * extent.width);
    extent.top = static_cast<int>(button.center.y - 0.5f * extent.height);

    Control &control = *button.control;
    control.setExtent(move(extent));
}

void ClassSelection::onClassButtonFocusChanged(int index, bool focus) {
    if (focus) {
        setClassButtonEnlarged(index, true);
    } else {
        setClassButtonEnlarged(index, false);
    }

    ClassButton &button = _classButtons[index];
    ClassType clazz = button.character.attributes.getEffectiveClass();

    string classText(_strings.get(g_genderStrRefs[button.character.gender]));
    classText += " " + _classes.get(clazz)->name();
    _binding.lblClass->setTextMessage(classText);

    string descText(_strings.get(g_classDescStrRefs[clazz]));
    _binding.lblDesc->setTextMessage(descText);
}

} // namespace game

} // namespace reone
