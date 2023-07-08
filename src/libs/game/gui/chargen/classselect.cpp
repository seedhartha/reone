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

#include "reone/game/gui/chargen/classselect.h"

#include "reone/graphics/di/services.h"
#include "reone/graphics/models.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/resource/strings.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/system/randomutil.h"

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/game/object/creature.h"
#include "reone/game/object/factory.h"
#include "reone/game/party.h"
#include "reone/game/portraits.h"
#include "reone/game/types.h"

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 1.1f;

static std::map<Gender, int> g_genderStrRefs {
    {Gender::Male, 646},
    {Gender::Female, 647}};

static std::map<ClassType, int> g_classDescStrRefs {
    {ClassType::Scoundrel, 32109},
    {ClassType::Scout, 32110},
    {ClassType::Soldier, 32111},
    {ClassType::JediConsular, 48031},
    {ClassType::JediSentinel, 48032},
    {ClassType::JediGuardian, 48033}};

void ClassSelection::onGUILoaded() {
    if (!_game.isTSL()) {
        loadBackground(BackgroundType::Menu);
    }

    bindControls();

    setupClassButtons();
    setButtonColors(*_binding.btnBack);

    _binding.btnBack->setOnClick([this]() {
        _charGen.cancel();
    });
}

void ClassSelection::bindControls() {
    _binding.btnBack = findControl<Button>("BTN_BACK");
    _binding.btnSel1 = findControl<Button>("BTN_SEL1");
    _binding.btnSel2 = findControl<Button>("BTN_SEL2");
    _binding.btnSel3 = findControl<Button>("BTN_SEL3");
    _binding.btnSel4 = findControl<Button>("BTN_SEL4");
    _binding.btnSel5 = findControl<Button>("BTN_SEL5");
    _binding.btnSel6 = findControl<Button>("BTN_SEL6");
    _binding.lblClass = findControl<Label>("LBL_CLASS");
    _binding.lblDesc = findControl<Label>("LBL_DESC");
    _binding.threeDModel1 = findControl<Label>("3D_MODEL1");
    _binding.threeDModel2 = findControl<Label>("3D_MODEL2");
    _binding.threeDModel3 = findControl<Label>("3D_MODEL3");
    _binding.threeDModel4 = findControl<Label>("3D_MODEL4");
    _binding.threeDModel5 = findControl<Label>("3D_MODEL5");
    _binding.threeDModel6 = findControl<Label>("3D_MODEL6");
}

void ClassSelection::setupClassButtons() {
    _enlargedButtonSize = glm::vec2(_binding.btnSel1->extent().width, _binding.btnSel1->extent().height);
    _defaultButtonSize = glm::vec2(_binding.btnSel2->extent().width, _binding.btnSel2->extent().height);

    setupClassButton(0, Gender::Male, !_game.isTSL() ? ClassType::Scoundrel : ClassType::JediConsular);
    setupClassButton(1, Gender::Male, !_game.isTSL() ? ClassType::Scout : ClassType::JediSentinel);
    setupClassButton(2, Gender::Male, !_game.isTSL() ? ClassType::Soldier : ClassType::JediGuardian);
    setupClassButton(3, Gender::Female, !_game.isTSL() ? ClassType::Soldier : ClassType::JediGuardian);
    setupClassButton(4, Gender::Female, !_game.isTSL() ? ClassType::Scout : ClassType::JediSentinel);
    setupClassButton(5, Gender::Female, !_game.isTSL() ? ClassType::Scoundrel : ClassType::JediConsular);

    setClassButtonEnlarged(0, false);
}

