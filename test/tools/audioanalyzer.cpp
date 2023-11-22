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

#include "reone/system/binarywriter.h"
#include "reone/tools/audioanalyzer.h"

using namespace reone;
using namespace reone::audio;

TEST(audio_analyzer, should_return_silent_spans_given_mono8_audio_with_sound_and_silence_at_end) {
    // given
    unsigned char samples[] = {
        128, 160, 192, 224, 255, 224, 192, 160, //
        128, 96, 64, 32, 0, 32, 64, 96,         //
        128, 128, 128, 128, 128, 128, 128, 128  //
    };
    ByteBuffer samplesBuffer(sizeof(samples));
    std::memcpy(&samplesBuffer[0], samples, sizeof(samples));
    AudioBuffer buffer;
    buffer.add(AudioBuffer::Frame {AudioFormat::Mono8, 24, samplesBuffer});
    AudioAnalyzer analyzer;

    // when
    auto spans = analyzer.silentSpans(buffer, 0.05f);

    // then
    EXPECT_EQ(spans.size(), 1ll);
    EXPECT_NEAR(spans[0].startInclusive, 0.66f, 0.01f);
    EXPECT_NEAR(spans[0].endExclusive, 1.0f, 0.01f);
}

TEST(audio_analyzer, should_return_waveform_given_mono8_audio_with_sound_and_silence_at_end) {
    // given
    unsigned char samples[] = {
        128, 160, 192, 224, 255, 224, 192, 160, //
        128, 96, 64, 32, 0, 32, 64, 96,         //
        128, 128, 128, 128, 128, 128, 128, 128  //
    };
    ByteBuffer samplesBuffer(sizeof(samples));
    std::memcpy(&samplesBuffer[0], samples, sizeof(samples));
    AudioBuffer buffer;
    buffer.add(AudioBuffer::Frame {AudioFormat::Mono8, 24, samplesBuffer});
    AudioAnalyzer analyzer;

    // when
    auto waveform = analyzer.waveform(buffer, 12);

    // then
    EXPECT_EQ(waveform.size(), 12ll);
    EXPECT_NEAR(waveform[0], 0.0f, 0.01f);
    EXPECT_NEAR(waveform[1], 0.5f, 0.01f);
    EXPECT_NEAR(waveform[2], 1.0f, 0.01f);
    EXPECT_NEAR(waveform[3], 0.5f, 0.01f);
    EXPECT_NEAR(waveform[4], 0.0f, 0.01f);
    EXPECT_NEAR(waveform[5], -0.5f, 0.01f);
    EXPECT_NEAR(waveform[6], -1.0f, 0.01f);
    EXPECT_NEAR(waveform[7], -0.5f, 0.01f);
    EXPECT_NEAR(waveform[8], 0.0f, 0.01f);
    EXPECT_NEAR(waveform[9], 0.0f, 0.01f);
    EXPECT_NEAR(waveform[10], 0.0f, 0.01f);
    EXPECT_NEAR(waveform[11], 0.0f, 0.01f);
}
