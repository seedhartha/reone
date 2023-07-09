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

#include "reone/tools/audio.h"

#include "reone/resource/exception/format.h"
#include "reone/system/binaryreader.h"
#include "reone/system/exception/endofstream.h"
#include "reone/system/stream/fileinput.h"

using namespace reone::resource;

namespace reone {

void AudioTool::invoke(Operation operation, const boost::filesystem::path &input, const boost::filesystem::path &outputDir, const boost::filesystem::path &gamePath) {
    if (operation == Operation::Unwrap) {
        unwrap(input, outputDir);
    }
}

void AudioTool::unwrap(const boost::filesystem::path &path, const boost::filesystem::path &destPath) {
    auto wav = FileInputStream(path);
    BinaryReader reader(wav);
    size_t filesize = reader.length();
    std::string suffix;

    // Read magic number
    uint32_t magic = reader.readUint32();
    if (magic == 0xc460f3ff) { // WAV in MP3
        reader.seek(0x1d6);
        suffix = "-unwrap.wav";
    } else if (magic == 0x46464952) { // MP3 in WAV
        // Read subchunks
        reader.ignore(8); // chunk size + format
        while (true) {
            std::string subchunkId;
            try {
                subchunkId = reader.readString(4);
            } catch (const EndOfStreamException &ignored) {
                break;
            }
            uint32_t subchunkSize = reader.readInt32();
            if (subchunkId == "data") {
                break;
            } else {
                reader.ignore(subchunkSize);
            }
        }
        suffix = ".mp3";
    } else {
        throw FormatException("Unsupported audio format");
    }

    int dataSize = static_cast<int>(filesize - reader.position());
    ByteArray data(reader.readBytes(dataSize));

    boost::filesystem::path unwrappedPath(path);
    unwrappedPath.replace_extension();
    unwrappedPath += suffix;

    boost::filesystem::ofstream unwrapped(unwrappedPath, std::ios::binary);
    unwrapped.write(&data[0], data.size());
}

bool AudioTool::supports(Operation operation, const boost::filesystem::path &input) const {
    return !boost::filesystem::is_directory(input) &&
           input.extension() == ".wav" &&
           operation == Operation::Unwrap;
}

} // namespace reone
