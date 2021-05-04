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

#include "item.h"

#include <stdexcept>

#include "../../audio/player.h"
#include "../../resource/resources.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

namespace game {

Item::Item(uint32_t id) : Object(id, ObjectType::Item) {
}

void Item::loadFromBlueprint(const string &resRef) {
    shared_ptr<GffStruct> uti(Resources::instance().getGFF(resRef, ResourceType::Uti));
    if (uti) {
        loadUTI(*uti);
    }
}

void Item::playShotSound(int variant, glm::vec3 position) {
    if (_ammunitionType) {
        shared_ptr<AudioStream> sound(variant == 1 ? _ammunitionType->shotSound2 : _ammunitionType->shotSound1);
        if (sound) {
            AudioPlayer::instance().play(sound, AudioType::Sound, false, 1.0f, true, move(position));
        }
    }
}

void Item::playImpactSound(int variant, glm::vec3 position) {
    if (_ammunitionType) {
        shared_ptr<AudioStream> sound(variant == 1 ? _ammunitionType->impactSound2 : _ammunitionType->impactSound1);
        AudioPlayer::instance().play(sound, AudioType::Sound, false, 1.0f, true, move(position));
    }
}

bool Item::isEquippable() const {
    return _equipableSlots != 0;
}

bool Item::isEquippable(int slot) const {
    return (_equipableSlots >> slot) & 1;
}

void Item::setDropable(bool dropable) {
    _dropable = dropable;
}

void Item::setStackSize(int stackSize) {
    _stackSize = stackSize;
}

void Item::setIdentified(bool value) {
    _identified = value;
}

void Item::setEquipped(bool equipped) {
    _equipped = equipped;
}

} // namespace game

} // namespace reone
