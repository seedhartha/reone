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

#include "../../../gui/scenebuilder.h"
#include "../../../graphics/model/models.h"
#include "../../../resource/strings.h"

#include "../../characterutil.h"
#include "../../d20/classes.h"
#include "../../game.h"
#include "../../gameidutil.h"
#include "../../object/creature.h"

#include "../colorutil.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static constexpr float kModelScale = 1.1f;

static map<Gender, int> g_genderStrRefs {
    { Gender::Male, 646 },
    { Gender::Female, 647 }
};

static map<ClassType, int> g_classDescStrRefs {
    { ClassType::Scoundrel, 32109 },
    { ClassType::Scout, 32110 },
    { ClassType::Soldier, 32111 },
    { ClassType::JediConsular, 48031 },
    { ClassType::JediSentinel, 48032 },
    { ClassType::JediGuardian, 48033 }
};

ClassSelection::ClassSelection(Game *game) : GameGUI(game) {
    _resRef = getResRef("classsel");

    if (game->gameId() == GameID::KotOR) {
        loadBackground(BackgroundType::Menu);
    }

    initForGame();
}

void ClassSelection::load() {
    GUI::load();
    setupClassButtons();

    Control &backButton = getControl("BTN_BACK");
    setButtonColors(backButton);
}

void ClassSelection::setupClassButtons() {
    Control &button1 = getControl("BTN_SEL1");
    Control &button2 = getControl("BTN_SEL2");
    _enlargedButtonSize = glm::vec2(button1.extent().width, button1.extent().height);
    _defaultButtonSize = glm::vec2(button2.extent().width, button2.extent().height);

    setupClassButton(0, Gender::Male, _game->gameId() == GameID::KotOR ? ClassType::Scoundrel : ClassType::JediConsular);
    setupClassButton(1, Gender::Male, _game->gameId() == GameID::KotOR ? ClassType::Scout : ClassType::JediSentinel);
    setupClassButton(2, Gender::Male, _game->gameId() == GameID::KotOR ? ClassType::Soldier : ClassType::JediGuardian);
    setupClassButton(3, Gender::Female, _game->gameId() == GameID::KotOR ? ClassType::Soldier : ClassType::JediGuardian);
    setupClassButton(4, Gender::Female, _game->gameId() == GameID::KotOR ? ClassType::Scout : ClassType::JediSentinel);
    setupClassButton(5, Gender::Female, _game->gameId() == GameID::KotOR ? ClassType::Scoundrel : ClassType::JediConsular);

    setClassButtonEnlarged(0, false);
}

void ClassSelection::setupClassButton(int index, Gender gender, ClassType clazz) {
    int appearance = getRandomCharacterAppearance(gender, clazz);


    // Button control

    Control &controlButton = getControl("BTN_SEL" + to_string(index + 1));
    setButtonColors(controlButton);

    glm::ivec2 center;
    controlButton.extent().getCenter(center.x, center.y);

    Control::Extent extent;
    extent.left = center.x - _defaultButtonSize.x / 2;
    extent.top = center.y - _defaultButtonSize.y / 2;
    extent.width = _defaultButtonSize.x;
    extent.height = _defaultButtonSize.y;
    controlButton.setExtent(move(extent));


    // 3D control

    float aspect = extent.width / static_cast<float>(extent.height);
    unique_ptr<SceneGraph> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier([&](SceneGraph &sceneGraph) { return getCharacterModel(appearance, sceneGraph); })
        .modelScale(kModelScale)
        .cameraFromModelNode("camerahook")
        .ambientLightColor(glm::vec3(0.2f))
        .build());

    Control &control3d = getControl("3D_MODEL" + to_string(index + 1));
    control3d.setScene(move(scene));


    ClassButton classButton;
    classButton.control = &controlButton;
    classButton.center = center;
    classButton.character.gender = gender;
    classButton.character.appearance = appearance;
    classButton.character.attributes = Classes::instance().get(clazz)->defaultAttributes();
    _classButtons.push_back(move(classButton));
}

shared_ptr<ModelSceneNode> ClassSelection::getCharacterModel(int appearance, SceneGraph &sceneGraph) {
    auto objectFactory = make_unique<ObjectFactory>(_game, &sceneGraph);

    shared_ptr<Creature> character(objectFactory->newCreature());
    character->setFacing(-glm::half_pi<float>());
    character->setAppearance(appearance);
    character->equip("g_a_clothes01");
    character->loadAppearance();
    character->sceneNode()->setCullable(false);
    character->updateModelAnimation();

    auto model = make_shared<ModelSceneNode>(Models::instance().get("cgbody_light"), ModelUsage::GUI, &sceneGraph);
    model->attach("cgbody_light", character->sceneNode());

    return move(model);
}

void ClassSelection::setButtonColors(Control &control) {
    control.setBorderColor(getBaseColor(_game->gameId()));
    control.setHilightColor(getHilightColor(_game->gameId()));
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

void ClassSelection::onFocusChanged(const string &control, bool focus) {
    GameGUI::onFocusChanged(control, focus);

    int idx = getClassButtonIndexByTag(control);
    if (idx == -1) return;

    if (focus) {
        setClassButtonEnlarged(idx, true);
    } else {
        setClassButtonEnlarged(idx, false);
    }

    ClassButton &button = _classButtons[idx];
    ClassType clazz = button.character.attributes.getEffectiveClass();

    string classText(Strings::instance().get(g_genderStrRefs[button.character.gender]));
    classText += " " + Classes::instance().get(clazz)->name();

    string descText(Strings::instance().get(g_classDescStrRefs[clazz]));

    getControl("LBL_CLASS").setTextMessage(classText);
    getControl("LBL_DESC").setTextMessage(descText);
}

int ClassSelection::getClassButtonIndexByTag(const string &tag) const {
    for (int i = 0; i < _classButtons.size(); ++i) {
        if (_classButtons[i].control->tag() == tag) {
            return i;
        }
    }

    return -1;
}

void ClassSelection::onClick(const string &control) {
    GameGUI::onClick(control);

    CharacterGeneration &charGen = _game->characterGeneration();
    int idx = getClassButtonIndexByTag(control);
    if (idx != -1) {
        charGen.setCharacter(_classButtons[idx].character);
        charGen.openQuickOrCustom();
        return;
    }
    if (control == "BTN_BACK") {
        charGen.cancel();
    }
}

} // namespace game

} // namespace reone
