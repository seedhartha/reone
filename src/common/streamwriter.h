/*
 * Copyright (c) 2020 The reone project contributors
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

#include <cstdint>
#include <memory>
#include <ostream>
#include <string>

#include "types.h"
#include "../common/endianutil.h"

namespace reone {

class StreamWriter {
public:
    StreamWriter(const std::shared_ptr<std::ostream> &stream, Endianess endianess = Endianess::Little);

    void putByte(uint8_t val);
    void putInt64(int64_t val);
    void putCString(const std::string &str);

    void write(char obj); // alias for putByte
    void write(const std::string &obj);

    /* only permits fundamental integral types or extended integral types */
    template <class T>
    void write(T obj) {
        swapBytesIfNotSystemEndianess(obj, _endianess);
        _stream->write(reinterpret_cast<char*>(&obj), sizeof(T));
    }

    template<class T>
    StreamWriter &operator<<(T obj) {
        write(obj);
        return *this;
    }

    const std::shared_ptr<std::ostream> &getStream();

private:
    std::shared_ptr<std::ostream> _stream;
    Endianess _endianess;

    StreamWriter(const StreamWriter &) = delete;
    StreamWriter &operator=(const StreamWriter &) = delete;
};

} // namespace reone
