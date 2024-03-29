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

    virtual void init() = 0;

    virtual uint32_t millis() const = 0;
    virtual uint64_t micros() const = 0;
};

class Clock : public IClock, boost::noncopyable {
public:
    void init() override;

    uint32_t millis() const override;
    uint64_t micros() const override;

private:
    uint64_t _freqOver1e3 {0};
    uint64_t _freqOver1e6 {0};
};

} // namespace reone
