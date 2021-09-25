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

#include "../common/mediastream.h"
#include "../common/types.h"

namespace reone {

namespace audio {

class AudioStream;

}

namespace graphics {

class Texture;

}

namespace di {

class GraphicsServices;

}

namespace video {

class BikReader;

class Video {
public:
    struct Frame {
        std::shared_ptr<ByteArray> pixels;
    };

    Video(di::GraphicsServices &graphics);

    void init();
    void deinit();

    void update(float dt);
    void draw();

    void finish() { _finished = true; }

    bool isFinished() const { return _finished; }

    std::shared_ptr<audio::AudioStream> audio() const { return _audio; }

    void setMediaStream(std::shared_ptr<MediaStream<Frame>> stream) { _stream = std::move(stream); }

private:
    di::GraphicsServices &_graphics;

    int _width { 0 };
    int _height { 0 };
    float _fps { 0.0f };
    std::shared_ptr<MediaStream<Frame>> _stream;

    bool _inited { false };
    float _time { 0.0f };
    std::shared_ptr<Frame> _frame;
    bool _finished { false };

    std::shared_ptr<graphics::Texture> _texture;
    std::shared_ptr<audio::AudioStream> _audio;

    void updateFrame(float dt);
    void updateFrameTexture();

    friend class BinkVideoDecoder;
};

} // namespace video

} // namespace reone
