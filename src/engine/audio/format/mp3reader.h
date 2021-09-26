/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "mad.h"

#include "../../common/types.h"

namespace reone {

namespace audio {

class AudioStream;

class Mp3Reader : boost::noncopyable {
public:
    void load(const std::shared_ptr<std::istream> &stream);
    void load(ByteArray &&data);

    std::shared_ptr<AudioStream> stream() const { return _stream; }

private:
    ByteArray _input;
    std::shared_ptr<AudioStream> _stream;
    bool _done {false};

    static mad_flow inputFunc(void *playbuf, mad_stream *stream);
    static mad_flow headerFunc(void *playbuf, mad_header const *header);
    static mad_flow outputFunc(void *playbuf, mad_header const *header, mad_pcm *pcm);
};

} // namespace audio

} // namespace reone
