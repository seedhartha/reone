/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../../../system/gui/scenebuilder.h"
#include "../../../system/resource/resources.h"

#include "../../characters.h"
#include "../../object/creature.h"

#include "../colors.h"

#include "chargen.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

namespace game {

static const float kModelScale = 1.05f;
static const float kModelOffsetY = 0.9f;

static map<Gender, int> g_genderStrRefs {
    { Gender::Male, 646 },
    { Gender::Female, 647 }
};

static map<ClassType, int> g_classStrRefs {
    { ClassType::Scout, 133 },
    { ClassType::Soldier, 134 },
    { ClassType::Scoundrel, 135 },
    { ClassType::JediGuardian, 353 },
    { ClassType::JediConsular, 354 },
    { ClassType::JediSentinel, 355 }
};

static map<ClassType, int> g_classDescStrRefs {
    { ClassType::Scoundrel, 32109 },
    { ClassType::Scout, 32110 },
    { ClassType::Soldier, 32111 },
    { ClassType::JediConsular, 48031 },
    { ClassType::JediSentinel, 48032 },
    { ClassType::JediGuardian, 48033 }
};

ClassSelection::ClassSelection(Game *game, CharacterGeneration *charGen, GameVersion version, const GraphicsOptions &opts) :
    GUI(version, opts),
    _game(game),
    _charGen(charGen) {

    _resRef = getResRef("classsel");

    switch (version) {
        case GameVersion::TheSithLords:
            _resolutionX = 800;
            _resolutionY = 600;
            break;
        default:
            _backgroundType = BackgroundType::Menu;
            break;
    }
}

void ClassSelection::load() {
    GUI::load();
    configureClassButtons();
    configureClassModels();

    Control &backButton = getControl("BTN_BACK");
    setButtonColors(backButton);
}

void ClassSelection::configureClassButtons() {
    int x, y;

    Control &button1 = getControl("BTN_SEL1");
    setButtonColors(button1);
    button1.extent().getCenter(x, y);
    _classButtons.push_back({ &button1, glm::vec2(x, y), randomCharacter(Gender::Male, _version == GameVersion::KotOR ? ClassType::Scoundrel : ClassType::JediConsular) });

    Control &button2 = getControl("BTN_SEL2");
    setButtonColors(button2);
    button2.extent().getCenter(x, y);
    _classButtons.push_back({ &button2, glm::vec2(x, y), randomCharacter(Gender::Male, _version == GameVersion::KotOR ? ClassType::Scout : ClassType::JediSentinel) });

    Control &button3 = getControl("BTN_SEL3");
    setButtonColors(button3);
    button3.extent().getCenter(x, y);
    _classButtons.push_back({ &button3, glm::vec2(x, y), randomCharacter(Gender::Male, _version == GameVersion::KotOR ? ClassType::Soldier : ClassType::JediGuardian) });

    Control &button4 = getControl("BTN_SEL4");
    setButtonColors(button4);
    button4.extent().getCenter(x, y);
    _classButtons.push_back({ &button4, glm::vec2(x, y), randomCharacter(Gender::Female, _version == GameVersion::KotOR ? ClassType::Soldier : ClassType::JediGuardian) });

    Control &button5 = getControl("BTN_SEL5");
    setButtonColors(button5);
    button5.extent().getCenter(x, y);
    _classButtons.push_back({ &button5, glm::vec2(x, y), randomCharacter(Gender::Female, _version == GameVersion::KotOR ? ClassType::Scout : ClassType::JediSentinel) });

    Control &button6 = getControl("BTN_SEL6");
    setButtonColors(button6);
    button6.extent().getCenter(x, y);
    _classButtons.push_back({ &button6, glm::vec2(x, y), randomCharacter(Gender::Female, _version == GameVersion::KotOR ? ClassType::Scoundrel : ClassType::JediConsular) });

    _enlargedButtonSize = glm::vec2(button1.extent().width, button1.extent().height);
    _defaultButtonSize = glm::vec2(button2.extent().width, button2.extent().height);

    setClassButtonEnlarged(0, false);
}

void ClassSelection::setButtonColors(Control &control) {
    Control::Border border(control.border());
    border.color = getBaseColor(_version);
    control.setBorder(move(border));

    Control::Border hilight(control.hilight());
    hilight.color = getHilightColor(_version);
    control.setHilight(move(hilight));
}

void ClassSelection::setClassButtonEnlarged(int index, bool enlarged) {
    ClassButton &button = _classButtons[index];
    Control &control(*button.control);
    Control::Extent extent(control.extent());

    extent.width = static_cast<int>(enlarged ? _enlargedButtonSize.x : _defaultButtonSize.x);
    extent.height = static_cast<int>(enlarged ? _enlargedButtonSize.y : _defaultButtonSize.y);
    extent.left = static_cast<int>(button.center.x - 0.5f * extent.width);
    extent.top = static_cast<int>(button.center.y - 0.5f * extent.height);

    control.setExtent(move(extent));
}

void ClassSelection::configureClassModels() {
    switch (_version) {
        case GameVersion::TheSithLords:
            configureClassModel(0, Gender::Male, ClassType::JediConsular);
            configureClassModel(1, Gender::Male, ClassType::JediSentinel);
            configureClassModel(2, Gender::Male, ClassType::JediGuardian);
            configureClassModel(3, Gender::Female, ClassType::JediGuardian);
            configureClassModel(4, Gender::Female, ClassType::JediSentinel);
            configureClassModel(5, Gender::Female, ClassType::JediConsular);
            break;
        default:
            configureClassModel(0, Gender::Male, ClassType::Scoundrel);
            configureClassModel(1, Gender::Male, ClassType::Scout);
            configureClassModel(2, Gender::Male, ClassType::Soldier);
            configureClassModel(3, Gender::Female, ClassType::Soldier);
            configureClassModel(4, Gender::Female, ClassType::Scout);
            configureClassModel(5, Gender::Female, ClassType::Scoundrel);
            break;
    }
}

void ClassSelection::configureClassModel(int index, Gender gender, ClassType clazz) {
    Control::Extent extent;
    extent.left = _classButtons[index].center.x - _defaultButtonSize.x / 2;
    extent.top = _classButtons[index].center.y - _defaultButtonSize.y / 2;
    extent.width = _defaultButtonSize.x;
    extent.height = _defaultButtonSize.y;

    float aspect = extent.width / static_cast<float>(extent.height);

    glm::mat4 cameraTransform(1.0f);
    cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, 1.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraTransform = glm::rotate(cameraTransform, glm::pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f));

