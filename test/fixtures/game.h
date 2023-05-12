/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "reone/game/services.h"

namespace reone {

namespace game {

class MockCameraStyles : public ICameraStyles {
};

class MockClasses : public IClasses {
};

class MockCursors : public ICursors {
};

class MockDialogs : public IDialogs {
};

class MockFeats : public IFeats {
};

class MockFootstepSounds : public IFootstepSounds {
};

class MockGUISounds : public IGUISounds {
};

class MockLayouts : public ILayouts {
};

class MockPaths : public IPaths {
};

class MockPortraits : public IPortraits {
};

class MockReputes : public IReputes {
};

class MockResourceLayout : public IResourceLayout {
};

class MockSkills : public ISkills {
};

class MockSoundSets : public ISoundSets {
};

class MockSpells : public ISpells {
};

class MockSurfaces : public ISurfaces {
};

class MockVisiblities : public IVisibilities {
};

inline std::unique_ptr<GameServices> mockGameServices() {
    // TODO: free automatically
    auto cameraStyles = new MockCameraStyles();
    auto classes = new MockClasses();
    auto cursors = new MockCursors();
    auto dialogs = new MockDialogs();
    auto feats = new MockFeats();
    auto footstepSounds = new MockFootstepSounds();
    auto guiSounds = new MockGUISounds();
    auto layouts = new MockLayouts();
    auto paths = new MockPaths();
    auto portraits = new MockPortraits();
    auto reputes = new MockReputes();
    auto resourceLayout = new MockResourceLayout();
    auto skills = new MockSkills();
    auto soundSets = new MockSoundSets();
    auto spells = new MockSpells();
    auto surfaces = new MockSurfaces();
    auto visibilities = new MockVisiblities();

    return std::make_unique<GameServices>(
        *cameraStyles,
        *classses,
        *cursors,
        *dialogs,
        *feats,
        *footstepSounds,
        *guiSounds,
        *layouts,
        *paths,
        *portraits,
        *reputes,
        *resourceLayout,
        *skills,
        *soundSets,
        *spells,
        *visibilities);
}

} // namespace game

} // namespace reone
