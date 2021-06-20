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

#include "skills.h"

#include "../../common/collectionutil.h"
#include "../../resource/2da.h"
#include "../../resource/strings.h"

using namespace std;

using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

Skills::Skills(GraphicsServices &graphics, ResourceServices &resource) : _graphics(graphics), _resource(resource) {
}

void Skills::init() {
    shared_ptr<TwoDA> skills(_resource.resources().get2DA("skills"));
    if (!skills) return;

    for (int row = 0; row < skills->getRowCount(); ++row) {
        string name(_resource.strings().get(skills->getInt(row, "name", -1)));
        string description(_resource.strings().get(skills->getInt(row, "description", -1)));
        shared_ptr<Texture> icon(_graphics.textures().get(skills->getString(row, "icon"), TextureUsage::GUI));

        auto skill = make_shared<Skill>();
        skill->name = move(name);
        skill->description = move(description);
        skill->icon = move(icon);
        _skills.insert(make_pair(static_cast<SkillType>(row), move(skill)));
    }
}

shared_ptr<Skill> Skills::get(SkillType type) const {
    return getFromLookupOrNull(_skills, type);
}

} // namespace game

} // namespace reone