    unique_ptr<Control::Scene3D> scene(SceneBuilder(_gfxOpts)
        .aspect(aspect)
        .depth(0.1f, 10.0f)
        .modelSupplier([this, &index](SceneGraph &sceneGraph) { return getCharacterModel(_classButtons[index].config, sceneGraph); })
        .modelScale(kModelScale)
        .modelOffset(glm::vec2(0.0f, kModelOffsetY))
        .cameraTransform(cameraTransform)
        .ambientLightColor(glm::vec3(1.0f))
        .build());

    Control &control = getControl("3D_MODEL" + to_string(index + 1));
    control.setExtent(extent);
    control.setScene3D(move(scene));
}

shared_ptr<ModelSceneNode> ClassSelection::getCharacterModel(const CreatureConfiguration &config, SceneGraph &sceneGraph) {
    unique_ptr<ObjectFactory> objectFactory(new ObjectFactory(_version, _game, &sceneGraph, _gfxOpts));

    unique_ptr<Creature> creature(objectFactory->newCreature());
    creature->load(config);

    return creature->model();
}

void ClassSelection::onFocusChanged(const string &control, bool focus) {
    int idx = getClassButtonIndexByTag(control);
    if (idx == -1) return;

    if (focus) {
        setClassButtonEnlarged(idx, true);
    } else {
        setClassButtonEnlarged(idx, false);
    }

    ClassButton &button = _classButtons[idx];

    string classText(Resources.getString(g_genderStrRefs[button.config.gender]).text);
    classText += " " + Resources.getString(g_classStrRefs[button.config.clazz]).text;

    string descText(Resources.getString(g_classDescStrRefs[button.config.clazz]).text);

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
    int idx = getClassButtonIndexByTag(control);
    if (idx != -1) {
        _charGen->setCharacter(_classButtons[idx].config);
        _charGen->openQuickOrCustom();
        return;
    }
    if (control == "BTN_BACK") {
        _charGen->cancel();
    }
}

} // namespace game

} // namespace reone
