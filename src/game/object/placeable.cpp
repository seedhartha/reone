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
#include "../../resources/resources.h"

#include "../template/templates.h"

#include "factory.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

Placeable::Placeable(uint32_t id, ObjectFactory *objectFactory) :
    SpatialObject(id, ObjectType::Placeable),
    _objectFactory(objectFactory) {

    assert(_objectFactory);
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
    shared_ptr<GffStruct> utp(ResMan.findGFF(templResRef, ResourceType::PlaceableBlueprint));
    loadBlueprint(*utp);
}

void Placeable::loadBlueprint(const GffStruct &gffs) {
    _tag = gffs.getString("Tag");
    boost::to_lower(_tag);

    shared_ptr<TwoDaTable> table(ResMan.find2DA("placeables"));

    int appearance = gffs.getInt("Appearance");
    string model(table->getString(appearance, "modelname"));
    boost::to_lower(model);

    _model = make_unique<ModelInstance>(ResMan.findModel(model));
    _walkmesh = ResMan.findWalkmesh(model, ResourceType::PlaceableWalkmesh);

    const GffField *itemList = gffs.find("ItemList");
    if (itemList) {
        for (auto &itemGffs : itemList->children()) {
            string resRef(itemGffs.getString("InventoryRes"));
            shared_ptr<ItemTemplate> itemTempl(TemplateMan.findItem(resRef));

            shared_ptr<Item> item(_objectFactory->newItem());
            item->load(itemTempl.get());

            _items.push_back(move(item));
        }
    }
}

const vector<shared_ptr<Item>> &Placeable::items() const {
    return _items;
}

} // namespace game

} // namespace reone
