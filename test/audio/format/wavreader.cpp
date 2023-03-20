/*
 * Copyright (c) 2020-2022 The reone project contributors
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

#include <boost/test/unit_test.hpp>

#include "../../../src/audio/format/mp3reader.h"
#include "../../../src/audio/format/wavreader.h"
#include "../../../src/audio/stream.h"
#include "../../../src/common/stream/bytearrayinput.h"
#include "../../../src/common/stringbuilder.h"

#include "../../fixtures/audio.h"

using namespace std;

using namespace reone;
using namespace reone::audio;

BOOST_AUTO_TEST_SUITE(wav_reader)

BOOST_AUTO_TEST_CASE(should_load_plain_wav) {
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
                        .build();
    auto wav = ByteArrayInputStream(wavBytes);
    auto mp3ReaderFactory = MockMp3ReaderFactory();
    auto reader = WavReader(mp3ReaderFactory);

    // when
    reader.load(wav);

    // then
    auto stream = reader.stream();
    BOOST_TEST(static_cast<bool>(stream));
    BOOST_TEST(1 == stream->getFrameCount());
    auto frame = stream->getFrame(0);
    BOOST_TEST(static_cast<int>(AudioFormat::Mono8) == static_cast<int>(frame.format));
    BOOST_TEST(22050 == frame.sampleRate);
    auto samples = reinterpret_cast<int16_t *>(frame.samples.data());
    BOOST_TEST(32767 == samples[0]);
}

BOOST_AUTO_TEST_CASE(should_load_obfuscated_wav) {
    // given
    auto wavBytes = StringBuilder()
                        // Header
                        .append("\xff\xf3\x60\xc4", 4) // fake signature
                        .repeat('\x00', 466)           // padding
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
                        .build();
    auto wav = ByteArrayInputStream(wavBytes);
    auto mp3ReaderFactory = MockMp3ReaderFactory();
    auto reader = WavReader(mp3ReaderFactory);

    // when
    reader.load(wav);

    // then
    auto stream = reader.stream();
    BOOST_TEST(static_cast<bool>(stream));
    BOOST_TEST(1 == stream->getFrameCount());
    auto frame = stream->getFrame(0);
    BOOST_TEST(static_cast<int>(AudioFormat::Mono16) == static_cast<int>(frame.format));
    BOOST_TEST(22050 == frame.sampleRate);
    BOOST_TEST(16ll == frame.samples.size());
    auto samples = reinterpret_cast<uint16_t *>(frame.samples.data());
    BOOST_TEST(6 == samples[0]);
    BOOST_TEST(9 == samples[1]);
    BOOST_TEST(18 == samples[2]);
    BOOST_TEST(26 == samples[3]);
    BOOST_TEST(40 == samples[4]);
    BOOST_TEST(62 == samples[5]);
    BOOST_TEST(60 == samples[6]);
    BOOST_TEST(99 == samples[7]);
}

BOOST_AUTO_TEST_CASE(should_load_obfuscated_mp3) {
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
                        .build();
    auto wav = ByteArrayInputStream(wavBytes);
    auto mp3Reader = make_shared<MockMp3Reader>();
    auto mp3ReaderFactory = MockMp3ReaderFactory(mp3Reader);
    auto reader = WavReader(mp3ReaderFactory);

    // when
    reader.load(wav);

    // then
    BOOST_TEST(1ll == mp3Reader->loadInvocations().size());
}

BOOST_AUTO_TEST_SUITE_END()
