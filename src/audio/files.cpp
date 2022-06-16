/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include "../common/stream/bytearrayinput.h"
#include "../resource/resources.h"

#include "format/mp3reader.h"
#include "format/wavreader.h"
#include "stream.h"

using namespace std;

using namespace reone::resource;

namespace reone {

namespace audio {

shared_ptr<AudioStream> AudioFiles::doGet(string resRef) {
    shared_ptr<AudioStream> result;

    shared_ptr<ByteArray> mp3Data(_resources.get(resRef, ResourceType::Mp3, false));
    if (mp3Data) {
        auto mp3 = ByteArrayInputStream(*mp3Data);
        auto reader = Mp3Reader();
        reader.load(mp3);
        result = reader.stream();
    }
    if (!result) {
        shared_ptr<ByteArray> wavData(_resources.get(resRef, ResourceType::Wav));
        if (wavData) {
            auto wav = ByteArrayInputStream(*wavData);
            auto reader = WavReader(Mp3ReaderFactory());
            reader.load(wav);
            result = reader.stream();
        }
    }

    return move(result);
}

} // namespace audio

} // namespace reone
