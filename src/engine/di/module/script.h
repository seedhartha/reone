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

#include "../../script/scripts.h"
#include "../../script/services.h"

namespace reone {

namespace engine {

class ResourceModule;

class ScriptModule : boost::noncopyable {
public:
    ScriptModule(ResourceModule &resource) :
        _resource(resource) {
    }

    ~ScriptModule() { deinit(); }

    void init();
    void deinit();

    script::Scripts &scripts() { return *_scripts; }

    script::ScriptServices &services() { return *_services; }

private:
    ResourceModule &_resource;

    std::unique_ptr<script::Scripts> _scripts;

    std::unique_ptr<script::ScriptServices> _services;
};

} // namespace engine

} // namespace reone
