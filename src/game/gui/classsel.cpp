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

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

static map<ClassType, int> g_classStrRefs {
    { ClassType::Scout, 133 },
    { ClassType::Soldier, 134 },
    { ClassType::Scoundrel, 135 },
    { ClassType::JediGuardian, 353 },
    { ClassType::JediConsular, 354 },
    { ClassType::JediSentinel, 355 }
};

ClassSelectionGui::ClassSelectionGui(const GraphicsOptions &opts) : GUI(opts) {
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

void ClassSelectionGui::onFocusChanged(const string &control, bool focus) {
    int idx = getClassButtonIndexByTag(control);
    if (idx == -1) return;

    if (focus) {
        setClassButtonEnlarged(idx, true);
    } else {
        setClassButtonEnlarged(idx, false);
    }

    ClassButton &button = _classButtons[idx];
    string text(ResMan.getString(g_classStrRefs[button.clazz]).text);
    getControl("LBL_CLASS").setTextMessage(text);
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
