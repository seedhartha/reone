/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "placeable.h"

#include <boost/algorithm/string.hpp>

#include "../../core/streamutil.h"
#include "../../render/modelinstance.h"
#include "../../resources/manager.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

Placeable::Placeable(uint32_t id) : Object(id) {
    _type = ObjectType::Placeable;
    _drawDistance = 4096.0f;
    _fadeDistance = 0.25f * _drawDistance;
}

void Placeable::load(const GffStruct &gffs) {
    _position[0] = gffs.getFloat("X");
    _position[1] = gffs.getFloat("Y");
    _position[2] = gffs.getFloat("Z");

    _heading = gffs.getFloat("Bearing");

    updateTransform();

    string templResRef(gffs.getString("TemplateResRef"));
    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<GffStruct> utp(resources.findGFF(templResRef, ResourceType::PlaceableBlueprint));
    loadBlueprint(*utp);
}

void Placeable::loadBlueprint(const resources::GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<TwoDaTable> table = resources.find2DA("placeables");

    int appearance = gffs.getInt("Appearance");
    string model(table->getString(appearance, "modelname"));
    boost::to_lower(model);

    _model = make_unique<ModelInstance>(resources.findModel(model));
    _walkmesh = resources.findWalkmesh(model, ResourceType::PlaceableWalkmesh);
}

} // namespace game

} // namespace reone
