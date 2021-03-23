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

#pragma once

#include <string>
#include <memory>
#include <vector>

#include <boost/noncopyable.hpp>

#include "../../resource/format/gffreader.h"

#include "../rp/attributes.h"

#include "blueprint.h"

namespace reone {

namespace game {

class Creature;

/**
 * Creature blueprint that loads data from a UTC file.
 */
class CreatureBlueprint : public Blueprint<Creature>, boost::noncopyable {
public:
    CreatureBlueprint(const std::string &resRef, const std::shared_ptr<resource::GffStruct> &utc);

    void load(Creature &creature) override;

    int getAppearanceFromUtc() const;

    const std::string &resRef() const { return _resRef; }

private:
    std::string _resRef;
    std::shared_ptr<resource::GffStruct> _utc;

    void loadName(Creature &creature);
    void loadAttributes(Creature &creature);
    void loadAbilities(CreatureAbilities &abilities);
    void loadSkills(CreatureSkills &skills);
    void loadScripts(Creature &creature);
    void loadItems(Creature &creature);
    void loadSoundSet(Creature &creature);
    void loadBodyBag(Creature &creature);
    void loadPerception(Creature &creature);
};

/**
 * Creature blueprint that has its data set manually.
 */
class StaticCreatureBlueprint : public Blueprint<Creature> {
public:
    void load(Creature &creature) override;

    void clearEquipment();
    void addEquippedItem(const std::string &resRef);

    Gender gender() const { return _gender; }
    int appearance() const { return _appearance; }
    CreatureAttributes &attributes() { return _attributes; }

    void setGender(Gender gender);
    void setAppearance(int appearance);
    void setAttributes(CreatureAttributes attributes);

private:
    Gender _gender { Gender::Male };
    int _appearance { 0 };
    CreatureAttributes _attributes;
    std::vector<std::string> _equipment;
};

} // namespace game

} // namespace reone
