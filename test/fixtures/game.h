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
#include "reone/game/d20/classes.h"
#include "reone/game/d20/feats.h"
#include "reone/game/d20/skills.h"
#include "reone/game/d20/spells.h"
#include "reone/game/footstepsounds.h"
#include "reone/game/gui/sounds.h"
#include "reone/game/options.h"
#include "reone/game/portraits.h"
#include "reone/game/reputes.h"
#include "reone/game/surfaces.h"
#include "reone/game/types.h"

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

class MockFragmenteats : public IFeats, boost::noncopyable {
public:
    MOCK_METHOD(void, init, (), (override));
    MOCK_METHOD(std::shared_ptr<Feat>, get, (FeatType type), (const override));
};

class MockFragmentootstepSounds : public IFootstepSounds, boost::noncopyable {
public:
    MOCK_METHOD(void, clear, (), (override));
    MOCK_METHOD(std::shared_ptr<FootstepTypeSounds>, get, (uint32_t key), (override));
};

class MockGeometryUISounds : public IGUISounds, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<audio::AudioClip>, getOnClick, (), (const override));
    MOCK_METHOD(std::shared_ptr<audio::AudioClip>, getOnEnter, (), (const override));
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

class MockSkills : public ISkills, boost::noncopyable {
public:
    MOCK_METHOD(std::shared_ptr<Skill>, get, (SkillType type), (const override));
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

class TestGameModule : boost::noncopyable {
public:
    void init() {
        _cameraStyles = std::make_unique<MockCameraStyles>();
        _classes = std::make_unique<MockClasses>();
        _feats = std::make_unique<MockFragmenteats>();
        _footstepSounds = std::make_unique<MockFragmentootstepSounds>();
        _guiSounds = std::make_unique<MockGeometryUISounds>();
        _portraits = std::make_unique<MockPortraits>();
        _reputes = std::make_unique<MockReputes>();
        _skills = std::make_unique<MockSkills>();
        _spells = std::make_unique<MockSpells>();
        _surfaces = std::make_unique<MockSurfaces>();

        _services = std::make_unique<GameServices>(
            *_cameraStyles,
            *_classes,
            *_feats,
            *_footstepSounds,
            *_guiSounds,
            *_portraits,
            *_reputes,
            *_skills,
            *_spells,
            *_surfaces);
    }

    GameServices &services() {
        return *_services;
    }

private:
    std::unique_ptr<MockCameraStyles> _cameraStyles;
    std::unique_ptr<MockClasses> _classes;
    std::unique_ptr<MockFragmenteats> _feats;
    std::unique_ptr<MockFragmentootstepSounds> _footstepSounds;
    std::unique_ptr<MockGeometryUISounds> _guiSounds;
    std::unique_ptr<MockPortraits> _portraits;
    std::unique_ptr<MockReputes> _reputes;
    std::unique_ptr<MockSkills> _skills;
    std::unique_ptr<MockSpells> _spells;
    std::unique_ptr<MockSurfaces> _surfaces;

    std::unique_ptr<GameServices> _services;
};

} // namespace game

} // namespace reone
