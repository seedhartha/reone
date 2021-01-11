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

#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace reone {

template <class Frame>
class MediaStream {
public:
    static const int kBufferSize = 8;

    virtual ~MediaStream() {
    }

    std::shared_ptr<Frame> get(int frame) {
        int relFrame = frame - _frameOffset;
        if (relFrame < 0) {
            throw std::logic_error("frame was unloaded: " + std::to_string(frame));
        }
        if (relFrame < static_cast<int>(_frames.size())) {
            return _frames[relFrame];
        }
        if (_ended) {
            return nullptr;
        }
        _frames.clear();

        int nextFrame = _frameOffset + kBufferSize;
        int framesToIgnore = std::max(0, frame - nextFrame);
        ignoreFrames(framesToIgnore);
        fetchFrames(kBufferSize);

        _frameOffset = frame;

        return get(frame);
    }

protected:
    MediaStream() = default;

    virtual void ignoreFrames(int count) = 0;
    virtual void fetchFrames(int count) = 0;

    std::vector<std::shared_ptr<Frame>> _frames;
    bool _ended { false };

private:
    MediaStream(const MediaStream &) = delete;
    MediaStream &operator=(const MediaStream &) = delete;

    int _frameOffset { 0 };
};

} // namespace reone
