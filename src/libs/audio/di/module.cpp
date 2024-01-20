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

#include "reone/audio/di/module.h"

namespace reone {

namespace audio {

void AudioModule::init() {
    _context = std::make_unique<Context>();
    _mixer = std::make_unique<AudioMixer>(_options);

    _context->init();

    _services = std::make_unique<AudioServices>(*_context, *_mixer);
}

void AudioModule::deinit() {
    _services.reset();

    _mixer.reset();
    _context.reset();
}

} // namespace audio

} // namespace reone
