/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../object.h"

namespace reone {

namespace game {

class IItem {
};

class Item : public IItem, public Object {
public:
    Item(
        uint32_t id,
        IGame &game,
        IObjectFactory &objectFactory,
        GameServices &gameSvc,
        graphics::GraphicsOptions &graphicsOpt,
        graphics::GraphicsServices &graphicsSvc,
        resource::ResourceServices &resourceSvc) :
        Object(
            id,
            ObjectType::Item,
            game,
            objectFactory,
            gameSvc,
            graphicsOpt,
            graphicsSvc,
            resourceSvc) {
    }

    void loadFromUti(const std::string &templateResRef);

    bool isEquipable(int slot) const {
        return (_equipableSlots & (1 << slot)) != 0;
    }

    int equipableSlots() const {
        return _equipableSlots;
    }
    
    int bodyVariation() const {
        return _bodyVariation;
    }

    int textureVar() const {
        return _textureVar;
    }

private:
    int _equipableSlots {0};
    int _bodyVariation {1};
    int _textureVar {1};
};

} // namespace game

} // namespace reone
