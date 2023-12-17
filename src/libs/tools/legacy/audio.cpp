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

#include "reone/tools/legacy/audio.h"

#include "reone/system/binaryreader.h"
#include "reone/system/exception/endofstream.h"
#include "reone/system/exception/validation.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryoutput.h"

using namespace reone::resource;

namespace reone {

void AudioTool::invoke(Operation operation, const std::filesystem::path &input, const std::filesystem::path &outputDir, const std::filesystem::path &gamePath) {
    throw NotImplementedException();
}

void AudioTool::unwrap(IInputStream &wav, IOutputStream &unwrapped, ResType &actualType) {
    actualType = ResType::Invalid;

    BinaryReader reader(wav);
    size_t filesize = reader.length();
    std::string suffix;

    // Read magic number
    uint32_t magic = reader.readUint32();
    if (magic == 0xc460f3ff) { // WAV in MP3
        actualType = ResType::Wav;
        reader.seek(0x1d6);
    } else if (magic == 0x46464952) { // regular WAV or MP3 in WAV
        // Read subchunks
        reader.skipBytes(8); // chunk size + format
        while (true) {
            std::string subchunkId = reader.readString(4);
            uint32_t subchunkSize = reader.readInt32();
            if (subchunkId == "data") {
                if (subchunkSize == 0) { // MP3 in WAV
                    actualType = ResType::Mp3;
                    break;
                } else { // regular WAV
                    actualType = ResType::Wav;
                    reader.seek(0);
                    break;
                }
            } else {
                reader.skipBytes(subchunkSize);
            }
        }
    } else {
        throw ValidationException("Unsupported audio format");
    }

    int dataSize = static_cast<int>(filesize - reader.position());
    auto data = reader.readBytes(dataSize);
    unwrapped.write(&data[0], data.size());
}

bool AudioTool::supports(Operation operation, const std::filesystem::path &input) const {
    return !std::filesystem::is_directory(input) &&
           input.extension().string() == ".wav" &&
           operation == Operation::Unwrap;
}

} // namespace reone
