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

#include <wx/numdlg.h>

#include "../../viewmodel/resource/table.h"

using namespace reone::resource;

namespace reone {

TableResourcePanel::TableResourcePanel(TableResourceViewModel &viewModel,
                                       wxWindow *parent) :
    wxPanel(parent),
    _viewModel(viewModel) {

    _tableCtrl = new wxDataViewListCtrl(this, wxID_ANY);
    RefreshDataView();

    _tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [this](const auto &event) {
        auto item = event.GetItem();
        if (!item) {
            return;
        }
        auto control = wxDynamicCast(event.GetEventObject(), wxDataViewListCtrl);
        auto column = event.GetColumn();
        if (_viewModel.content().rowNumberColumn) {
            --column;
        }
        auto row = control->ItemToRow(item);
        auto newValue = event.GetValue().GetString();
        if (_viewModel.content().rows[row][column] != newValue) {
            _viewModel.content().rows[row][column] = newValue;
            _viewModel.modified() = true;
        }
    });
    _tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, [this](const auto &event) {
        enum class MenuItemId {
            AppendRow = 1,
            DeleteRow
        };
        auto column = event.GetColumn();
        if (_viewModel.content().rowNumberColumn && column == 0) {
            return;
        }
        auto item = event.GetItem();
        if (!item) {
            return;
        }
        auto control = wxDynamicCast(event.GetEventObject(), wxDataViewListCtrl);
        auto row = control->ItemToRow(item);
        wxMenu menu;
        menu.Append(static_cast<int>(MenuItemId::AppendRow), "Append row");
        menu.Append(static_cast<int>(MenuItemId::DeleteRow), "Delete row");
        menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [this, &row](const auto &event) {
            bool modified = false;
            switch (static_cast<MenuItemId>(event.GetId())) {
            case MenuItemId::AppendRow: {
                auto iter = _viewModel.content().rows.begin();
                std::advance(iter, row + 1);
                std::vector<std::string> values;
                for (size_t i = 0; i < _viewModel.content().columns.size(); ++i) {
                    values.emplace_back("****");
                }
                _viewModel.content().rows.insert(iter, std::move(values));
                modified = true;
            } break;
            case MenuItemId::DeleteRow: {
                auto iter = _viewModel.content().rows.begin();
                std::advance(iter, row);
                _viewModel.content().rows.erase(iter);
                modified = true;
            } break;
            default:
                break;
            }
            if (modified) {
                _viewModel.modified() = true;
                RefreshDataView();
            }
        });
        PopupMenu(&menu, event.GetPosition());
    });
    if (_viewModel.resType() == ResType::TwoDa) {
        _tableCtrl->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, [this](const auto &event) {
            enum class MenuItemId {
                RenameColumn = 1,
                AppendColumn,
                DeleteColumn
            };
            auto column = event.GetColumn();
            if (column == -1 || (_viewModel.content().rowNumberColumn && column == 0)) {
                return;
            }
            if (_viewModel.content().rowNumberColumn) {
                --column;
            }
            auto control = wxDynamicCast(event.GetEventObject(), wxDataViewListCtrl);
            wxMenu menu;
            menu.Append(static_cast<int>(MenuItemId::RenameColumn), "Rename column");
            menu.Append(static_cast<int>(MenuItemId::AppendColumn), "Append column");
            menu.Append(static_cast<int>(MenuItemId::DeleteColumn), "Delete column");
            menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [this, &column](const auto &event) {
                bool modified = false;
                switch (static_cast<MenuItemId>(event.GetId())) {
                case MenuItemId::RenameColumn: {
                    wxTextEntryDialog dialog(nullptr, "New name:", "Column rename", _viewModel.content().columns[column]);
                    if (dialog.ShowModal() == wxID_OK) {
                        auto newName = dialog.GetValue().ToStdString();
                        if (_viewModel.content().columns[column] != newName) {
                            _viewModel.content().columns[column] = newName;
                            modified = true;
                        }
                    }
                } break;
                case MenuItemId::AppendColumn: {
                    auto iter = _viewModel.content().columns.begin();
                    std::advance(iter, column + 1);
                    _viewModel.content().columns.insert(iter, "New Column");
                    for (auto &row : _viewModel.content().rows) {
                        auto valuesIter = row.begin();
                        std::advance(valuesIter, column + 1);
                        row.insert(valuesIter, "****");
                    }
                    modified = true;
                } break;
                case MenuItemId::DeleteColumn: {
                    auto iter = _viewModel.content().columns.begin();
                    std::advance(iter, column);
                    _viewModel.content().columns.erase(iter);
                    for (auto &row : _viewModel.content().rows) {
                        auto valuesIter = row.begin();
                        std::advance(valuesIter, column);
                        row.erase(valuesIter);
                    }
                    modified = true;
                } break;
                default:
                    break;
                }
                if (modified) {
                    _viewModel.modified() = true;
                    RefreshDataView();
                }
            });
            PopupMenu(&menu, event.GetPosition());
        });
    }

    auto goToRowBtn = new wxButton(this, wxID_ANY, "Go to row...");
    goToRowBtn->Bind(wxEVT_BUTTON, [this](const auto &event) {
        wxNumberEntryDialog dialog(nullptr, "Row number:", wxEmptyString, "Go to row", 0, 0, _viewModel.content().rows.size());
        if (dialog.ShowModal() == wxID_OK) {
            auto row = dialog.GetValue();
            _tableCtrl->EnsureVisible(_tableCtrl->RowToItem(row));
        }
    });

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(goToRowBtn, wxSizerFlags(0).Border(wxALL, 3));
    sizer->Add(_tableCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    SetSizer(sizer);
}

void TableResourcePanel::RefreshDataView() {
    _tableCtrl->Freeze();
    _tableCtrl->DeleteAllItems();
    _tableCtrl->ClearColumns();
    if (_viewModel.content().rowNumberColumn) {
        _tableCtrl->AppendTextColumn("Index");
    }
    for (const auto &column : _viewModel.content().columns) {
        _tableCtrl->AppendTextColumn(column, wxDATAVIEW_CELL_EDITABLE);
    }
    for (size_t i = 0; i < _viewModel.content().rows.size(); ++i) {
        auto &row = _viewModel.content().rows[i];
        auto values = wxVector<wxVariant>();
        if (_viewModel.content().rowNumberColumn) {
            values.push_back(wxVariant(std::to_string(i)));
        }
        for (auto &value : row) {
            values.push_back(wxVariant(value));
        }
        _tableCtrl->AppendItem(values);
    }
    _tableCtrl->Thaw();
}

} // namespace reone
