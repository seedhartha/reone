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

#include "gui.h"

namespace reone {

namespace resource {

class Gffs;

}

namespace gui {

class IGUIs {
public:
    virtual ~IGUIs() = default;

    virtual void clear() = 0;

    virtual std::shared_ptr<IGUI> get(const std::string &resRef, std::function<void(IGUI &)> preload = nullptr) = 0;
};

class GUIs : public IGUIs, boost::noncopyable {
public:
    GUIs(graphics::GraphicsOptions &graphicsOpt,
         scene::ISceneGraphs &sceneGraphs,
         graphics::GraphicsServices &graphicsSvc,
         resource::ResourceServices &resourceSvc) :
        _graphicsOpt(graphicsOpt),
        _sceneGraphs(sceneGraphs),
        _graphicsSvc(graphicsSvc),
        _resourceSvc(resourceSvc) {
    }

    void clear() override {
        _guis.clear();
    }

    std::shared_ptr<IGUI> get(const std::string &resRef, std::function<void(IGUI &)> preload) override {
        auto maybeGUI = _guis.find(resRef);
        if (maybeGUI != _guis.end()) {
            return maybeGUI->second;
        }
        auto gui = doGet(resRef, preload);
        return _guis.insert(make_pair(resRef, std::move(gui))).first->second;
    }

private:
    graphics::GraphicsOptions &_graphicsOpt;
    scene::ISceneGraphs &_sceneGraphs;
    graphics::GraphicsServices &_graphicsSvc;
    resource::ResourceServices &_resourceSvc;

    std::unordered_map<std::string, std::shared_ptr<IGUI>> _guis;

    std::shared_ptr<IGUI> doGet(std::string resRef, std::function<void(IGUI &)> preload);
};

} // namespace gui

} // namespace reone
