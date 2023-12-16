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

#include "tablepanel.h"

#include <wx/dataview.h>

#include "../../viewmodel/resource/table.h"

namespace reone {

TableResourcePanel::TableResourcePanel(TableResourceViewModel &viewModel,
                                       wxWindow *parent) :
    wxPanel(parent),
    _viewModel(viewModel) {

    auto tableCtrl = new wxDataViewListCtrl(this, wxID_ANY);
    tableCtrl->Freeze();
    for (auto &column : viewModel.content().columns) {
        tableCtrl->AppendTextColumn(column);
    }
    for (auto &row : viewModel.content().rows) {
        auto values = wxVector<wxVariant>();
        for (auto &value : row) {
            values.push_back(wxVariant(value));
        }
        tableCtrl->AppendItem(values);
    }
    tableCtrl->Thaw();

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(tableCtrl, 1, wxEXPAND);
    SetSizer(sizer);
}

} // namespace reone
