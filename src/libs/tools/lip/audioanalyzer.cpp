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

#include "reone/tools/lip/audioanalyzer.h"

#include "reone/system/logutil.h"

using namespace reone::audio;

namespace reone {

std::vector<TimeSpan> AudioAnalyzer::silentSpans(const AudioClip &clip,
                                                 float minSilenceDuration,
                                                 float maxSilenceAmplitude) {
    std::vector<TimeSpan> silentSpans;
    auto frameSpans = computeFrameSpans(clip);
    float silenceStart = 0.0f;
    bool silentSpan = false;
    for (float t = 0.0f; t < clip.duration(); t += 0.001f) {
        for (int i = 0; i < clip.getFrameCount(); ++i) {
            if (!frameSpans[i].contains(t)) {
                continue;
            }
            float sampleTime = t - frameSpans[i].startInclusive;
            float sample = sampleNormalized(clip.getFrame(i), sampleTime);
            bool silent = std::fabs(sample) <= maxSilenceAmplitude;
            if (silent && !silentSpan) {
                silenceStart = t;
                silentSpan = true;
            } else if (!silent && silentSpan) {
                if (t - silenceStart >= minSilenceDuration) {
                    silentSpans.push_back(TimeSpan {silenceStart, t});
                }
                silentSpan = false;
            }
            break;
        }
    }
    if (silentSpan && clip.duration() - silenceStart >= minSilenceDuration) {
        silentSpans.push_back(TimeSpan {silenceStart, clip.duration()});
    }
    return silentSpans;
}

std::vector<float> AudioAnalyzer::waveform(const AudioClip &clip, int resolution) {
    auto frameSpans = computeFrameSpans(clip);
    std::vector<float> waveform;
    for (int x = 0; x < resolution; ++x) {
        float waveformTime = (x / static_cast<float>(resolution)) * clip.duration();
        for (int i = 0; i < clip.getFrameCount(); ++i) {
            if (!frameSpans[i].contains(waveformTime)) {
                continue;
            }
            float sampleTime = waveformTime - frameSpans[i].startInclusive;
            waveform.push_back(sampleNormalized(clip.getFrame(i), sampleTime));
            break;
        }
    }
    return waveform;
}

std::vector<TimeSpan> AudioAnalyzer::computeFrameSpans(const AudioClip &clip) {
    std::vector<TimeSpan> spans;
    float time = 0.0f;
    for (int i = 0; i < clip.getFrameCount(); ++i) {
        const auto &frame = clip.getFrame(i);
        float duration = frame.samples.size() / frame.stride() / static_cast<float>(frame.sampleRate);
        spans.push_back(TimeSpan {time, time + duration});
        time += duration;
    }
    return spans;
}

float AudioAnalyzer::sampleNormalized(const AudioClip::Frame &frame, float time) {
    int sampleIdx = static_cast<int>(time * frame.sampleRate);
    if (sampleIdx >= frame.samples.size() / frame.stride()) {
        sampleIdx = frame.samples.size() / frame.stride() - 1;
    }
    switch (frame.format) {
    case AudioFormat::Mono8:
        return reinterpret_cast<const uint8_t *>(&frame.samples[0])[sampleIdx] / 255.0f * 2.0f - 1.0f;
    case AudioFormat::Mono16:
        return reinterpret_cast<const int16_t *>(&frame.samples[0])[sampleIdx] / 65535.0f * 2.0f;
    case AudioFormat::Stereo8:
        return reinterpret_cast<const uint8_t *>(&frame.samples[0])[2 * sampleIdx] / 255.0f * 2.0f - 1.0f;
    case AudioFormat::Stereo16:
        return reinterpret_cast<const int16_t *>(&frame.samples[0])[2 * sampleIdx] / 65535.0f * 2.0f;
    default:
        throw std::logic_error("Unsupported audio format: " + std::to_string(static_cast<int>(frame.format)));
    }
}

} // namespace reone
