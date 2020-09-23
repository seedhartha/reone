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

#include "classsel.h"

#include "../../resources/resources.h"

#include "../characters.h"
#include "../object/creature.h"

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

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

ClassSelectionGui::ClassSelectionGui(ObjectFactory *objectFactory, const GraphicsOptions &opts) :
    GUI(opts), _objectFactory(objectFactory) {

    assert(_objectFactory);
}

void ClassSelectionGui::load(GameVersion version) {
    string resRef;
    BackgroundType background;

    switch (version) {
        case GameVersion::TheSithLords:
            _resolutionX = 800;
            _resolutionY = 600;
            resRef = "classsel_p";
            background = BackgroundType::None;
            break;
        default:
            resRef = "classsel";
            background = BackgroundType::Menu;
            break;
    }

    GUI::load(resRef, background);
    _version = version;

    configureClassButtons();
    configureClassModels();

    Control &backButton = getControl("BTN_BACK");
    setButtonColors(backButton);
}

void ClassSelectionGui::configureClassButtons() {
    int x, y;

    Control &button1 = getControl("BTN_SEL1");
    setButtonColors(button1);
    button1.extent().getCenter(x, y);
    _classButtons.push_back({ &button1, glm::vec2(x, y), Gender::Male, _version == GameVersion::KotOR ? ClassType::Scoundrel : ClassType::JediConsular });

    Control &button2 = getControl("BTN_SEL2");
    setButtonColors(button2);
    button2.extent().getCenter(x, y);
    _classButtons.push_back({ &button2, glm::vec2(x, y), Gender::Male, _version == GameVersion::KotOR ? ClassType::Scout : ClassType::JediSentinel });

    Control &button3 = getControl("BTN_SEL3");
    setButtonColors(button3);
    button3.extent().getCenter(x, y);
    _classButtons.push_back({ &button3, glm::vec2(x, y), Gender::Male, _version == GameVersion::KotOR ? ClassType::Soldier : ClassType::JediGuardian });

    Control &button4 = getControl("BTN_SEL4");
    setButtonColors(button4);
    button4.extent().getCenter(x, y);
    _classButtons.push_back({ &button4, glm::vec2(x, y), Gender::Female, _version == GameVersion::KotOR ? ClassType::Soldier : ClassType::JediGuardian });

    Control &button5 = getControl("BTN_SEL5");
    setButtonColors(button5);
    button5.extent().getCenter(x, y);
    _classButtons.push_back({ &button5, glm::vec2(x, y), Gender::Female, _version == GameVersion::KotOR ? ClassType::Scout : ClassType::JediSentinel });

    Control &button6 = getControl("BTN_SEL6");
    setButtonColors(button6);
    button6.extent().getCenter(x, y);
    _classButtons.push_back({ &button6, glm::vec2(x, y), Gender::Female, _version == GameVersion::KotOR ? ClassType::Scoundrel : ClassType::JediConsular });

    _enlargedButtonSize = glm::vec2(button1.extent().width, button1.extent().height);
    _defaultButtonSize = glm::vec2(button2.extent().width, button2.extent().height);

    setClassButtonEnlarged(0, false);
}

void ClassSelectionGui::setButtonColors(Control &control) {
    Control::Border border(control.border());
    border.color = _version == GameVersion::KotOR ? getKotorBaseColor() : getTslBaseColor();
    control.setBorder(move(border));

    Control::Border hilight(control.hilight());
    hilight.color = _version == GameVersion::KotOR ? getKotorHilightColor() : getTslHilightColor();
    control.setHilight(move(hilight));
}

void ClassSelectionGui::setClassButtonEnlarged(int index, bool enlarged) {
    assert(index >= 0 && index < _classButtons.size());

    ClassButton &button = _classButtons[index];
    Control &control(*button.control);
    Control::Extent extent(control.extent());

    extent.width = static_cast<int>(enlarged ? _enlargedButtonSize.x : _defaultButtonSize.x);
    extent.height = static_cast<int>(enlarged ? _enlargedButtonSize.y : _defaultButtonSize.y);
    extent.left = static_cast<int>(button.center.x - 0.5f * extent.width);
    extent.top = static_cast<int>(button.center.y - 0.5f * extent.height);

    control.setExtent(move(extent));
}

void ClassSelectionGui::configureClassModels() {
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

void ClassSelectionGui::configureClassModel(int index, Gender gender, ClassType clazz) {
    Control &control = getControl("3D_MODEL" + to_string(index + 1));
    const Control::Extent &extent = control.extent();

    unique_ptr<Creature> creature(_objectFactory->newCreature());
    creature->load(randomCharacter(gender, clazz));

    int frameHeight = _defaultButtonSize.y;
    int x = extent.left + extent.width / 2;
    int y = extent.top + (extent.height + frameHeight) / 2 - 12;

    glm::mat4 transform(1.0f);
    transform = glm::translate(transform, glm::vec3(x, y, 0.0f));
    transform = glm::rotate(transform, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::scale(transform, glm::vec3(frameHeight / 2.0f));

    Control::Scene3D scene;
    scene.model = creature->model();
    scene.transform = move(transform);

    control.setScene3D(scene);
}

void ClassSelectionGui::onFocusChanged(const string &control, bool focus) {
    int idx = getClassButtonIndexByTag(control);
    if (idx == -1) return;

    if (focus) {
        setClassButtonEnlarged(idx, true);
    } else {
        setClassButtonEnlarged(idx, false);
    }

    ClassButton &button = _classButtons[idx];

    string classText(Resources.getString(g_genderStrRefs[button.gender]).text);
    classText += " " + Resources.getString(g_classStrRefs[button.clazz]).text;

    string descText(Resources.getString(g_classDescStrRefs[button.clazz]).text);

    getControl("LBL_CLASS").setTextMessage(classText);
    getControl("LBL_DESC").setTextMessage(descText);
}

int ClassSelectionGui::getClassButtonIndexByTag(const string &tag) const {
    for (int i = 0; i < _classButtons.size(); ++i) {
        if (_classButtons[i].control->tag() == tag) {
            return i;
        }
    }

    return -1;
}

void ClassSelectionGui::onClick(const string &control) {
    int idx = getClassButtonIndexByTag(control);
    if (idx != -1) {
        ClassButton &button = _classButtons[idx];
        if (_onClassSelected) {
            CreatureConfiguration character;
            character.gender = button.gender;
            character.clazz = button.clazz;
            _onClassSelected(move(character));
        }
        return;
    }
    if (control == "BTN_BACK") {
        if (_onCancel) _onCancel();
    }
}

void ClassSelectionGui::setOnClassSelected(const function<void(const CreatureConfiguration &)> &fn) {
    _onClassSelected = fn;
}

void ClassSelectionGui::setOnCancel(const function<void()> &fn) {
    _onCancel = fn;
}

} // namespace game

} // namespace reone
