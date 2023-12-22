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
    m_viewModel(viewModel) {

    InitControls();
    BindEvents();
    RefreshDataView();
}

void TableResourcePanel::InitControls() {
    m_tableCtrl = new wxDataViewListCtrl(this, wxID_ANY);
    m_goToRowBtn = new wxButton(this, wxID_ANY, "Go to row...");

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_goToRowBtn, wxSizerFlags(0).Border(wxALL, 3));
    sizer->Add(m_tableCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    SetSizer(sizer);
}

void TableResourcePanel::BindEvents() {
    m_tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, [this](const auto &event) {
        auto item = event.GetItem();
        if (!item) {
            return;
        }
        auto control = wxDynamicCast(event.GetEventObject(), wxDataViewListCtrl);
        auto column = event.GetColumn();
        if (m_viewModel.content().rowNumberColumn) {
            --column;
        }
        auto row = control->ItemToRow(item);
        auto newValue = event.GetValue().GetString();
        if (m_viewModel.content().rows[row][column] != newValue) {
            m_viewModel.content().rows[row][column] = newValue;
            m_viewModel.modified() = true;
        }
    });
    m_tableCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, [this](const auto &event) {
        enum class MenuItemId {
            AppendRow = 1,
            DeleteRow
        };
        auto column = event.GetColumn();
        if (m_viewModel.content().rowNumberColumn && column == 0) {
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
                auto iter = m_viewModel.content().rows.begin();
                std::advance(iter, row + 1);
                std::vector<std::string> values;
                for (size_t i = 0; i < m_viewModel.content().columns.size(); ++i) {
                    values.emplace_back("****");
                }
                m_viewModel.content().rows.insert(iter, std::move(values));
                modified = true;
            } break;
            case MenuItemId::DeleteRow: {
                auto iter = m_viewModel.content().rows.begin();
                std::advance(iter, row);
                m_viewModel.content().rows.erase(iter);
                modified = true;
            } break;
            default:
                break;
            }
            if (modified) {
                m_viewModel.modified() = true;
                RefreshDataView();
            }
        });
        PopupMenu(&menu, event.GetPosition());
    });
    if (m_viewModel.resType() == ResType::TwoDa) {
        m_tableCtrl->Bind(wxEVT_DATAVIEW_COLUMN_HEADER_RIGHT_CLICK, [this](const auto &event) {
            enum class MenuItemId {
                RenameColumn = 1,
                AppendColumn,
                DeleteColumn
            };
            auto column = event.GetColumn();
            if (column == -1 || (m_viewModel.content().rowNumberColumn && column == 0)) {
                return;
            }
            if (m_viewModel.content().rowNumberColumn) {
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
                    wxTextEntryDialog dialog(nullptr, "New name:", "Column rename", m_viewModel.content().columns[column].name);
                    if (dialog.ShowModal() == wxID_OK) {
                        auto newName = dialog.GetValue().ToStdString();
                        if (m_viewModel.content().columns[column].name != newName) {
                            m_viewModel.content().columns[column].name = newName;
                            modified = true;
                        }
                    }
                } break;
                case MenuItemId::AppendColumn: {
                    auto iter = m_viewModel.content().columns.begin();
                    std::advance(iter, column + 1);
                    m_viewModel.content().columns.insert(iter, {"New Column"});
                    for (auto &row : m_viewModel.content().rows) {
                        auto valuesIter = row.begin();
                        std::advance(valuesIter, column + 1);
                        row.insert(valuesIter, "****");
                    }
                    modified = true;
                } break;
                case MenuItemId::DeleteColumn: {
                    auto iter = m_viewModel.content().columns.begin();
                    std::advance(iter, column);
                    m_viewModel.content().columns.erase(iter);
                    for (auto &row : m_viewModel.content().rows) {
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
                    m_viewModel.modified() = true;
                    RefreshDataView();
                }
            });
            PopupMenu(&menu, event.GetPosition());
        });
    }
    m_goToRowBtn->Bind(wxEVT_BUTTON, [this](const auto &event) {
        wxNumberEntryDialog dialog(nullptr, "Row number:", wxEmptyString, "Go to row", 0, 0, m_viewModel.content().rows.size());
        if (dialog.ShowModal() == wxID_OK) {
            auto row = dialog.GetValue();
            m_tableCtrl->EnsureVisible(m_tableCtrl->RowToItem(row));
        }
    });
}

void TableResourcePanel::RefreshDataView() {
    m_tableCtrl->Freeze();
    m_tableCtrl->DeleteAllItems();
    m_tableCtrl->ClearColumns();
    if (m_viewModel.content().rowNumberColumn) {
        m_tableCtrl->AppendTextColumn("Index");
    }
    for (size_t i = 0; i < m_viewModel.content().columns.size(); ++i) {
        const auto &column = m_viewModel.content().columns.at(i);
        if (column.choices.empty()) {
            m_tableCtrl->AppendTextColumn(column.name, wxDATAVIEW_CELL_EDITABLE);
        } else {
            wxArrayString choices;
            for (const auto &choice : column.choices) {
                choices.Add(choice);
            }
            m_tableCtrl->AppendColumn(new wxDataViewColumn(
                column.name,
                new wxDataViewChoiceRenderer {choices},
                m_viewModel.content().rowNumberColumn ? i + 1 : i));
        }
    }
    for (size_t i = 0; i < m_viewModel.content().rows.size(); ++i) {
        auto &row = m_viewModel.content().rows[i];
        auto values = wxVector<wxVariant>();
        if (m_viewModel.content().rowNumberColumn) {
            values.push_back(wxVariant(std::to_string(i)));
        }
        for (auto &value : row) {
            values.push_back(wxVariant(value));
        }
        m_tableCtrl->AppendItem(values);
    }
    m_tableCtrl->Thaw();
}

} // namespace reone
