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

#include "skill.h"

namespace reone {

namespace resource {

class Strings;
class TwoDAs;
class Textures;

} // namespace resource

namespace game {

class ISkills {
public:
    virtual ~ISkills() = default;

    virtual std::shared_ptr<Skill> get(SkillType type) const = 0;
};

class Skills : public ISkills, boost::noncopyable {
public:
    Skills(
        resource::Textures &textures,
        resource::Strings &strings,
        resource::TwoDAs &twoDas) :
        _textures(textures),
        _strings(strings),
        _twoDas(twoDas) {
    }

    void init();

    std::shared_ptr<Skill> get(SkillType type) const override;

private:
    std::unordered_map<SkillType, std::shared_ptr<Skill>> _skills;

    // Services

    resource::Textures &_textures;
    resource::Strings &_strings;
    resource::TwoDAs &_twoDas;

    // END Services
};

} // namespace game

} // namespace reone
