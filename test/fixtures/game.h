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

#pragma once

#include <gmock/gmock.h>

#include "reone/game/di/services.h"
#include "reone/system/exception/notimplemented.h"

#include "reone/game/camerastyles.h"
#include "reone/game/cursors.h"
#include "reone/game/d20/classes.h"
#include "reone/game/d20/feats.h"
#include "reone/game/d20/skills.h"
#include "reone/game/d20/spells.h"
#include "reone/game/dialogs.h"
#include "reone/game/footstepsounds.h"
#include "reone/game/gui/sounds.h"
#include "reone/game/layouts.h"
#include "reone/game/options.h"
#include "reone/game/paths.h"
#include "reone/game/portraits.h"
#include "reone/game/reputes.h"
#include "reone/game/resourcedirector.h"
#include "reone/game/soundsets.h"
#include "reone/game/surfaces.h"
#include "reone/game/types.h"
#include "reone/game/visibilities.h"

namespace reone {

namespace game {

class MockCameraStyles : public ICameraStyles, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<CameraStyle>, get, (int index), (const override));
    MOCK_METHOD(std::shared_ptr<CameraStyle>, get, (const std::string &name), (const override));
};

class MockClasses : public IClasses, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<CreatureClass>, get, (ClassType key), (override));
};

class MockCursors : public ICursors, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<graphics::Cursor>, get, (game::CursorType type), (override));
};

class MockDialogs : public IDialogs, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Dialog>, get, (const std::string &key), (override));
};

class MockFeats : public IFeats, boost::noncopyable {
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(std::shared_ptr<Feat>, get, (FeatType type), (const override));
};

class MockFootstepSounds : public IFootstepSounds, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<FootstepTypeSounds>, get, (uint32_t key), (override));
};

class MockGUISounds : public IGUISounds, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<audio::AudioBuffer>, getOnClick, (), (const override));
    MOCK_METHOD(std::shared_ptr<audio::AudioBuffer>, getOnEnter, (), (const override));
};

class MockLayouts : public ILayouts, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<resource::Layout>, get, (const std::string &key), (override));
};

class MockPaths : public IPaths, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Path>, get, (const std::string &key), (override));
};

class MockPortraits : public IPortraits, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<graphics::Texture>, getTextureByIndex, (int index), (const override));
    MOCK_METHOD(std::shared_ptr<graphics::Texture>, getTextureByAppearance, (int appearance), (const override));
    MOCK_METHOD(const std::vector<Portrait> &, portraits, (), (const override));
};

class MockReputes : public IReputes, boost::noncopyable {
public:
    MOCK_METHOD(bool, getIsEnemy, (const Creature &left, const Creature &right), (const override));
    MOCK_METHOD(bool, getIsFriend, (const Creature &left, const Creature &right), (const override));
    MOCK_METHOD(bool, getIsNeutral, (const Creature &left, const Creature &right), (const override));
};

class MockResourceDirector : public IResourceDirector, boost::noncopyable {
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(void, onModuleLoad, (const std::string &name), (override));
    MOCK_METHOD(std::set<std::string>, moduleNames, (), (override));
};

class MockSkills : public ISkills, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Skill>, get, (SkillType type), (const override));
};

class MockSoundSets : public ISoundSets, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<SoundSet>, get, (const std::string &key), (override));
};

class MockSpells : public ISpells, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Spell>, get, (SpellType type), (const override));
};

class MockSurfaces : public ISurfaces, boost::noncopyable {
public:
    MOCK_METHOD(bool, isWalkable, (int index), (const override));
    MOCK_METHOD(const Surface &, getSurface, (int index), (const override));

    MOCK_METHOD(std::set<uint32_t>, getGrassSurfaces, (), (const override));
    MOCK_METHOD(std::set<uint32_t>, getWalkableSurfaces, (), (const override));
    MOCK_METHOD(std::set<uint32_t>, getWalkcheckSurfaces, (), (const override));
    MOCK_METHOD(std::set<uint32_t>, getLineOfSightSurfaces, (), (const override));
};

class MockVisiblities : public IVisibilities, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<Visibility>, get, (const std::string &key), (override));
};

class TestGameModule : boost::noncopyable {
public:
    void init() {
        _cameraStyles = std::make_unique<MockCameraStyles>();
        _classes = std::make_unique<MockClasses>();
        _cursors = std::make_unique<MockCursors>();
        _dialogs = std::make_unique<MockDialogs>();
        _feats = std::make_unique<MockFeats>();
        _footstepSounds = std::make_unique<MockFootstepSounds>();
        _guiSounds = std::make_unique<MockGUISounds>();
        _layouts = std::make_unique<MockLayouts>();
        _paths = std::make_unique<MockPaths>();
        _portraits = std::make_unique<MockPortraits>();
        _reputes = std::make_unique<MockReputes>();
        _resourceDirector = std::make_unique<MockResourceDirector>();
        _skills = std::make_unique<MockSkills>();
        _soundSets = std::make_unique<MockSoundSets>();
        _spells = std::make_unique<MockSpells>();
        _surfaces = std::make_unique<MockSurfaces>();
        _visibilities = std::make_unique<MockVisiblities>();

        _services = std::make_unique<GameServices>(
            *_cameraStyles,
            *_classes,
            *_cursors,
            *_dialogs,
            *_feats,
            *_footstepSounds,
            *_guiSounds,
            *_layouts,
            *_paths,
            *_portraits,
            *_reputes,
            *_resourceDirector,
            *_skills,
            *_soundSets,
            *_spells,
            *_surfaces,
            *_visibilities);
    }

    MockResourceDirector &resourceDirector() {
        return *_resourceDirector;
    }

    GameServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockCameraStyles> _cameraStyles;
    std::unique_ptr<MockClasses> _classes;
    std::unique_ptr<MockCursors> _cursors;
    std::unique_ptr<MockDialogs> _dialogs;
    std::unique_ptr<MockFeats> _feats;
    std::unique_ptr<MockFootstepSounds> _footstepSounds;
    std::unique_ptr<MockGUISounds> _guiSounds;
    std::unique_ptr<MockLayouts> _layouts;
    std::unique_ptr<MockPaths> _paths;
    std::unique_ptr<MockPortraits> _portraits;
    std::unique_ptr<MockReputes> _reputes;
    std::unique_ptr<MockResourceDirector> _resourceDirector;
    std::unique_ptr<MockSkills> _skills;
    std::unique_ptr<MockSoundSets> _soundSets;
    std::unique_ptr<MockSpells> _spells;
    std::unique_ptr<MockSurfaces> _surfaces;
    std::unique_ptr<MockVisiblities> _visibilities;

    std::unique_ptr<GameServices> _services;
};

} // namespace game

} // namespace reone
