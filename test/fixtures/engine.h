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

#include "reone/system/logger.h"

#include "audio.h"
#include "game.h"
#include "graphics.h"
#include "gui.h"
#include "movie.h"
#include "resource.h"
#include "scene.h"
#include "script.h"
#include "system.h"

namespace reone {

class TestEngine : boost::noncopyable {
public:
    void init() {
        Logger::instance.init(LogSeverity::Error, {}, std::nullopt);

        _gameModule = std::make_unique<game::TestGameModule>();
        _movieModule = std::make_unique<movie::TestMovieModule>();
        _audioModule = std::make_unique<audio::TestAudioModule>();
        _graphicsModule = std::make_unique<graphics::TestGraphicsModule>();
        _sceneModule = std::make_unique<scene::TestSceneModule>();
        _guiModule = std::make_unique<gui::TestGUIModule>();
        _scriptModule = std::make_unique<script::TestScriptModule>();
        _resourceModule = std::make_unique<resource::TestResourceModule>();
        _systemModule = std::make_unique<TestSystemModule>();

        _gameModule->init();
        _movieModule->init();
        _audioModule->init();
        _graphicsModule->init();
        _sceneModule->init();
        _guiModule->init();
        _scriptModule->init();
        _resourceModule->init();
        _systemModule->init();

        _services = std::make_unique<game::ServicesView>(
            _gameModule->services(),
            _movieModule->services(),
            _audioModule->services(),
            _graphicsModule->services(),
            _sceneModule->services(),
            _guiModule->services(),
            _scriptModule->services(),
            _resourceModule->services(),
            _systemModule->services());
    }

    game::OptionsView &options() {
        return _options;
    }

    game::TestGameModule &gameModule() {
        return *_gameModule;
    }

    movie::TestMovieModule &movieModule() {
        return *_movieModule;
    }

    audio::TestAudioModule &audioModule() {
        return *_audioModule;
    }

    graphics::TestGraphicsModule &graphicsModule() {
        return *_graphicsModule;
    }

    scene::TestSceneModule &sceneModule() {
        return *_sceneModule;
    }

    gui::TestGUIModule &guiModule() {
        return *_guiModule;
    }

    script::TestScriptModule &scriptModule() {
        return *_scriptModule;
    }

    resource::TestResourceModule &resourceModule() {
        return *_resourceModule;
    }

    TestSystemModule &systemModule() {
        return *_systemModule;
    }

    game::ServicesView &services() {
        return *_services;
    }

private:
    game::GameOptions _gameOpt;
    graphics::GraphicsOptions _graphicsOpt;
    audio::AudioOptions _audioOpt;
    game::OptionsView _options {_gameOpt, _graphicsOpt, _audioOpt};

    std::unique_ptr<game::TestGameModule> _gameModule;
    std::unique_ptr<movie::TestMovieModule> _movieModule;
    std::unique_ptr<audio::TestAudioModule> _audioModule;
    std::unique_ptr<graphics::TestGraphicsModule> _graphicsModule;
    std::unique_ptr<scene::TestSceneModule> _sceneModule;
    std::unique_ptr<gui::TestGUIModule> _guiModule;
    std::unique_ptr<script::TestScriptModule> _scriptModule;
    std::unique_ptr<resource::TestResourceModule> _resourceModule;
    std::unique_ptr<TestSystemModule> _systemModule;

    std::unique_ptr<game::ServicesView> _services;
};

} // namespace reone
