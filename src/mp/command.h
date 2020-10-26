/*
 * Copyright © 2020 Vsevolod Kremianskii
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

#include "../net/command.h"

namespace reone {

namespace mp {

enum class CommandType {
    None
};

class Command : public net::Command {
public:
    Command() = default;
    Command(uint32_t id, CommandType type);

    void load(const ByteArray &data);

    ByteArray getBytes() const override;

    CommandType type() const;

private:
    CommandType _type { CommandType::None };
};

} // namespace mp

} // namespace reone
