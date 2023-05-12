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

#include "2das.h"
#include "gffs.h"
#include "resources.h"
#include "strings.h"

namespace reone {

namespace resource {

struct ResourceServices {
    IGffs &gffs;
    IResources &resources;
    IStrings &strings;
    ITwoDas &twoDas;

    ResourceServices(
        IGffs &gffs,
        IResources &resources,
        IStrings &strings,
        ITwoDas &twoDas) :
        gffs(gffs),
        resources(resources),
        strings(strings),
        twoDas(twoDas) {
    }

    Gffs &defaultGffs() {
        auto casted = dynamic_cast<Gffs *>(&gffs);
        if (!casted) {
            throw std::logic_error("Illegal Gffs implementation");
        }
        return *casted;
    }

    Resources &defaultResources() {
        auto casted = dynamic_cast<Resources *>(&resources);
        if (!casted) {
            throw std::logic_error("Illegal Resources implementation");
        }
        return *casted;
    }

    Strings &defaultStrings() {
        auto casted = dynamic_cast<Strings *>(&strings);
        if (!casted) {
            throw std::logic_error("Illegal Strings implementation");
        }
        return *casted;
    }

    TwoDas &defaultTwoDas() {
        auto casted = dynamic_cast<TwoDas *>(&twoDas);
        if (!casted) {
            throw std::logic_error("Illegal TwoDas implementation");
        }
        return *casted;
    }
};

} // namespace resource

} // namespace reone
