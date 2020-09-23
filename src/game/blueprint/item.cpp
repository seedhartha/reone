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
#include <boost/format.hpp>

#include "../../resources/resources.h"

using namespace std;

using namespace reone::render;
using namespace reone::resources;

namespace reone {

namespace game {

void ItemBlueprint::load(const string &resRef, const GffStruct &uti) {
    _resRef = resRef;

    ResourceManager &resources = ResMan;
    _localizedName = ResMan.getString(uti.getInt("LocalizedName")).text;

    shared_ptr<TwoDaTable> baseItems(resources.find2DA("baseitems"));
    int baseItem = uti.getInt("BaseItem");
    uint32_t itemType = baseItems->getUint(baseItem, "equipableslots", 0);

    string itemClass(baseItems->getString(baseItem, "itemclass"));
    _itemClass = boost::to_lower_copy(itemClass);

    string iconResRef;

    if ((itemType >> kInventorySlotBody) & 1) {
        const string &baseBodyVar = baseItems->getString(baseItem, "bodyvar");
        int bodyVariation = uti.getInt("BodyVariation", 1);
        int textureVar = uti.getInt("TextureVar", 1);

        _type = ItemType::Armor;
        _baseBodyVariation = boost::to_lower_copy(baseBodyVar);
        _bodyVariation = bodyVariation;
        _textureVariation = textureVar;

        iconResRef = str(boost::format("i%s_%03d") % _itemClass % textureVar);

    } else if ((itemType >> kInventorySlotRightWeapon) & 1) {
        int modelVariation = uti.getInt("ModelVariation", 1);

        _type = ItemType::RightWeapon;
        _modelVariation = modelVariation;

        iconResRef = str(boost::format("i%s_%03d") % _itemClass % modelVariation);

    } else {
        int modelVariation = uti.getInt("ModelVariation", 1);
        iconResRef = str(boost::format("i%s_%03d") % _itemClass % modelVariation);
    }

    _icon = ResMan.findTexture(iconResRef, TextureType::GUI);
}

const string &ItemBlueprint::resRef() const {
    return _resRef;
}

ItemType ItemBlueprint::type() const {
    return _type;
}

const string &ItemBlueprint::localizedName() const {
    return _localizedName;
}

const string &ItemBlueprint::baseBodyVariation() const {
    return _baseBodyVariation;
}

int ItemBlueprint::bodyVariation() const {
    return _bodyVariation;
}

int ItemBlueprint::textureVariation() const {
    return _textureVariation;
}

const string &ItemBlueprint::itemClass() const {
    return _itemClass;
}

int ItemBlueprint::modelVariation() const {
    return _modelVariation;
}

shared_ptr<Texture> ItemBlueprint::icon() const {
    return _icon;
}

} // namespace game

} // namespace reone
