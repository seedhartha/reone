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

namespace reone {

class IClock {
public:
    virtual ~IClock() = default;

    virtual uint32_t ticks() const = 0;

    virtual uint64_t performanceFrequency() const = 0;
    virtual uint64_t performanceCounter() const = 0;
};

class Clock : public IClock, boost::noncopyable {
public:
    uint32_t ticks() const override;

    uint64_t performanceFrequency() const override;
    uint64_t performanceCounter() const override;
};

} // namespace reone
