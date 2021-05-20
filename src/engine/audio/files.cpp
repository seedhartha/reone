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

#include "files.h"

#include "../common/guardutil.h"
#include "../common/streamutil.h"
#include "../resource/resources.h"

#include "format/mp3reader.h"
#include "format/wavreader.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::resource;

namespace reone {

namespace audio {

AudioFiles::AudioFiles(Resources *resources) :
    MemoryCache(bind(&AudioFiles::doGet, this, _1)),
    _resources(resources) {

    ensureNotNull(resources, "resources");
}

shared_ptr<AudioStream> AudioFiles::doGet(string resRef) {
    shared_ptr<AudioStream> result;

    shared_ptr<ByteArray> mp3Data(_resources->getRaw(resRef, ResourceType::Mp3, false));
    if (mp3Data) {
        Mp3Reader mp3;
        mp3.load(wrap(mp3Data));
        result = mp3.stream();
    }
    if (!result) {
        shared_ptr<ByteArray> wavData(_resources->getRaw(resRef, ResourceType::Wav));
        if (wavData) {
            WavReader wav;
            wav.load(wrap(wavData));
            result = wav.stream();
        }
    }

    return move(result);
}

} // namespace audio

} // namespace reone
