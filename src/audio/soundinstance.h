#pragma once

#include <memory>

#include "stream.h"

namespace reone {

namespace audio {

class SoundInstance {
public:
    SoundInstance(const std::shared_ptr<AudioStream> &stream, bool loop);
    SoundInstance(SoundInstance &&) = default;
    ~SoundInstance();

    SoundInstance &operator=(SoundInstance &&) = default;

    void update();
    bool stopped() const;

private:
    enum class State {
        NotInited,
        Playing,
        Stopped
    };

    std::shared_ptr<AudioStream> _stream;
    bool _loop { false };
    bool _multiframe { false };
    State _state { State::NotInited };
    int _nextFrame { 0 };
    int _nextBuffer { 0 };
    unsigned int _source { 0 };
    std::vector<unsigned int> _buffers;

    SoundInstance(SoundInstance &) = delete;
    SoundInstance &operator=(SoundInstance &) = delete;

    void init();
    void deinit();
};

} // namespace audio

} // namespace reone
