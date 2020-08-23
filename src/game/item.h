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
    const std::string &localizedName() const;
    const std::string &baseBodyVariation() const;
    int bodyVariation() const;
    int textureVariation() const;
    const std::string &itemClass() const;
    int modelVariation() const;
    std::shared_ptr<render::Texture> icon() const;

private:
    std::string _resRef;
    ItemType _type { ItemType::None };
    std::string _localizedName;
    std::string _baseBodyVariation;
    int _bodyVariation { 0 };
    int _textureVariation { 0 };
    std::string _itemClass;
    int _modelVariation { 0 };
    std::shared_ptr<render::Texture> _icon;

    Item(const Item &) = delete;
    Item &operator=(const Item &) = delete;
};

} // namespace game

} // namespace reone
