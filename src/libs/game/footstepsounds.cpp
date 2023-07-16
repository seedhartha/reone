/*
 * Copyright (c) 2020-2023 The reone project contributors
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

#include "reone/game/footstepsounds.h"

#include "reone/audio/files.h"
#include "reone/resource/2da.h"
#include "reone/resource/2das.h"

using namespace reone::audio;
using namespace reone::resource;

namespace reone {

namespace game {

std::shared_ptr<FootstepTypeSounds> FootstepSounds::doGet(uint32_t type) {
    std::shared_ptr<FootstepTypeSounds> result;

    std::shared_ptr<TwoDa> twoDa(_twoDas.get("footstepsounds"));
    if (twoDa) {
        result = std::make_shared<FootstepTypeSounds>();
        std::map<std::string, std::vector<std::shared_ptr<AudioBuffer>> &> dict {
            {"dirt", result->dirt},
            {"grass", result->grass},
            {"stone", result->stone},
            {"wood", result->wood},
            {"water", result->water},
            {"carpet", result->carpet},
            {"metal", result->metal},
            {"leaves", result->leaves}};
        for (auto &pair : dict) {
            for (int i = 0; i < 3; ++i) {
                std::string key(str(boost::format("%s%d") % pair.first % i));
                std::string resRef(twoDa->getString(static_cast<int>(type), key));
                std::shared_ptr<AudioBuffer> audio(_audioFiles.get(resRef));
                pair.second.push_back(std::move(audio));
            }
        }
    }

    return result;
}

} // namespace game

} // namespace reone
