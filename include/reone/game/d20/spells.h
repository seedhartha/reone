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

#include "../types.h"

#include "spell.h"

namespace reone {

namespace resource {

class Strings;
class TwoDas;
class Textures;

} // namespace resource

namespace game {

class ISpells {
public:
    virtual ~ISpells() = default;

    virtual std::shared_ptr<Spell> get(SpellType type) const = 0;
};

class Spells : public ISpells, boost::noncopyable {
public:
    Spells(
        resource::Textures &textures,
        resource::Strings &strings,
        resource::TwoDas &twoDas) :
        _textures(textures),
        _strings(strings),
        _twoDas(twoDas) {
    }

    void init();

    std::shared_ptr<Spell> get(SpellType type) const override;

private:
    std::unordered_map<SpellType, std::shared_ptr<Spell>> _spells;

    // Services

    resource::Textures &_textures;
    resource::Strings &_strings;
    resource::TwoDas &_twoDas;

    // END Services
};

} // namespace game

} // namespace reone
