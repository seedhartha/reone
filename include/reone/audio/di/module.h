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

#include "reone/resource/di/module.h"

#include "../context.h"
#include "../options.h"

#include "services.h"

namespace reone {

namespace audio {

class AudioModule : boost::noncopyable {
public:
    ~AudioModule() { deinit(); }

    void init();
    void deinit();

    AudioContext &context() { return *_context; }

    AudioServices &services() { return *_services; }

private:
    std::unique_ptr<AudioContext> _context;

    std::unique_ptr<AudioServices> _services;
};

} // namespace audio

} // namespace reone
