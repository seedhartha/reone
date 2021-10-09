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

#include "loadscreen.h"

#include "../../../resource/resources.h"

#include "../game.h"

using namespace std;

using namespace reone::gui;
using namespace reone::resource;

namespace reone {

namespace game {

LoadingScreen::LoadingScreen(
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
    graphics::Context &context,
    graphics::Features &features,
    graphics::Fonts &fonts,
    graphics::Lips &lips,
    graphics::Materials &materials,
    graphics::Meshes &meshes,
    graphics::Models &models,
    graphics::PBRIBL &pbrIbl,
    graphics::Shaders &shaders,
    graphics::Textures &textures,
    graphics::Walkmeshes &walkmeshes,
    graphics::Window &window,
    resource::Resources &resources,
    resource::Strings &strings) :
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
        strings) {
    _resRef = getResRef("loadscreen");

    if (_game->isTSL()) {
        _resolutionX = 800;
        _resolutionY = 600;
    } else {
        loadBackground(BackgroundType::Load);
    }
}

void LoadingScreen::load() {
    GUI::load();
    bindControls();

    _binding.lblHint->setTextMessage("");
}

void LoadingScreen::bindControls() {
    _binding.pbProgress = getControl<ProgressBar>("PB_PROGRESS");
    _binding.lblHint = getControl<Label>("LBL_HINT");
    _binding.lblLogo = getControl<Label>("LBL_LOGO");
    _binding.lblLoading = getControl<Label>("LBL_LOADING");
}

void LoadingScreen::setImage(const string &resRef) {
    _rootControl->setBorderFill(resRef);
}

void LoadingScreen::setProgress(int progress) {
    _binding.pbProgress->setValue(progress);
}

} // namespace game

} // namespace reone
