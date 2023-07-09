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

#pragma once

namespace reone {

namespace movie {

class VideoStream {
public:
    struct Frame {
        std::shared_ptr<ByteBuffer> pixels;
    };

    virtual void seek(float time) = 0;

    bool hasEnded() const { return _ended; }

    int width() const { return _width; }
    int height() const { return _height; }
    const Frame &frame() const { return _frame; }

protected:
    int _width {0};
    int _height {0};

    Frame _frame;

    bool _ended {false};
};

} // namespace movie

} // namespace reone
