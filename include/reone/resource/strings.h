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

#include "format/tlkreader.h"

#include "types.h"

namespace reone {

namespace resource {

class IStrings {
public:
    virtual ~IStrings() = default;

    virtual std::string getText(int strRef) = 0;
    virtual std::string getSound(int strRef) = 0;
};

class Strings : public IStrings {
public:
    Strings() = default;

    void init(const std::filesystem::path &gameDir);

    std::string getText(int strRef) override;
    std::string getSound(int strRef) override;

    void setTalkTable(std::shared_ptr<TalkTable> table) {
        _table = std::move(table);
    }

private:
    std::shared_ptr<TalkTable> _table;

    void process(std::string &str);
    void stripDeveloperNotes(std::string &str);
};

} // namespace resource

} // namespace reone
