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

#include "reone/game/gui.h"

#include "reone/audio/di/services.h"
#include "reone/audio/player.h"
#include "reone/game/di/services.h"
#include "reone/game/game.h"
#include "reone/game/guisounds.h"
#include "reone/graphics/di/services.h"
#include "reone/graphics/textures.h"
#include "reone/resource/di/services.h"
#include "reone/scene/di/services.h"
#include "reone/system/exception/notfound.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

void GameGUI::load(const string &resRef) {
    _gui = _services.gui.guis.get(resRef, bind(&GameGUI::preload, this, _1));
    if (!_gui) {
        throw NotFoundException(str(boost::format("GUI not found: %s") % resRef));
    }
    _gui->setEventListener(*this);

    bindControls();

    if (_game.isTSL()) {
        _baseColor = kTSLGUIColorBase;
        _hilightColor = kTSLGUIColorHilight;
        _disabledColor = kTSLGUIColorDisabled;
    } else {
        _baseColor = kGUIColorBase;
        _hilightColor = kGUIColorHilight;
        _disabledColor = kGUIColorDisabled;
    }
}

void GameGUI::preload(IGUI &gui) {
    if (_game.isTSL()) {
        gui.setResolution(800, 600);
    } else {
        gui.setDefaultHilightColor(kGUIColorHilight);
    }
}

bool GameGUI::handle(const SDL_Event &event) {
    if (!_gui) {
        return false;
    }
    return _gui->handle(event);
}

void GameGUI::update(float dt) {
    if (_gui) {
        _gui->update(dt);
    }
    if (_audioSource) {
        _audioSource->update();
    }
}

void GameGUI::draw() {
    if (_gui) {
        _gui->draw();
    }
}

void GameGUI::loadBackground(BackgroundType type) {
    string resRef;

    if (_game.isTSL()) {
        switch (type) {
        case BackgroundType::Computer0:
        case BackgroundType::Computer1:
            resRef = "pnl_computer_pc";
            break;
        default:
            break;
        }
    } else {
        auto &options = _game.options().graphics;
        if ((options.width == 1600 && options.height == 1200) ||
            (options.width == 1280 && options.height == 960) ||
            (options.width == 1024 && options.height == 768) ||
            (options.width == 800 && options.height == 600)) {

            resRef = str(boost::format("%dx%d") % options.width % options.height);
        } else {
            resRef = "1600x1200";
        }
        switch (type) {
        case BackgroundType::Menu:
            resRef += "back";
            break;
        case BackgroundType::Load:
            resRef += "load";
            break;
        case BackgroundType::Computer0:
            resRef += "comp0";
            break;
        case BackgroundType::Computer1:
            resRef += "comp1";
            break;
        default:
            return;
        }
    }

    if (_gui) {
        _gui->setBackground(_services.graphics.textures.get(resRef, TextureUsage::Diffuse));
    }
}

string GameGUI::getResRef(const string &base) const {
    return _game.isTSL() ? base + "_p" : base;
}

void GameGUI::onClick(const string &control) {
    _audioSource = _services.audio.player.play(_services.game.guiSounds.getOnClick(), AudioType::Sound);
}

void GameGUI::onFocusChanged(const string &control, bool focus) {
    if (focus) {
        _audioSource = _services.audio.player.play(_services.game.guiSounds.getOnEnter(), AudioType::Sound);
    }
}

} // namespace game

} // namespace reone
