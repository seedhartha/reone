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

#include <cstdint>
#include <memory>

#include "../common/mediastream.h"
#include "../common/types.h"

namespace reone {

namespace audio {

class AudioStream;

}

namespace video {

class BikFile;

class Video {
public:
    struct Frame {
        ByteArray data;
    };

    void init();
    void deinit();

    void update(float dt);
    void render();

    void finish();

    bool isFinished() const;

    std::shared_ptr<audio::AudioStream> audio() const;

    void setMediaStream(const std::shared_ptr<MediaStream<Frame>> &stream);

private:
    int _width { 0 };
    int _height { 0 };
    float _fps { 0.0f };
    std::shared_ptr<Frame> _frame;
    bool _inited { false };
    uint32_t _textureId { 0 };
    float _time { 0.0f };
    bool _finished { false };
    std::shared_ptr<MediaStream<Frame>> _stream;
    std::shared_ptr<audio::AudioStream> _audio;

    void updateFrame(float dt);
    void updateFrameTexture();

    friend class BinkVideoDecoder;
};

} // namespace video

} // namespace reone
