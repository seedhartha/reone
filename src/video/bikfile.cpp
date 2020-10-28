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

#include "bikfile.h"

#include <stdexcept>

#include <boost/filesystem.hpp>

#include "video.h"

namespace fs = boost::filesystem;

using namespace std;

namespace reone {

namespace video {

BikFile::BikFile(const fs::path &path) : _path(path) {
}

void BikFile::load() {
    if (!fs::exists(_path)) {
        throw runtime_error("File not found: " + _path.string());
    }
    _video = make_shared<Video>();
    _video->init();
}

shared_ptr<Video> BikFile::video() const {
    return _video;
}

} // namespace video

} // namespace reone
