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

#include "../gui.h"

namespace reone {

namespace gui {

class Button;
class Label;
class ListBox;

} // namespace gui

namespace game {

class CharacterGeneration;

class QuickOrCustom : public GameGUI {
public:
    QuickOrCustom(
        CharacterGeneration *charGen,
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
        resource::Gffs &gffs,
        resource::Resources &resources,
        resource::Strings &strings,
        resource::TwoDas &twoDas);

    void load() override;

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnBack;
        std::shared_ptr<gui::Button> custCharBtn;
        std::shared_ptr<gui::Button> quickCharBtn;
        std::shared_ptr<gui::Label> lblRbg;
        std::shared_ptr<gui::ListBox> lbDesc;
    } _binding;

    CharacterGeneration *_charGen;

    void bindControls();
};

} // namespace game

} // namespace reone
