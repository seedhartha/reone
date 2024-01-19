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

#include "reone/game/d20/classes.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/gui/chargen.h"
#include "reone/game/object/creature.h"
#include "reone/game/party.h"
#include "reone/game/portraits.h"
#include "reone/game/types.h"
#include "reone/graphics/di/services.h"
#include "reone/gui/control/button.h"
#include "reone/gui/control/label.h"
#include "reone/gui/sceneinitializer.h"
#include "reone/resource/provider/models.h"
#include "reone/resource/strings.h"
#include "reone/scene/di/services.h"
#include "reone/scene/graphs.h"
#include "reone/system/randomutil.h"

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
    setButtonColors(*_controls.BTN_BACK);

    _controls.BTN_BACK->setOnClick([this]() {
        _charGen.cancel();
    });
}

void ClassSelection::setupClassButtons() {
    _enlargedButtonSize = glm::vec2(_controls.BTN_SEL1->extent().width, _controls.BTN_SEL1->extent().height);
    _defaultButtonSize = glm::vec2(_controls.BTN_SEL2->extent().width, _controls.BTN_SEL2->extent().height);

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
        _controls.BTN_SEL1.get(),
        _controls.BTN_SEL2.get(),
        _controls.BTN_SEL3.get(),
        _controls.BTN_SEL4.get(),
        _controls.BTN_SEL5.get(),
        _controls.BTN_SEL6.get(),
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
    selButton.setOnSelectionChanged([this, index](bool selected) {
        onClassButtonSelectionChanged(index, selected);
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

    std::vector<Label *> THREE_D_MODELs {
        _controls.THREE_D_MODEL1.get(),
        _controls.THREE_D_MODEL2.get(),
        _controls.THREE_D_MODEL3.get(),
        _controls.THREE_D_MODEL4.get(),
        _controls.THREE_D_MODEL5.get(),
        _controls.THREE_D_MODEL6.get(),
    };
    THREE_D_MODELs[index]->setSceneName(sceneName);

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
    return result;
}

int ClassSelection::getRandomCharacterAppearance(Gender gender, ClassType clazz) {
    int result = 0;
    std::vector<Portrait> portraits(getPCPortraitsByGender(gender));
    int portraitIdx = randomInt(0, static_cast<int>(portraits.size()) - 1);
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
    std::shared_ptr<Creature> character = _game.newCreature(sceneGraph.name());
    character->setFacing(-glm::half_pi<float>());
    character->setAppearance(appearance);
    character->equip("g_a_clothes01");
    character->loadAppearance();
    character->updateModelAnimation();

    auto model = sceneGraph.newModel(*_services.resource.models.get("cgbody_light"), ModelUsage::GUI);
    model->attach("cgbody_light", *character->sceneNode());

    return model;
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

void ClassSelection::onClassButtonSelectionChanged(int index, bool selected) {
    if (selected) {
        setClassButtonEnlarged(index, true);
    } else {
        setClassButtonEnlarged(index, false);
    }

    ClassButton &button = _classButtons[index];
    ClassType clazz = button.character.attributes.getEffectiveClass();

    std::string classText(_services.resource.strings.getText(g_genderStrRefs[button.character.gender]));
    classText += " " + _services.game.classes.get(clazz)->name();
    _controls.LBL_CLASS->setTextMessage(classText);

    std::string descText(_services.resource.strings.getText(g_classDescStrRefs[clazz]));
    _controls.LBL_DESC->setTextMessage(descText);
}

} // namespace game

} // namespace reone
