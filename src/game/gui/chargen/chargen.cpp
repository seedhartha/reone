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

#include "chargen.h"

#include <stdexcept>

using namespace std;

using namespace reone::gui;
using namespace reone::render;
using namespace reone::resource;

namespace reone {

namespace game {

CharacterGeneration::CharacterGeneration(GameVersion version, const GraphicsOptions &opts) : GUI(version, opts) {
    _resRef = getResRef("maincg");
    _backgroundType = BackgroundType::Menu;

    if (version == GameVersion::TheSithLords) {
        _resolutionX = 800;
        _resolutionY = 600;
    }
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
    setControlText("LBL_CLASS", "");

    loadClassSelection();
    loadQuickOrCustom();
    loadQuickCharacterGeneration();
    loadPortraitSelection();
    loadNameGui();
}

void CharacterGeneration::loadClassSelection() {
    _classSelection = make_unique<ClassSelection>(_version, _gfxOpts);
    _classSelection->load();
    _classSelection->setOnClassSelected([this](const CreatureConfiguration &config) {
        _classSelection->resetFocus();
        _portraitSelection->loadPortraits(config);
        _screen = CharGenScreen::QuickOrCustom;
    });
    _classSelection->setOnCancel([this]() {
        _classSelection->resetFocus();

        if (_onCancel) {
            _onCancel();
        }
    });
}

void CharacterGeneration::loadQuickOrCustom() {
    _quickOrCustom = make_unique<QuickOrCustom>(_version, _gfxOpts);
    _quickOrCustom->load();
    _quickOrCustom->setOnQuickCharacter([this]() {
        _quickOrCustom->resetFocus();
        _screen = CharGenScreen::Quick;
    });
    _quickOrCustom->setOnBack([this]() {
        _quickOrCustom->resetFocus();
        _screen = CharGenScreen::ClassSelection;
    });
}

void CharacterGeneration::loadQuickCharacterGeneration() {
    _quick = make_unique<QuickCharacterGeneration>(_version, _gfxOpts);
    _quick->load();

    if (_version == GameVersion::KotOR) {
        _quick->configureControl("LBL_DECORATION", [](Control &ctrl) { ctrl.setDiscardColor(glm::vec3(0.0f, 0.0f, 0.082353f)); });
    }
    _quick->setOnStepSelected([this](int step) {
        _quick->resetFocus();

        switch (step) {
            case 1:
                _screen = CharGenScreen::PortraitSelection;
                break;
            case 2:
                _screen = CharGenScreen::Name;
                break;
            default:
                if (_onPlay) {
                    _onPlay(_character);
                }
                break;
        }
    });
    _quick->setOnCancel([this]() {
        _quick->resetFocus();
        _screen = CharGenScreen::QuickOrCustom;
    });
}

void CharacterGeneration::loadPortraitSelection() {
    _portraitSelection = make_unique<PortraitSelection>(_version, _gfxOpts);
    _portraitSelection->load();
    _portraitSelection->setOnPortraitSelected([this](const CreatureConfiguration &config) {
        _portraitSelection->resetFocus();
        _character = config;
        _screen = CharGenScreen::Quick;
        _quick->setStep(1);
    });
    _portraitSelection->setOnCancel([this]() {
        _portraitSelection->resetFocus();
        _screen = CharGenScreen::Quick;
    });
}

void CharacterGeneration::loadNameGui() {
    _nameGui = make_unique<NameGui>(_version, _gfxOpts);
    _nameGui->load();
    _nameGui->setOnEnd([this]() {
        _nameGui->resetFocus();
        _screen = CharGenScreen::Quick;
        _quick->setStep(2);
    });
    _nameGui->setOnBack([this]() {
        _nameGui->resetFocus();
        _screen = CharGenScreen::Quick;
    });
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
        case CharGenScreen::PortraitSelection:
            return _portraitSelection.get();
        case CharGenScreen::Name:
            return _nameGui.get();
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

void CharacterGeneration::setOnPlay(const function<void(const CreatureConfiguration &)> &fn) {
    _onPlay = fn;
}

void CharacterGeneration::setOnCancel(const function<void()> &fn) {
    _onCancel = fn;
}

} // namespace game

} // namespace reone
