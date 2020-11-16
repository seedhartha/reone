/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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
#include <vector>

#include "../../resource/gfffile.h"

namespace reone {

namespace game {

class SoundBlueprint {
public:
    SoundBlueprint(const std::string &resRef);

    void load(const resource::GffStruct &uts);

    const std::string &tag() const;
    bool active() const;
    int priority() const;
    float maxDistance() const;
    float minDistance() const;
    bool continuous() const;
    float elevation() const;
    bool looping() const;
    bool positional() const;
    int interval() const;
    int volume() const;
    const std::vector<std::string> &sounds() const;

private:
    std::string _resRef;
    std::string _tag;
    bool _active { false };
    int _priority { 0 };
    float _maxDistance { 0.0f };
    float _minDistance { 0.0f };
    bool _continuous { false };
    float _elevation { 0.0f };
    bool _looping { false };
    bool _positional { false };
    int _interval { 0 };
    int _volume { 0 };
    std::vector<std::string> _sounds;

    SoundBlueprint(const SoundBlueprint &) = delete;
    SoundBlueprint &operator=(const SoundBlueprint &) = delete;
};

} // namespace game

} // namespace reone
