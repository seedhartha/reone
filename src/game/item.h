#pragma once

#include "../resources/gfffile.h"

#include "types.h"

namespace reone {

namespace game {

class Item {
public:
    Item() = default;

    void load(const std::string &resRef, const resources::GffStruct &uti);

    const std::string &resRef() const;
    ItemType type() const;
    const std::string &baseBodyVariation() const;
    int bodyVariation() const;
    int textureVariation() const;
    const std::string &itemClass() const;
    int modelVariation() const;

private:
    std::string _resRef;
    ItemType _type { ItemType::Armor };
    std::string _baseBodyVariation;
    int _bodyVariation { 0 };
    int _textureVariation { 0 };
    std::string _itemClass;
    int _modelVariation { 0 };

    Item(const Item &) = delete;
    Item &operator=(const Item &) = delete;
};

} // namespace game

} // namespace reone
