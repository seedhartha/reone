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

#pragma once

#include <boost/filesystem/path.hpp>

#include "../audio/types.h"

namespace reone {

namespace video {

class Video;

/**
 * Represents a Bink Video file. Uses FFmpeg for loading.
 *
 * http://dranger.com/ffmpeg/ffmpeg.html
 */
class BikFile {
public:
    BikFile(const boost::filesystem::path &path);

    void load();

    std::shared_ptr<Video> video() const;

private:
    boost::filesystem::path _path;
    std::shared_ptr<Video> _video;
};

} // namespace video

} // namespace reone
