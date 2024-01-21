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

#include "reone/resource/provider/audioclips.h"

#include "reone/audio/clip.h"
#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/resource/resources.h"
#include "reone/system/stream/memoryinput.h"

using namespace reone::audio;

namespace reone {

namespace resource {

std::shared_ptr<AudioClip> AudioClips::doGet(std::string resRef) {
    std::shared_ptr<AudioClip> clip;
    auto m3pRes = _resources.find(ResourceId(resRef, ResType::Mp3));
    if (m3pRes) {
        auto stream = MemoryInputStream(m3pRes->data);
        auto reader = Mp3Reader();
        reader.load(stream);
        clip = reader.stream();
    }
    if (!clip) {
        auto wavRes = _resources.find(ResourceId(resRef, ResType::Wav));
        if (wavRes) {
            auto stream = MemoryInputStream(wavRes->data);
            auto mp3ReaderFactory = Mp3ReaderFactory();
            auto reader = WavReader(stream, mp3ReaderFactory);
            reader.load();
            clip = reader.stream();
        }
    }
    return clip;
}

} // namespace resource

} // namespace reone
