#pragma once

#include <cstdint>

#include "../core/types.h"

#include "types.h"

namespace reone {

namespace audio {

class AudioStream {
public:
    struct Frame {
        AudioFormat format { AudioFormat::Mono8 };
        int sampleRate { 0 };
        ByteArray samples;
    };

    AudioStream() = default;

    void add(Frame &&frame);
    void fill(int frameIdx, uint32_t buffer);

    int frameCount() const;
    const Frame &getFrame(int index) const;

private:
    std::vector<Frame> _frames;

    AudioStream(const AudioStream &) = delete;
    AudioStream &operator=(const AudioStream &) = delete;

    int getALAudioFormat(AudioFormat format) const;
};

} // namespace audio

} // namespace reone
