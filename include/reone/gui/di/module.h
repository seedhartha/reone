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

#include "reone/resource/di/module.h"

#include "services.h"

namespace reone {

namespace gui {

class GUIModule : boost::noncopyable {
public:
    GUIModule(resource::ResourceModule &resource) :
        _resource(resource) {
    }

    ~GUIModule() { deinit(); }

    void init();
    void deinit();

    GUIs &guis() { return *_guis; }

    GUIServices &services() { return *_services; }

private:
    resource::ResourceModule &_resource;

    std::unique_ptr<GUIs> _guis;

    std::unique_ptr<GUIServices> _services;
};

} // namespace movie

} // namespace reone
