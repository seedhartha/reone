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

#include "../../resource/2das.h"
#include "../../resource/gffs.h"
#include "../../resource/resources.h"
#include "../../resource/strings.h"

namespace reone {

class ResourceModule : boost::noncopyable {
public:
    ResourceModule(boost::filesystem::path gamePath) :
        _gamePath(std::move(gamePath)) {
    }

    ~ResourceModule() { deinit(); }

    void init();
    void deinit();

    resource::Gffs &gffs() { return *_gffs; }
    resource::Resources &resources() { return *_resources; }
    resource::Strings &strings() { return *_strings; }
    resource::TwoDas &twoDas() { return *_twoDas; }

private:
    boost::filesystem::path _gamePath;

    std::unique_ptr<resource::Gffs> _gffs;
    std::unique_ptr<resource::Resources> _resources;
    std::unique_ptr<resource::Strings> _strings;
    std::unique_ptr<resource::TwoDas> _twoDas;
};

} // namespace reone
