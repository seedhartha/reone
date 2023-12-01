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

#include <gtest/gtest.h>

#include "reone/audio/buffer.h"
#include "reone/resource/format/mp3reader.h"
#include "reone/resource/format/wavreader.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"

#include "../../fixtures/resource.h"

using namespace reone;
using namespace reone::audio;
using namespace reone::resource;

using testing::_;
using testing::Return;

TEST(wav_reader, should_load_plain_wav) {
    // given
    auto wavBytes = StringBuilder()
                        // Header
                        .append("RIFF")                // signature
                        .append("\x00\x00\x00\x00", 4) // chunk size
                        .append("WAVE")                // format
                        // Fmt Chunk
                        .append("fmt ")                // chunk id
                        .append("\x10\x00\x00\x00", 4) // chunk size
                        .append("\x01\x00", 2)         // audio format
                        .append("\x01\x00", 2)         // number of channels
                        .append("\x22\x56\x00\x00", 4) // sample rate
                        .append("\x00\x00\x00\x00", 4) // byte rate
                        .append("\x00\x00", 2)         // block align
                        .append("\x08\x00", 2)         // bits per sample
                        // Data Chunk
                        .append("data")                // chunk id
                        .append("\x02\x00\x00\x00", 4) // chunk size
                        // Samples
                        .append("\xff\x7f", 2)
                        .string();
    auto wav = MemoryInputStream(wavBytes);
    auto mp3ReaderFactory = MockMp3ReaderFactory();
    auto reader = WavReader(wav, mp3ReaderFactory);

    // when
    reader.load();

    // then
    auto stream = reader.stream();
    EXPECT_TRUE(static_cast<bool>(stream));
    EXPECT_EQ(1, stream->getFrameCount());
    auto &frame = stream->getFrame(0);
    EXPECT_EQ(static_cast<int>(AudioFormat::Mono8), static_cast<int>(frame.format));
    EXPECT_EQ(22050, frame.sampleRate);
    auto samples = reinterpret_cast<const int16_t *>(frame.samples.data());
    EXPECT_EQ(32767, samples[0]);
}

TEST(wav_reader, should_load_obfuscated_wav) {
    // given
    auto wavBytes = StringBuilder()
                        // Header
                        .append("\xff\xf3\x60\xc4", 4) // fake signature
                        .append('\x00', 466)           // padding
                        .append("RIFF")                // real signature
                        .append("\x00\x00\x00\x00", 4) // chunk size
                        .append("WAVE")                // format
                        // Fmt Chunk
                        .append("fmt ")                // chunk id
                        .append("\x10\x00\x00\x00", 4) // chunk size
                        .append("\x11\x00", 2)         // audio format
                        .append("\x01\x00", 2)         // number of channels
                        .append("\x22\x56\x00\x00", 4) // sample rate
                        .append("\x00\x00\x00\x00", 4) // byte rate
                        .append("\x08\x00", 2)         // block align
                        .append("\x04\x00", 2)         // bits per sample
                        // Data Chunk
                        .append("data")                // chunk id
                        .append("\x08\x00\x00\x00", 4) // chunk size
                        // IMA Blocks
                        .append("\x00\x00\x03\x00\x12\x34\x56\x78", 8)
                        .string();
    auto wav = MemoryInputStream(wavBytes);
    auto mp3ReaderFactory = MockMp3ReaderFactory();
    auto reader = WavReader(wav, mp3ReaderFactory);

    // when
    reader.load();

    // then
    auto stream = reader.stream();
    EXPECT_TRUE(static_cast<bool>(stream));
    EXPECT_EQ(1, stream->getFrameCount());
    auto &frame = stream->getFrame(0);
    EXPECT_EQ(static_cast<int>(AudioFormat::Mono16), static_cast<int>(frame.format));
    EXPECT_EQ(22050, frame.sampleRate);
    EXPECT_EQ(16ll, frame.samples.size());
    auto samples = reinterpret_cast<const uint16_t *>(frame.samples.data());
    EXPECT_EQ(6, samples[0]);
    EXPECT_EQ(9, samples[1]);
    EXPECT_EQ(18, samples[2]);
    EXPECT_EQ(26, samples[3]);
    EXPECT_EQ(40, samples[4]);
    EXPECT_EQ(62, samples[5]);
    EXPECT_EQ(60, samples[6]);
    EXPECT_EQ(99, samples[7]);
}

TEST(wav_reader, should_load_obfuscated_mp3) {
    // given
    auto wavBytes = StringBuilder()
                        // Header
                        .append("RIFF")                // signature
                        .append("\x00\x00\x00\x00", 4) // chunk size
                        .append("WAVE")                // format
                        // Fmt Chunk
                        .append("fmt ")                // chunk id
                        .append("\x10\x00\x00\x00", 4) // chunk size
                        .append("\x01\x00", 2)         // audio format
                        .append("\x01\x00", 2)         // number of channels
                        .append("\x22\x56\x00\x00", 4) // sample rate
                        .append("\x00\x00\x00\x00", 4) // byte rate
                        .append("\x00\x00", 2)         // block align
                        .append("\x08\x00", 2)         // bits per sample
                        // Data Chunk
                        .append("data")                // chunk id
                        .append("\x00\x00\x00\x00", 4) // chunk size
                        // MP3
                        .append("\x00", 1)
                        .string();
    auto wav = MemoryInputStream(wavBytes);

    auto mp3Reader = std::make_shared<MockMp3Reader>();
    EXPECT_CALL(*mp3Reader, load(_)).Times(1);

    auto mp3ReaderFactory = MockMp3ReaderFactory();
    EXPECT_CALL(mp3ReaderFactory, create())
        .WillOnce(Return(mp3Reader));

    auto reader = WavReader(wav, mp3ReaderFactory);

    // expect
    reader.load();
}
