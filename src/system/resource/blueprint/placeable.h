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

#include <string>
#include <unordered_map>
#include <vector>

#include "../gfffile.h"

namespace reone {

namespace resource {

class PlaceableBlueprint {
public:
    enum class ScriptType {
        OnInvDisturbed
    };

    PlaceableBlueprint() = default;

    void load(const GffStruct &utp);

    bool getScript(ScriptType type, std::string &resRef) const;

    const std::string &tag() const;
    int appearance() const;
    bool hasInventory() const;
    bool isUsable() const;
    const std::vector<std::string> &items() const;

private:
    std::string _tag;
    int _appearance { 0 };
    bool _hasInventory { false };
    bool _usable { false };
    std::vector<std::string> _items;
    std::unordered_map<ScriptType, std::string> _scripts;

    PlaceableBlueprint(const PlaceableBlueprint &) = delete;
    PlaceableBlueprint &operator=(const PlaceableBlueprint &) = delete;

    void loadItems(const GffStruct &utp);
    void loadScripts(const GffStruct &utp);
};

} // namespace resource

} // namespace reone
