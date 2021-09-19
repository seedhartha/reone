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

#include "../types.h"

#include "skill.h"

namespace reone {

namespace resource {

class ResourceServices;

}

namespace graphics {

class GraphicsServices;

}

namespace game {

class Skills : boost::noncopyable {
public:
    Skills(graphics::GraphicsServices &graphics, resource::ResourceServices &resource);

    void init();

    std::shared_ptr<Skill> get(SkillType type) const;

private:
    graphics::GraphicsServices &_graphics;
    resource::ResourceServices &_resource;

    std::unordered_map<SkillType, std::shared_ptr<Skill>> _skills;
};

} // namespace game

} // namespace reone
