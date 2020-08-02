#include "item.h"

#include <boost/algorithm/string.hpp>

#include "../resources/manager.h"

using namespace reone::resources;

namespace reone {

namespace game {

void Item::load(const std::string &resRef, const GffStruct &uti) {
    _resRef = resRef;

    ResourceManager &resources = ResourceManager::instance();
    std::shared_ptr<TwoDaTable> baseItems(resources.find2DA("baseitems"));

    int baseItem = uti.getInt("BaseItem");
    uint32_t itemType = baseItems->getUint(baseItem, "equipableslots", 0);

    if ((itemType >> kInventorySlotBody) & 1) {
        const std::string &baseBodyVar = baseItems->getString(baseItem, "bodyvar");
        int bodyVariation = uti.getInt("BodyVariation", 1);
        int textureVar = uti.getInt("TextureVar", 1);

        _type = ItemType::Armor;
        _baseBodyVariation = boost::to_lower_copy(baseBodyVar);
        _bodyVariation = bodyVariation;
        _textureVariation = textureVar;

    } else if ((itemType >> kInventorySlotRightWeapon) & 1) {
        const std::string &itemClass = baseItems->getString(baseItem, "itemclass");
        int modelVariation = uti.getInt("ModelVariation", 1);

        _type = ItemType::RightWeapon;
        _itemClass = itemClass;
        _modelVariation = modelVariation;
    }
}

const std::string &Item::resRef() const {
    return _resRef;
}

ItemType Item::type() const {
    return _type;
}

const std::string &Item::baseBodyVariation() const {
    return _baseBodyVariation;
}

int Item::bodyVariation() const {
    return _bodyVariation;
}

int Item::textureVariation() const {
    return _textureVariation;
}

const std::string &Item::itemClass() const {
    return _itemClass;
}

int Item::modelVariation() const {
    return _modelVariation;
}

} // namespace game

} // namespace reone
