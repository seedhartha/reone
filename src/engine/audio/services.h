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

#pragma once

#include <boost/noncopyable.hpp>

namespace reone {

namespace audio {

class AudioFiles;
class AudioPlayer;

class AudioServices : boost::noncopyable {
public:
    AudioServices(AudioFiles &files, AudioPlayer &player) : _files(files), _player(player) {
    }

    AudioFiles &files() { return _files; }
    AudioPlayer &player() { return _player; }

private:
    AudioFiles &_files;
    AudioPlayer &_player;
};

} // namespace audio

} // namespace reone
