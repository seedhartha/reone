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

#include "reone/audio/source.h"
#include "reone/graphics/texture.h"
#include "reone/system/types.h"

#include "videostream.h"

namespace reone {

namespace graphics {

struct GraphicsServices;

}

namespace audio {

class IAudioPlayer;

}

namespace movie {

class BikReader;

class IMovie {
public:
    virtual ~IMovie() = default;

    virtual void update(float dt) = 0;
    virtual void render() = 0;

    virtual void finish() = 0;

    virtual bool isFinished() const = 0;
};

class Movie : public IMovie, boost::noncopyable {
public:
    Movie(
        graphics::GraphicsServices &graphicsSvc,
        audio::IAudioPlayer &audioPlayer) :
        _graphicsSvc(graphicsSvc),
        _audioPlayer(audioPlayer) {
    }

    void init();
    void deinit();

    void update(float dt) override;
    void render() override;

    void finish() override { _finished = true; }

    bool isFinished() const override { return _finished; }

    void setVideoStream(std::shared_ptr<VideoStream> stream) { _videoStream = std::move(stream); }
    void setAudioBuffer(std::shared_ptr<audio::AudioBuffer> stream) { _audioStream = std::move(stream); }

private:
    graphics::GraphicsServices &_graphicsSvc;
    audio::IAudioPlayer &_audioPlayer;

    bool _inited {false};

    int _width {0};
    int _height {0};

    float _time {0.0f};
    bool _finished {false};

    std::shared_ptr<VideoStream> _videoStream;
    std::shared_ptr<audio::AudioBuffer> _audioStream;

    std::shared_ptr<graphics::Texture> _texture;
    std::shared_ptr<audio::AudioSource> _audioSource;
};

} // namespace movie

} // namespace reone
