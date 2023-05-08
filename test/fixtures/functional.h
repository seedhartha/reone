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

#include "reone/audio/context.h"
#include "reone/audio/files.h"
#include "reone/audio/options.h"
#include "reone/audio/player.h"
#include "reone/audio/services.h"
#include "../../game/astar.h"
#include "../../game/camerastyles.h"
#include "../../game/cursors.h"
#include "../../game/footstepsounds.h"
#include "../../game/guisounds.h"
#include "../../game/layouts.h"
#include "../../game/options.h"
#include "../../game/paths.h"
#include "../../game/portraits.h"
#include "../../game/resourcelayout.h"
#include "../../game/services.h"
#include "../../game/soundsets.h"
#include "../../game/surfaces.h"
#include "../../game/visibilities.h"
#include "reone/graphics/context.h"
#include "reone/graphics/fonts.h"
#include "reone/graphics/lipanimations.h"
#include "reone/graphics/meshes.h"
#include "reone/graphics/models.h"
#include "reone/graphics/options.h"
#include "reone/graphics/pipeline.h"
#include "reone/graphics/services.h"
#include "reone/graphics/shaders.h"
#include "reone/graphics/textures.h"
#include "reone/graphics/uniforms.h"
#include "reone/graphics/walkmeshes.h"
#include "reone/graphics/window.h"
#include "reone/resource/2das.h"
#include "reone/resource/gffs.h"
#include "reone/resource/resources.h"
#include "reone/resource/services.h"
#include "reone/resource/strings.h"
#include "../../scene/graphs.h"
#include "../../scene/services.h"
#include "../../script/scripts.h"
#include "../../script/services.h"

#include "game.h"
#include "scene.h"

namespace reone {

class FunctionalTest {
public:
    FunctionalTest() :
        _options(
            _gameOpt,
            _graphicsOpt,
            _audioOpt) {
    }

    std::unique_ptr<game::MockGame> mockGame() {
        initOnce();
        return std::make_unique<game::MockGame>(_gameId, _options, *_services);
    }

    scene::MockSceneGraph &sceneMockByName(const std::string &name) {
        initOnce();
        return static_cast<scene::MockSceneGraph &>(_sceneGraphs->get(name));
    }

    game::ServicesView &services() {
        initOnce();
        return *_services;
    }

private:
    game::GameID _gameId {game::GameID::KotOR};

    bool _inited {false};

    // Options

    game::GameOptions _gameOpt;
    graphics::GraphicsOptions _graphicsOpt;
    audio::AudioOptions _audioOpt;
    game::OptionsView _options;

    // END Options

    // Services

    std::unique_ptr<resource::Gffs> _gffs;
    std::unique_ptr<resource::Resources> _resources;
    std::unique_ptr<resource::Strings> _strings;
    std::unique_ptr<resource::TwoDas> _twoDas;
    std::unique_ptr<resource::ResourceServices> _resourceSvc;

    std::unique_ptr<graphics::Fonts> _fonts;
    std::unique_ptr<graphics::GraphicsContext> _graphicsContext;
    std::unique_ptr<graphics::LipAnimations> _lipAnimations;
    std::unique_ptr<graphics::Meshes> _meshes;
    std::unique_ptr<graphics::Models> _models;
    std::unique_ptr<graphics::Pipeline> _pipeline;
    std::unique_ptr<graphics::Shaders> _shaders;
    std::unique_ptr<graphics::Textures> _textures;
    std::unique_ptr<graphics::Uniforms> _uniforms;
    std::unique_ptr<graphics::Walkmeshes> _walkmeshes;
    std::unique_ptr<graphics::Window> _window;
    std::unique_ptr<graphics::GraphicsServices> _graphicsSvc;

    std::unique_ptr<audio::AudioContext> _audioContext;
    std::unique_ptr<audio::AudioFiles> _audioFiles;
    std::unique_ptr<audio::AudioPlayer> _audioPlayer;
    std::unique_ptr<audio::AudioServices> _audioSvc;

    std::unique_ptr<scene::SceneGraphs> _sceneGraphs;
    std::unique_ptr<scene::SceneServices> _sceneSvc;

    std::unique_ptr<script::Scripts> _scripts;
    std::unique_ptr<script::ScriptServices> _scriptSvc;

    std::unique_ptr<game::AStar> _aStar;
    std::unique_ptr<game::CameraStyles> _cameraStyles;
    std::unique_ptr<game::Cursors> _cursors;
    std::unique_ptr<game::FootstepSounds> _footstepSounds;
    std::unique_ptr<game::GUISounds> _guiSounds;
    std::unique_ptr<game::Layouts> _layouts;
    std::unique_ptr<game::Paths> _paths;
    std::unique_ptr<game::Portraits> _portraits;
    std::unique_ptr<game::ResourceLayout> _resourceLayout;
    std::unique_ptr<game::SoundSets> _soundSets;
    std::unique_ptr<game::Surfaces> _surfaces;
    std::unique_ptr<game::Visibilities> _visibilities;
    std::unique_ptr<game::GameServices> _gameSvc;

    std::unique_ptr<game::ServicesView> _services;

    // END Services

    void initOnce() {
        if (!_inited) {
            initServices();
            _inited = true;
        }
    }

    void initServices() {
        initResourceServices();
        initGraphicsServices();
        initAudioServices();
        initSceneServices();
        initScriptServices();
        initGameServices();

        _services = std::make_unique<game::ServicesView>(
            *_gameSvc,
            *_audioSvc,
            *_graphicsSvc,
            *_sceneSvc,
            *_scriptSvc,
            *_resourceSvc);
    }

    void initResourceServices() {
        _resources = std::make_unique<resource::Resources>();
        _strings = std::make_unique<resource::Strings>();
        _gffs = std::make_unique<resource::Gffs>(*_resources);
        _twoDas = std::make_unique<resource::TwoDas>(*_resources);

        _resourceSvc = std::make_unique<resource::ResourceServices>(
            *_gffs,
            *_resources,
            *_strings,
            *_twoDas);
    }

    void initGraphicsServices() {
        _textures = std::make_unique<graphics::Textures>(_graphicsOpt, *_resources);
        _models = std::make_unique<graphics::Models>(*_textures, *_resources);
        _walkmeshes = std::make_unique<graphics::Walkmeshes>(*_resources);

        _graphicsSvc = std::make_unique<graphics::GraphicsServices>(
            *_fonts,
            *_graphicsContext,
            *_lipAnimations,
            *_meshes,
            *_models,
            *_pipeline,
            *_shaders,
            *_textures,
            *_uniforms,
            *_walkmeshes,
            *_window);
    }

    void initAudioServices() {
        _audioSvc = std::make_unique<audio::AudioServices>(
            *_audioContext,
            *_audioFiles,
            *_audioPlayer);
    }

    void initSceneServices() {
        _sceneGraphs = std::make_unique<scene::MockSceneGraphs>(
            _graphicsOpt,
            *_graphicsSvc,
            *_audioSvc);

        _sceneGraphs->reserve(game::kSceneMain);

        _sceneSvc = std::make_unique<scene::SceneServices>(
            *_sceneGraphs);
    }

    void initScriptServices() {
        _scriptSvc = std::make_unique<script::ScriptServices>(
            *_scripts);
    }

    void initGameServices() {
        _gameSvc = std::make_unique<game::GameServices>(
            *_aStar,
            *_cameraStyles,
            *_cursors,
            *_footstepSounds,
            *_guiSounds,
            *_layouts,
            *_paths,
            *_portraits,
            *_resourceLayout,
            *_soundSets,
            *_surfaces,
            *_visibilities);
    }
};

} // namespace reone
