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

} // namespace gui

namespace game {

class Map;
class Waypoint;

class MapMenu : public GameGUI {
public:
    MapMenu(
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
        resource::Strings &strings);

    void load() override;
    void draw() override;

    void refreshControls();

private:
    struct Binding {
        std::shared_ptr<gui::Button> btnExit;
        std::shared_ptr<gui::Button> btnUp;
        std::shared_ptr<gui::Button> btnDown;
        std::shared_ptr<gui::Button> btnReturn;
        std::shared_ptr<gui::Label> lblArea;
        std::shared_ptr<gui::Label> lblMap;
        std::shared_ptr<gui::Label> lblMapNote;

        // KotOR only
        std::shared_ptr<gui::Button> btnPrtySlct;
        // END KotOR only
    } _binding;

    std::vector<std::shared_ptr<Waypoint>> _notes;
    int _selectedNoteIdx {0};

    void bindControls();
    void refreshSelectedNote();
};

} // namespace game

} // namespace reone
