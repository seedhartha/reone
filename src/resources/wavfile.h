#pragma once

#include "../audio/stream.h"

#include "binfile.h"

namespace reone {

namespace resources {

enum class WavAudioFormat {
    PCM = 1,
    IMAADPCM = 0x11
};

class WavFile : public BinaryFile {
public:
    WavFile();
    std::shared_ptr<audio::AudioStream> stream() const;

private:
    struct ChunkHeader {
        std::string id;
        uint32_t size { 0 };
    };

    struct IMA {
        int16_t lastSample { 0 };
        int16_t stepIndex { 0 };
    };

    WavAudioFormat _audioFormat { WavAudioFormat::PCM };
    uint16_t _channelCount { 0 };
    uint32_t _sampleRate { 0 };
    uint16_t _blockAlign { 0 };
    uint16_t _bitsPerSample { 0 };
    IMA _ima[2];
    std::shared_ptr<audio::AudioStream> _stream;

    void doLoad() override;
    bool readChunkHeader(ChunkHeader &chunk);
    void loadFormat(ChunkHeader chunk);
    void loadData(ChunkHeader chunk);
    void loadPCM(uint32_t chunkSize);
    void loadIMAADPCM(uint32_t chunkSize);
    void getIMASamples(int channel, uint8_t nibbles, int16_t &sample1, int16_t &sample2);
    int16_t getIMASample(int channel, uint8_t nibble);
    audio::AudioFormat getAudioFormat() const;
};

} // namespace resources

} // namespace reone
