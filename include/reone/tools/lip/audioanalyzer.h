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

#include "reone/audio/clip.h"
#include "reone/system/timespan.h"

namespace reone {

class AudioAnalyzer : boost::noncopyable {
public:
    std::vector<TimeSpan> silentSpans(const audio::AudioClip &clip,
                                      float minSilenceDuration = 0.05f,
                                      float maxSilenceAmplitude = 0.01f);

    std::vector<float> waveform(const audio::AudioClip &clip, int resolution);

private:
    std::vector<TimeSpan> computeFrameSpans(const audio::AudioClip &clip);
    float sampleNormalized(const audio::AudioClip::Frame &frame, float time);
};

} // namespace reone
