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

#include "types.h"

namespace reone {

namespace resource {

struct ResourceServices;

}

namespace graphics {

struct GraphicsServices;

}

namespace script {

struct ScriptServices;

}

namespace game {

struct OptionsView;

class IResourceDirector {
public:
    virtual ~IResourceDirector() = default;

    virtual void init() = 0;
    virtual void onModuleLoad(const std::string &name) = 0;

    virtual std::set<std::string> moduleNames() = 0;
};

class ResourceDirector : public IResourceDirector, boost::noncopyable {
public:
    ResourceDirector(GameID gameId,
                     OptionsView &options,
                     script::ScriptServices &scriptSvc,
                     graphics::GraphicsServices &graphicsSvc,
                     resource::ResourceServices &resourceSvc) :
        _gameId(gameId),
        _options(options),
        _scriptSvc(scriptSvc),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    void init() override;
    void onModuleLoad(const std::string &name) override;

    std::set<std::string> moduleNames() override;

private:
    GameID _gameId;
    OptionsView &_options;
    script::ScriptServices &_scriptSvc;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    void loadGlobalResources();
    void loadModuleResources(const std::string &name);
};

} // namespace game

} // namespace reone
