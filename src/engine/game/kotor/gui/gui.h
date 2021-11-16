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

#pragma once

#include "../../../gui/gui.h"

#include "../../core/types.h"

namespace reone {

namespace resource {

class TwoDas;

}

namespace graphics {

class Lips;
class Models;
class Walkmeshes;

} // namespace graphics

namespace audio {

class AudioFiles;
class AudioPlayer;

} // namespace audio

namespace game {

class ActionFactory;
class Classes;
class Combat;
class Feats;
class FootstepSounds;
class GUISounds;
class KotOR;
class ObjectFactory;
class Party;
class Portraits;
class Reputes;
class ScriptRunner;
class SoundSets;
class Surfaces;

/**
 * Encapsulates game-specific GUI configuration.
 */
class GameGUI : public gui::GUI {
protected:
    GameGUI(
        KotOR *game,
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
        resource::Strings &strings,
        resource::TwoDas &twoDas);

    void initForGame();

    std::string getResRef(const std::string &base) const;

protected:
    KotOR *_game;

    // Services

    ActionFactory &_actionFactory;
    Classes &_classes;
    Combat &_combat;
    Feats &_feats;
    FootstepSounds &_footstepSounds;
    GUISounds &_guiSounds;
    ObjectFactory &_objectFactory;
    Party &_party;
    Portraits &_portraits;
    Reputes &_reputes;
    ScriptRunner &_scriptRunner;
    SoundSets &_soundSets;
    Surfaces &_surfaces;

    audio::AudioFiles &_audioFiles;
    audio::AudioPlayer &_audioPlayer;
    graphics::Lips &_lips;
    graphics::Models &_models;
    graphics::Walkmeshes &_walkmeshes;
    resource::TwoDas &_twoDas;

    // END Services

    void loadBackground(BackgroundType type);

private:
    void onClick(const std::string &control) override;
    void onFocusChanged(const std::string &control, bool focus) override;
};

} // namespace game

} // namespace reone