void ClassSelection::setupClassButton(int index, Gender gender, ClassType clazz) {
    int appearance = getRandomCharacterAppearance(gender, clazz);

    Character character;
    character.gender = gender;
    character.appearance = appearance;
    character.attributes = _services.game.classes.get(clazz)->defaultAttributes();

    // Button control

    std::vector<Button *> selButtons {
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
    selButton.setExtent(std::move(extent));
    selButton.setOnClick([this, character]() {
        _charGen.setCharacter(character);
        _charGen.openQuickOrCustom();
    });
    selButton.setOnFocusChanged([this, index](bool focus) {
        onClassButtonFocusChanged(index, focus);
    });

    // 3D control

    std::string sceneName(kSceneClassSelect);
    sceneName += "." + std::to_string(index);

    auto &sceneGraph = _services.scene.graphs.get(sceneName);
    float aspect = extent.width / static_cast<float>(extent.height);

    SceneInitializer(sceneGraph)
        .aspect(aspect)
        .depth(kDefaultClipPlaneNear, 10.0f)
        .modelSupplier([&](ISceneGraph &sceneGraph) { return getCharacterModel(appearance, sceneGraph); })
        .modelScale(kModelScale)
        .cameraFromModelNode("camerahook")
        .invoke();

    std::vector<Label *> threeDModels {
        _binding.threeDModel1.get(),
        _binding.threeDModel2.get(),
        _binding.threeDModel3.get(),
        _binding.threeDModel4.get(),
        _binding.threeDModel5.get(),
        _binding.threeDModel6.get(),
    };
    threeDModels[index]->setSceneName(sceneName);

    ClassButton classButton;
    classButton.control = &selButton;
    classButton.center = center;
    classButton.character = character;
    _classButtons.push_back(std::move(classButton));
}

std::vector<Portrait> ClassSelection::getPCPortraitsByGender(Gender gender) {
    std::vector<Portrait> result;
    int sex = gender == Gender::Female ? 1 : 0;
    for (auto &portrait : _services.game.portraits.portraits()) {
        if (portrait.forPC && portrait.sex == sex) {
            result.push_back(portrait);
        }
    }
    return std::move(result);
}

int ClassSelection::getRandomCharacterAppearance(Gender gender, ClassType clazz) {
    int result = 0;
    std::vector<Portrait> portraits(getPCPortraitsByGender(gender));
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

std::shared_ptr<ModelSceneNode> ClassSelection::getCharacterModel(int appearance, ISceneGraph &sceneGraph) {
    std::shared_ptr<Creature> character = _game.objectFactory().newCreature(sceneGraph.name());
    _game.addObject(character);
    character->setFacing(-glm::half_pi<float>());
    character->setAppearance(appearance);
    character->equip("g_a_clothes01");
    character->loadAppearance();
    character->sceneNode()->setCullable(false);
    character->updateModelAnimation();

    auto model = sceneGraph.newModel(*_services.graphics.models.get("cgbody_light"), ModelUsage::GUI);
    model->attach("cgbody_light", *character->sceneNode());

    return std::move(model);
}

void ClassSelection::setButtonColors(Control &control) {
    control.setBorderColor(_baseColor);
    control.setHilightColor(_hilightColor);
}

void ClassSelection::setClassButtonEnlarged(int index, bool enlarged) {
    ClassButton &button = _classButtons[index];

    Control::Extent extent;
    extent.width = static_cast<int>(enlarged ? _enlargedButtonSize.x : _defaultButtonSize.x);
    extent.height = static_cast<int>(enlarged ? _enlargedButtonSize.y : _defaultButtonSize.y);
    extent.left = static_cast<int>(button.center.x - 0.5f * extent.width);
    extent.top = static_cast<int>(button.center.y - 0.5f * extent.height);

    Control &control = *button.control;
    control.setExtent(std::move(extent));
}

void ClassSelection::onClassButtonFocusChanged(int index, bool focus) {
    if (focus) {
        setClassButtonEnlarged(index, true);
    } else {
        setClassButtonEnlarged(index, false);
    }

    ClassButton &button = _classButtons[index];
    ClassType clazz = button.character.attributes.getEffectiveClass();

    std::string classText(_services.resource.strings.get(g_genderStrRefs[button.character.gender]));
    classText += " " + _services.game.classes.get(clazz)->name();
    _binding.lblClass->setTextMessage(classText);

    std::string descText(_services.resource.strings.get(g_classDescStrRefs[clazz]));
    _binding.lblDesc->setTextMessage(descText);
}

} // namespace game

} // namespace reone
