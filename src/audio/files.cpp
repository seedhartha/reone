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

#include "../common/streamutil.h"
#include "../resource/resources.h"

#include "format/mp3file.h"
#include "format/wavfile.h"

using namespace std;
using namespace std::placeholders;

using namespace reone::resource;

namespace reone {

namespace audio {

AudioFiles &AudioFiles::instance() {
    static AudioFiles instance;
    return instance;
}

AudioFiles::AudioFiles() : MemoryCache(bind(&AudioFiles::doGet, this, _1)) {
}

shared_ptr<AudioStream> AudioFiles::doGet(string resRef) {
    shared_ptr<AudioStream> result;

    shared_ptr<ByteArray> mp3Data(Resources::instance().get(resRef, ResourceType::Mp3, false));
    if (mp3Data) {
        Mp3File mp3;
        mp3.load(wrap(mp3Data));
        result = mp3.stream();
    }
    if (!result) {
        shared_ptr<ByteArray> wavData(Resources::instance().get(resRef, ResourceType::Wav));
        if (wavData) {
            WavFile wav;
            wav.load(wrap(wavData));
            result = wav.stream();
        }
    }

    return move(result);
}

} // namespace audio

} // namespace reone
