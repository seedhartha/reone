/*
 * Copyright (c) 2020 The reone project contributors
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

#include "../../resource/gfffile.h"

namespace reone {

namespace game {

class TriggerBlueprint {
public:
    TriggerBlueprint(const std::string &resRef);

    void load(const resource::GffStruct &utt);

    const std::string &tag() const;
    const std::string &onEnter() const;
    const std::string &onExit() const;

private:
    std::string _resRef;
    std::string _tag;
    std::string _onEnter;
    std::string _onExit;

    TriggerBlueprint(const TriggerBlueprint &) = delete;
    TriggerBlueprint &operator=(const TriggerBlueprint &) = delete;
};

} // namespace game

} // namespace reone
