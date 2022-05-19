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

#include "audio.h"

#include "../../common/binaryreader.h"
#include "../../common/stream/fileinput.h"


using namespace std;

namespace fs = boost::filesystem;

namespace reone {

void AudioTool::invoke(Operation operation, const fs::path &target, const fs::path &gamePath, const fs::path &destPath) {
    if (operation == Operation::Unwrap) {
        unwrap(target, destPath);
    }
}

void AudioTool::unwrap(const fs::path &path, const fs::path &destPath) {
    auto wav = FileInputStream(path, OpenMode::Binary);

    // Determine filesize
    wav.seek(0, SeekOrigin::End);
    size_t filesize = wav.position();
    wav.seek(0, SeekOrigin::Begin);

    string suffix;

    // Read magic number
    BinaryReader reader(wav);
    uint32_t magic = reader.getUint32();
    if (magic == 0xc460f3ff) { // WAV in MP3
        reader.seek(0x1d6);
        suffix = "-unwrap.wav";
    } else if (magic == 0x46464952) { // MP3 in WAV
        // Read subchunks
        reader.ignore(8); // chunk size + format
        while (!reader.eof()) {
            string subchunkId(reader.getString(4));
            uint32_t subchunkSize = reader.getInt32();
            if (subchunkId == "data") {
                break;
            } else {
                reader.ignore(subchunkSize);
            }
        }
        suffix = ".mp3";
    } else {
        throw runtime_error("Unsupported audio format");
    }

    int dataSize = static_cast<int>(filesize - reader.tell());
    ByteArray data(reader.getBytes(dataSize));

    fs::path unwrappedPath(path);
    unwrappedPath.replace_extension();
    unwrappedPath += suffix;

    fs::ofstream unwrapped(unwrappedPath, ios::binary);
    unwrapped.write(&data[0], data.size());
}

bool AudioTool::supports(Operation operation, const fs::path &target) const {
    return !fs::is_directory(target) &&
           target.extension() == ".wav" &&
           operation == Operation::Unwrap;
}

} // namespace reone
