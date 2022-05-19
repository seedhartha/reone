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

#pragma once

#include "../types.h"

namespace reone {

class IInputStream : boost::noncopyable {
public:
    virtual void seek(int64_t offset, SeekOrigin origin) = 0;
    virtual int readByte() = 0;
    virtual int read(char *outData, int length) = 0;

    virtual size_t position() = 0;
    virtual bool eof() = 0;
};

} // namespace reone
