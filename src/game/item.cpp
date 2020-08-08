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

#include "item.h"

#include <boost/algorithm/string.hpp>

#include "../resources/manager.h"

using namespace std;

using namespace reone::resources;

namespace reone {

namespace game {

void Item::load(const string &resRef, const GffStruct &uti) {
    _resRef = resRef;

    ResourceManager &resources = ResourceManager::instance();
    shared_ptr<TwoDaTable> baseItems(resources.find2DA("baseitems"));

    int baseItem = uti.getInt("BaseItem");
    uint32_t itemType = baseItems->getUint(baseItem, "equipableslots", 0);

    if ((itemType >> kInventorySlotBody) & 1) {
        const string &baseBodyVar = baseItems->getString(baseItem, "bodyvar");
        int bodyVariation = uti.getInt("BodyVariation", 1);
        int textureVar = uti.getInt("TextureVar", 1);

        _type = ItemType::Armor;
        _baseBodyVariation = boost::to_lower_copy(baseBodyVar);
        _bodyVariation = bodyVariation;
        _textureVariation = textureVar;

    } else if ((itemType >> kInventorySlotRightWeapon) & 1) {
        const string &itemClass = baseItems->getString(baseItem, "itemclass");
        int modelVariation = uti.getInt("ModelVariation", 1);

        _type = ItemType::RightWeapon;
        _itemClass = itemClass;
        _modelVariation = modelVariation;
    }
}

const string &Item::resRef() const {
    return _resRef;
}

ItemType Item::type() const {
    return _type;
}

const string &Item::baseBodyVariation() const {
    return _baseBodyVariation;
}

int Item::bodyVariation() const {
    return _bodyVariation;
}

int Item::textureVariation() const {
    return _textureVariation;
}

const string &Item::itemClass() const {
    return _itemClass;
}

int Item::modelVariation() const {
    return _modelVariation;
}

} // namespace game

} // namespace reone
