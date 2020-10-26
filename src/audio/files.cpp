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

#include "files.h"

#include "../resource/resources.h"
#include "../system/streamutil.h"

#include "mp3file.h"
#include "wavfile.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace audio {

AudioFiles &AudioFiles::instance() {
    static AudioFiles instance;
    return instance;
}

void AudioFiles::invalidateCache() {
    _cache.clear();
}

shared_ptr<AudioStream> AudioFiles::get(const string &resRef) {
    auto maybeAudio = _cache.find(resRef);
    if (maybeAudio != _cache.end()) {
        return maybeAudio->second;
    }
    auto inserted = _cache.insert(make_pair(resRef, doGet(resRef)));

    return inserted.first->second;
}

shared_ptr<AudioStream> AudioFiles::doGet(const string &resRef) {
    shared_ptr<ByteArray> mp3Data(Resources::instance().findRaw(resRef, ResourceType::Mp3, false));
    shared_ptr<AudioStream> stream;

    if (mp3Data) {
        Mp3File mp3;
        mp3.load(wrap(mp3Data));
        stream = mp3.stream();

    } else {
        shared_ptr<ByteArray> wavData(Resources::instance().findRaw(resRef, ResourceType::Wav));
        if (wavData) {
            WavFile wav;
            wav.load(wrap(wavData));
            stream = wav.stream();
        }
    }

    return move(stream);
}

} // namespace audio

} // namespace reone
