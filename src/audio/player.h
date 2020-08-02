#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <thread>

#include "AL/alc.h"

#include "soundinstance.h"

namespace reone {

namespace audio {

class AudioPlayer {
public:
    static AudioPlayer &instance();

    void init(const AudioOptions &opts);
    void deinit();
    void play(const std::shared_ptr<AudioStream> &stream, bool loop = false);
    void reset();

private:
    AudioOptions _opts;
    ALCdevice *_device { nullptr };
    ALCcontext *_context { nullptr };
    std::thread _thread;
    std::atomic_bool _run { true };
    std::list<SoundInstance> _sounds;
    std::recursive_mutex _soundsMutex;

    AudioPlayer() = default;
    AudioPlayer(const AudioPlayer &) = delete;
    ~AudioPlayer();

    AudioPlayer &operator=(const AudioPlayer &) = delete;

    static void threadStart(AudioPlayer *);
};

} // namespace audio

} // namespace reone
