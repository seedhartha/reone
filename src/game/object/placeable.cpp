/*
 * Copyright � 2020 Vsevolod Kremianskii
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
#include "../../render/scene/modelnode.h"
#include "../../resources/resources.h"

#include "../blueprint/blueprints.h"

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

    string templResRef(gffs.getString("TemplateResRef"));
    boost::to_lower(templResRef);

    loadBlueprint(templResRef);
    updateTransform();
}

void Placeable::loadBlueprint(const string &resRef) {
    _blueprint = Blueprints.findPlaceable(resRef);
    _tag = _blueprint->tag();

    shared_ptr<TwoDaTable> table(Resources.find2DA("placeables"));

    string model(table->getString(_blueprint->appearance(), "modelname"));
    boost::to_lower(model);

    _model = make_unique<ModelSceneNode>(Resources.findModel(model));
    _model->setLightingEnabled(true);

    _walkmesh = Resources.findWalkmesh(model, ResourceType::PlaceableWalkmesh);

    for (auto &itemResRef : _blueprint->items()) {
        shared_ptr<ItemBlueprint> itemTempl(Blueprints.findItem(itemResRef));

        shared_ptr<Item> item(_objectFactory->newItem());
        item->load(itemTempl.get());

        _items.push_back(move(item));
    }
}

const vector<shared_ptr<Item>> &Placeable::items() const {
    return _items;
}

} // namespace game

} // namespace reone
