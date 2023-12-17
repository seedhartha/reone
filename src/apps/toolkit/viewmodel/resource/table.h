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

#include "reone/resource/types.h"

#include "../../property.h"
#include "../../viewmodel.h"

namespace reone {

struct TableContent {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
    bool rowNumberColumn;

    TableContent(std::vector<std::string> columns,
                 std::vector<std::vector<std::string>> rows,
                 bool rowNumberColumn = false) :
        columns(columns),
        rows(rows),
        rowNumberColumn(rowNumberColumn) {
    }
};

class TableResourceViewModel : public ViewModel {
public:
    TableResourceViewModel(resource::ResType resType,
                           std::shared_ptr<TableContent> content) :
        _resType(resType),
        _content(std::move(content)) {
    }

    Property<bool> &modified() {
        return _modified;
    }

    resource::ResType resType() const {
        return _resType;
    }

    TableContent &content() const {
        return *_content;
    }

private:
    resource::ResType _resType;
    std::shared_ptr<TableContent> _content;

    Property<bool> _modified;
};

} // namespace reone
