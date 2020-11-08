/*
 * Copyright (c) 2020 Vsevolod Kremianskii
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

#include "bikfile.h"

#include <stdexcept>

#include <boost/filesystem.hpp>

#include "../system/log.h"
#include "../system/streamreader.h"

#include "video.h"

namespace fs = boost::filesystem;

using namespace std;

namespace reone {

namespace video {

static const char kSignature[] = "BIKi";

BikFile::BikFile(const fs::path &path) : _path(path) {
}

void BikFile::load() {
    if (!fs::exists(_path)) {
        throw runtime_error("File not found: " + _path.string());
    }
    shared_ptr<ifstream> stream(new fs::ifstream(_path, ios::binary));
    StreamReader reader(stream);

    string sign(reader.getString(4));
    if (sign != kSignature) {
        throw runtime_error("Invalid BIK file signature");
    }

    uint32_t fileSize = reader.getUint32();
    uint32_t frameCount = reader.getUint32();
    uint32_t largestFrameSize = reader.getUint32();

    reader.ignore(4);

    uint32_t width = reader.getUint32();
    uint32_t height = reader.getUint32();
    uint32_t fpsNumerator = reader.getUint32();
    uint32_t fpsDenominator = reader.getUint32();
    uint32_t videoFlags = reader.getUint32();

    uint32_t audioTrackCount = reader.getUint32();
    if (audioTrackCount > 0) {
        reader.ignore(12 * audioTrackCount);
    }

    vector<FrameDescriptor> descriptors;
    for (uint32_t i = 0; i <= frameCount; ++i) {
        uint32_t value = reader.getUint32();
        uint32_t offset = value & ~1;

        FrameDescriptor desc;
        desc.offset = offset;
        desc.keyframe = (value & 1) != 0;
        if (i > 0) {
            descriptors[i - 1].size = offset - descriptors[i - 1].offset;
        }
        if (i < frameCount) {
            descriptors.push_back(move(desc));
        }
    }

    _video = make_shared<Video>();
    _video->_width = width;
    _video->_height = height;
    _video->_fps = fpsNumerator / static_cast<float>(fpsDenominator);

    for (auto &desc : descriptors) {
        reader.seek(desc.offset);

        ByteArray data(reader.getArray<char>(static_cast<int>(desc.size)));
        data.resize(3 * static_cast<size_t>(width) * height);
        memset(&data[0], 0, data.size());

        Video::Frame frame;
        frame.data = move(data);
        _video->_frames.push_back(move(frame));
    }

    _video->init();
}

shared_ptr<Video> BikFile::video() const {
    return _video;
}

} // namespace video

} // namespace reone
