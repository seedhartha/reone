#pragma once

#include <memory>

#include "mad.h"

#include "../audio/stream.h"

namespace reone {

namespace resources {

class Mp3File {
public:
    Mp3File() = default;

    void load(ByteArray &&data);
    std::shared_ptr<audio::AudioStream> stream() const;

private:
    ByteArray _input;
    std::shared_ptr<audio::AudioStream> _stream;
    bool _done { false };

    Mp3File(const Mp3File &) = delete;
    Mp3File &operator=(const Mp3File &) = delete;

    static mad_flow inputFunc(void *playbuf, mad_stream *stream);
    static mad_flow headerFunc(void *playbuf, mad_header const *header);
    static mad_flow outputFunc(void *playbuf, mad_header const *header, mad_pcm *pcm);
};

} // namespace resources

} // namespace reone
