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

#include "reone/game/soundsets.h"

#include "reone/audio/files.h"
#include "reone/resource/format/ssfreader.h"
#include "reone/resource/resources.h"
#include "reone/resource/strings.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::audio;
using namespace reone::resource;

namespace reone {

namespace game {

std::shared_ptr<SoundSet> SoundSets::doGet(std::string resRef) {
    auto res = _resources.find(ResourceId(resRef, ResType::Ssf));
    if (!res) {
        return nullptr;
    }
    auto stream = MemoryInputStream(res->data);
    auto result = std::make_shared<SoundSet>();

    SsfReader ssf(stream);
    ssf.load();

    std::vector<int> sounds(ssf.soundSet());
    for (size_t i = 0; i < sounds.size(); ++i) {
        std::string soundResRef(boost::to_lower_copy(_strings.getSound(sounds[i])));
        std::shared_ptr<AudioBuffer> sound(_audioFiles.get(soundResRef));
        if (sound) {
            result->insert(std::make_pair(static_cast<SoundSetEntry>(i), sound));
        }
    }

    return result;
}

} // namespace game

} // namespace reone
