#pragma once

namespace reone {

namespace audio {

enum class AudioFormat {
    Mono8,
    Mono16,
    Stereo8,
    Stereo16
};

struct AudioOptions {
    int volume { 0 };
};

} // namespace audio

} // namespace reone
