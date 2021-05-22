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

#include <memory>

#include <boost/noncopyable.hpp>

#include "../resource/services.h"

#include "scripts.h"

namespace reone {

namespace script {

class ScriptServices : boost::noncopyable {
public:
    ScriptServices(resource::ResourceServices &resource);

    void init();

    Scripts &scripts() { return *_scripts; }

private:
    resource::ResourceServices &_resource;

    std::unique_ptr<Scripts> _scripts;
};

} // namespace script

} // namespace reone
