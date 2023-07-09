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

#include "reone/audio/format/mp3reader.h"

#include "reone/system/stream/input.h"

#include "reone/audio/buffer.h"

namespace reone {

namespace audio {

static inline int scale(mad_fixed_t sample) {
    // round
    sample += (1L << (MAD_F_FRACBITS - 16));

    // clip
    if (sample >= MAD_F_ONE) {
        sample = MAD_F_ONE - 1;
    } else if (sample < -MAD_F_ONE) {
        sample = -MAD_F_ONE;
    }

    // quantize
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

void Mp3Reader::load(IInputStream &stream) {
    stream.seek(0, SeekOrigin::End);
    size_t size = stream.position();

    ByteArray data(size, '\0');
    stream.seek(0, SeekOrigin::Begin);
    stream.read(&data[0], size);

    _input = data;
    _stream = std::make_shared<AudioBuffer>();

    mad_decoder decoder;
    mad_decoder_init(
        &decoder,
        this,
        inputFunc,
        headerFunc,
        nullptr,
        outputFunc,
        nullptr,
        nullptr);

    mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
    mad_decoder_finish(&decoder);
}

mad_flow Mp3Reader::inputFunc(void *playbuf, mad_stream *stream) {
    Mp3Reader *mp3 = reinterpret_cast<Mp3Reader *>(playbuf);
    if (mp3->_done) {
        return MAD_FLOW_BREAK;
    }
    mad_stream_buffer(stream, reinterpret_cast<unsigned char *>(&mp3->_input[0]), static_cast<int>(mp3->_input.size()));
    mp3->_done = true;

    return MAD_FLOW_CONTINUE;
}

mad_flow Mp3Reader::headerFunc(void *playbuf, mad_header const *header) {
    return MAD_FLOW_CONTINUE;
}

mad_flow Mp3Reader::outputFunc(void *playbuf, mad_header const *header, mad_pcm *pcm) {
    Mp3Reader *mp3 = reinterpret_cast<Mp3Reader *>(playbuf);
    unsigned short sampleCount = pcm->length;
    mad_fixed_t *chLeft = pcm->samples[0];
    mad_fixed_t *chRight = pcm->samples[1];

    AudioBuffer::Frame frame;
    frame.format = pcm->channels == 2 ? AudioFormat::Stereo16 : AudioFormat::Mono16;
    frame.sampleRate = pcm->samplerate;
    frame.samples.reserve(static_cast<uint64_t>(pcm->channels) * sampleCount * sizeof(int16_t));

    while (sampleCount--) {
        int sample = scale(*chLeft++);
        frame.samples.push_back((sample >> 0) & 0xff);
        frame.samples.push_back((sample >> 8) & 0xff);

        if (pcm->channels == 2) {
            sample = scale(*chRight++);
            frame.samples.push_back((sample >> 0) & 0xff);
            frame.samples.push_back((sample >> 8) & 0xff);
        }
    }

    mp3->_stream->add(std::move(frame));

    return MAD_FLOW_CONTINUE;
}

} // namespace audio

} // namespace reone
