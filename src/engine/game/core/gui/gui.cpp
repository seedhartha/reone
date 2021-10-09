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

#include "gui.h"

#include "../../../audio/player.h"
#include "../../../graphics/texture/textures.h"

#include "../game.h"

#include "sounds.h"

using namespace std;

using namespace reone::audio;
using namespace reone::gui;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

GameGUI::GameGUI(
    Game *game,
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
    audio::AudioFiles &audioFiles,
    audio::AudioPlayer &audioPlayer,
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
    Strings &strings) :
    GUI(
        game->options().graphics,
        context,
        features,
        fonts,
        materials,
        meshes,
        pbrIbl,
        shaders,
        textures,
        window,
        resources,
        strings),
    _game(game),
    _actionFactory(actionFactory),
    _classes(classes),
    _combat(combat),
    _feats(feats),
    _footstepSounds(footstepSounds),
    _guiSounds(guiSounds),
    _objectFactory(objectFactory),
    _party(party),
    _portraits(portraits),
    _reputes(reputes),
    _scriptRunner(scriptRunner),
    _soundSets(soundSets),
    _surfaces(surfaces),
    _audioFiles(audioFiles),
    _audioPlayer(audioPlayer),
    _lips(lips),
    _models(models),
    _walkmeshes(walkmeshes) {
}

void GameGUI::onClick(const string &control) {
    _audioPlayer.play(_guiSounds.getOnClick(), AudioType::Sound);
}

void GameGUI::onFocusChanged(const string &control, bool focus) {
    if (focus) {
        _audioPlayer.play(_guiSounds.getOnEnter(), AudioType::Sound);
    }
}

void GameGUI::initForGame() {
    if (_game->isTSL()) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        _hasDefaultHilightColor = true;
        _defaultHilightColor = _game->getGUIColorHilight();
    }
}

string GameGUI::getResRef(const std::string &base) const {
    return _game->isTSL() ? base + "_p" : base;
}

void GameGUI::loadBackground(BackgroundType type) {
    string resRef;

    if (_game->isTSL()) {
        switch (type) {
        case BackgroundType::Computer0:
        case BackgroundType::Computer1:
            resRef = "pnl_computer_pc";
            break;
        default:
            break;
        }
    } else {
        if ((_options.width == 1600 && _options.height == 1200) ||
            (_options.width == 1280 && _options.height == 960) ||
            (_options.width == 1024 && _options.height == 768) ||
            (_options.width == 800 && _options.height == 600)) {

            resRef = str(boost::format("%dx%d") % _options.width % _options.height);
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

    _background = _textures.get(resRef, TextureUsage::Diffuse);
}

} // namespace game

} // namespace reone
