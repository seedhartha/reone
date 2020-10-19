/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "wavfile.h"

#include "glm/glm.hpp"

#include "../../streamutil.h"

#include "mp3file.h"

using namespace std;

using namespace reone::audio;

namespace reone {

namespace resource {

WavFile::WavFile() : BinaryFile(0) {
}

void WavFile::doLoad() {
    string sign(readFixedString(4));
    if (sign == "\xff\xf3\x60\xc4") {
        seek(0x1da);
    } else if (sign != "RIFF") {
        throw runtime_error("WAV: invalid file signature: " + sign);
    }

    uint32_t chunkSize = readUint32();
    string format(readFixedString(4));
    if (format != "WAVE") {
        throw runtime_error("WAV: invalid chunk format: " + format);
    }
    ChunkHeader chunk;
    while (readChunkHeader(chunk)) {
        if (chunk.id == "fmt ") {
            loadFormat(chunk);
        } else if (chunk.id == "data") {
            loadData(chunk);
            break;
        } else {
            ignore(chunk.size);
        }
    }
}

bool WavFile::readChunkHeader(ChunkHeader &chunk) {
    if (_in->eof()) return false;

    string id(readFixedString(4));
    uint32_t size = readUint32();

    chunk.id = move(id);
    chunk.size = size;

    return true;
}

void WavFile::loadFormat(ChunkHeader chunk) {
    _audioFormat = static_cast<WavAudioFormat>(readUint16());
    if (_audioFormat != WavAudioFormat::PCM && _audioFormat != WavAudioFormat::IMAADPCM) {
        throw runtime_error("WAV: unsupported audio format: " + to_string(static_cast<int>(_audioFormat)));
    }
    _channelCount = readUint16();
    if (_channelCount != 1 && _channelCount != 2) {
        throw runtime_error("WAV: invalid number of channels: " + to_string(_channelCount));
    }
    _sampleRate = readUint32();

    uint32_t byteRate = readUint32();

    _blockAlign = readUint16();
    _bitsPerSample = readUint16();

    if (_bitsPerSample != 4 && _bitsPerSample != 8 && _bitsPerSample != 16) {
        throw runtime_error("WAV: invalid bits per sample: " + to_string(_bitsPerSample));
    }

    ignore(chunk.size - 16);
}

void WavFile::loadData(ChunkHeader chunk) {
    if (chunk.size == 0) {
        uint32_t pos = tell();
        ByteArray data(_size - pos);
        _in->read(&data[0], data.size());

        Mp3File mp3;
        mp3.load(move(data));
        _stream = mp3.stream();

        return;
    }

    switch (_audioFormat) {
        case WavAudioFormat::PCM:
            loadPCM(chunk.size);
            break;
        case WavAudioFormat::IMAADPCM:
            loadIMAADPCM(chunk.size);
            break;
    }
}

void WavFile::loadPCM(uint32_t chunkSize) {
    AudioStream::Frame frame;
    frame.format = getAudioFormat();
    frame.sampleRate = _sampleRate;
    frame.samples.resize(chunkSize);
    _in->read(&frame.samples[0], chunkSize);

    _stream = make_shared<AudioStream>();
    _stream->add(move(frame));
}

static const int kIMAIndexTable[] = { -1, -1, -1, -1, 2, 4, 6, 8 };

static const int kIMAStepTable[] = {
    7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
    19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
    50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
    130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
    876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
    2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
    5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

void WavFile::loadIMAADPCM(uint32_t chunkSize) {
    ByteArray chunk(chunkSize);
    _in->read(&chunk[0], chunkSize);

    AudioStream::Frame frame;
    frame.format = getAudioFormat();
    frame.sampleRate = _sampleRate;
    frame.samples.reserve(2 * (chunkSize - 4 * _channelCount * chunkSize / _blockAlign));

    uint32_t off = 0;
    while (off < chunkSize) {
        if (off % _blockAlign == 0) {
            for (int i = 0; i < _channelCount; ++i) {
                _ima[i].lastSample = *reinterpret_cast<int16_t *>(&chunk[off + 0]);
                _ima[i].stepIndex = *reinterpret_cast<int16_t *>(&chunk[off + 2]);
                off += 4;
            }
        }
        int16_t samples[16];
        for (int i = 0; i < _channelCount; ++i) {
            for (int j = 0; j < 4; ++j) {
                int idx = 8 * i + 2 * j;
                getIMASamples(i, chunk[off++], samples[idx + 0], samples[idx + 1]);
            }
        }
        if (_channelCount == 2) {
            for (int i = 0; i < 8; ++i) {
                frame.samples.push_back((samples[i + 0] >> 0) & 0xff);
                frame.samples.push_back((samples[i + 0] >> 8) & 0xff);
                frame.samples.push_back((samples[i + 8] >> 0) & 0xff);
                frame.samples.push_back((samples[i + 8] >> 8) & 0xff);
            }
        } else {
            for (int i = 0; i < 8; ++i) {
                frame.samples.push_back((samples[i] >> 0) & 0xff);
                frame.samples.push_back((samples[i] >> 8) & 0xff);
            }
        }
    }

    _stream = make_shared<AudioStream>();
    _stream->add(move(frame));
}

AudioFormat WavFile::getAudioFormat() const {
    switch (_audioFormat) {
        case WavAudioFormat::PCM:
            switch (_bitsPerSample) {
                case 16:
                    return _channelCount == 2 ? AudioFormat::Stereo16 : AudioFormat::Mono16;
                case 8:
                    return _channelCount == 2 ? AudioFormat::Stereo8 : AudioFormat::Mono8;
                default:
                    throw logic_error("WAV: PCM: invalid bits per sample: " + to_string(_bitsPerSample));
            }
        case WavAudioFormat::IMAADPCM:
            if (_bitsPerSample != 4) {
                throw logic_error("WAV: IMA ADPCM: invalid bits per sample: " + to_string(_bitsPerSample));
            }
            return _channelCount == 2 ? AudioFormat::Stereo16 : AudioFormat::Mono16;
        default:
            throw logic_error("WAV: invalid audio format: " + to_string(static_cast<int>(_audioFormat)));

    }
}

void WavFile::getIMASamples(int channel, uint8_t nibbles, int16_t &sample1, int16_t &sample2) {
    uint8_t n1 = (nibbles >> 0) & 0xf;
    uint8_t n2 = (nibbles >> 4) & 0xf;

    sample1 = getIMASample(channel, n1);
    sample2 = getIMASample(channel, n2);
}

int16_t WavFile::getIMASample(int channel, uint8_t nibble) {
    int step = (2 * (nibble & 0x7) + 1) * kIMAStepTable[_ima[channel].stepIndex] / 8;
    int diff = nibble & 0x8 ? -step : step;
    int sample = glm::clamp(_ima[channel].lastSample + diff, -32768, 32767);

    _ima[channel].lastSample = sample;
    _ima[channel].stepIndex = glm::clamp(_ima[channel].stepIndex + kIMAIndexTable[nibble & 0x7], 0, 88);

    return sample;
}

shared_ptr<audio::AudioStream> WavFile::stream() const {
    return _stream;
}

} // namespace resources

} // namespace reone
