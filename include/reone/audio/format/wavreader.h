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

#include "reone/system/binaryreader.h"
#include "reone/system/stream/input.h"

#include "../types.h"

namespace reone {

namespace audio {

class AudioClip;

enum class WavAudioFormat {
    PCM = 1,
    IMAADPCM = 0x11
};

class IMp3ReaderFactory;

class WavReader : public boost::noncopyable {
public:
    WavReader(IInputStream &wav, IMp3ReaderFactory &mp3ReaderFactory) :
        _wav(BinaryReader(wav)),
        _mp3ReaderFactory(mp3ReaderFactory) {
    }

    void load();

    std::shared_ptr<AudioClip> stream() const { return _stream; }

private:
    struct ChunkHeader {
        std::string id;
        uint32_t size {0};
    };

    struct IMA {
        int16_t lastSample {0};
        int16_t stepIndex {0};
    };

    BinaryReader _wav;
    IMp3ReaderFactory &_mp3ReaderFactory;

    size_t _wavLength {0};

    WavAudioFormat _audioFormat {WavAudioFormat::PCM};
    uint16_t _channelCount {0};
    uint32_t _sampleRate {0};
    uint16_t _blockAlign {0};
    uint16_t _bitsPerSample {0};
    IMA _ima[2];

    std::shared_ptr<AudioClip> _stream;

    int16_t getIMASample(int channel, uint8_t nibble);
    void getIMASamples(int channel, uint8_t nibbles, int16_t &sample1, int16_t &sample2);
    void loadData(ChunkHeader chunk);
    void loadFormat(ChunkHeader chunk);
    void loadIMAADPCM(uint32_t chunkSize);
    void loadPCM(uint32_t chunkSize);
    bool readChunkHeader(ChunkHeader &chunk);

    AudioFormat getAudioFormat() const;
};

} // namespace audio

} // namespace reone
