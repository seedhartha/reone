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

namespace reone {

namespace video {

class Video;

class BikFile {
public:
    BikFile(const boost::filesystem::path &path);

    void load();

    std::shared_ptr<Video> video() const;

private:
    struct FrameDescriptor {
        uint32_t offset { 0 };
        bool keyframe { false };
        size_t size { 0 };
    };

    boost::filesystem::path _path;
    std::shared_ptr<Video> _video;
};

} // namespace video

} // namespace reone
