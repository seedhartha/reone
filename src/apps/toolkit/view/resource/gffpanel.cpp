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

#include "gffpanel.h"

#include <wx/statline.h>

#include "reone/resource/gff.h"
#include "reone/resource/talktable.h"
#include "reone/system/hexutil.h"

#include "../../viewmodel/resource/gff.h"

using namespace reone::resource;

namespace reone {

GFFResourcePanel::GFFResourcePanel(GFFResourceViewModel &viewModel,
                                   const TalkTable &talkTable,
                                   wxWindow *parent) :
    wxPanel(parent),
    _talkTable(talkTable),
    _viewModel(viewModel) {

    auto treeCtrl = new wxDataViewTreeCtrl(this, wxID_ANY);
    treeCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &GFFResourcePanel::OnGffTreeCtrlItemStartEditing, this);
    treeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &GFFResourcePanel::OnGffTreeCtrlItemContextMenu, this);
    treeCtrl->Freeze();
    AppendGffStructToTree(*treeCtrl, wxDataViewItem(), "/", viewModel.content());
    treeCtrl->Thaw();

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(treeCtrl, 1, wxEXPAND);
    SetSizer(sizer);
}

void GFFResourcePanel::AppendGffStructToTree(wxDataViewTreeCtrl &ctrl,
                                             wxDataViewItem parent,
                                             const std::string &text,
                                             Gff &gff,
                                             std::optional<std::reference_wrapper<Gff::Field>> parentField,
                                             std::optional<std::reference_wrapper<Gff>> parentFieldGff,
                                             std::optional<int> parentListIdx) {
    auto structItem = ctrl.AppendContainer(
        parent,
        str(boost::format("%s [%d]") % text % static_cast<int>(gff.type())),
        -1,
        -1,
        new GFFTreeItemClientData {gff, std::nullopt, parentField, parentFieldGff, parentListIdx});
    ctrl.Expand(structItem);
    for (auto &field : gff.fields()) {
        switch (field.type) {
        case Gff::FieldType::CExoString:
        case Gff::FieldType::ResRef: {
            auto cleaned = boost::replace_all_copy(field.strValue, "\n", "\\n");
            ctrl.AppendItem(
                structItem,
                str(boost::format("%s = \"%s\" [%d]") % field.label % cleaned % static_cast<int>(field.type)),
                -1,
                new GFFTreeItemClientData {gff, field});
        } break;
        case Gff::FieldType::CExoLocString: {
            auto locStringItem = ctrl.AppendContainer(
                structItem,
                str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)),
                -1,
                -1,
                new GFFTreeItemClientData {gff, field});
            ctrl.Expand(locStringItem);
            ctrl.AppendItem(locStringItem, str(boost::format("StrRef = %d") % field.intValue));
            ctrl.AppendItem(locStringItem, str(boost::format("Substring = \"%s\"") % field.strValue));
            if (field.intValue != -1) {
                auto tlkText = _talkTable.getString(field.intValue).text;
                auto cleanedTlkText = boost::replace_all_copy(tlkText, "\n", "\\n");
                ctrl.AppendItem(locStringItem, str(boost::format("TalkTableText = \"%s\"") % cleanedTlkText));
            }
        } break;
        case Gff::FieldType::Void:
            ctrl.AppendItem(
                structItem,
                str(boost::format("%s = \"%s\" [%d]") % field.label % hexify(field.data, "") % static_cast<int>(field.type)),
                -1,
                new GFFTreeItemClientData {gff, field});
            break;
        case Gff::FieldType::Struct:
            AppendGffStructToTree(ctrl, structItem, field.label, *field.children[0], field);
            break;
        case Gff::FieldType::List: {
            auto listItem = ctrl.AppendContainer(
                structItem,
                str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)),
                -1,
                -1,
                new GFFTreeItemClientData {gff, field});
            ctrl.Expand(listItem);
            for (auto it = field.children.begin(); it != field.children.end(); ++it) {
                auto childIdx = std::distance(field.children.begin(), it);
                AppendGffStructToTree(ctrl, listItem, std::to_string(childIdx), **it, field, gff, childIdx);
            }
        } break;
        case Gff::FieldType::Orientation: {
            auto orientationItem = ctrl.AppendContainer(
                structItem,
                str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)),
                -1,
                -1,
                new GFFTreeItemClientData {gff, field});
            ctrl.Expand(orientationItem);
            ctrl.AppendItem(orientationItem, str(boost::format("W = %f") % field.quatValue.w));
            ctrl.AppendItem(orientationItem, str(boost::format("X = %f") % field.quatValue.x));
            ctrl.AppendItem(orientationItem, str(boost::format("Y = %f") % field.quatValue.y));
            ctrl.AppendItem(orientationItem, str(boost::format("Z = %f") % field.quatValue.z));
        } break;
        case Gff::FieldType::Vector: {
            auto vectorItem = ctrl.AppendContainer(
                structItem,
                str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)),
                -1,
                -1,
                new GFFTreeItemClientData {gff, field});
            ctrl.Expand(vectorItem);
            ctrl.AppendItem(vectorItem, str(boost::format("X = %f") % field.vecValue.x));
            ctrl.AppendItem(vectorItem, str(boost::format("Y = %f") % field.vecValue.y));
            ctrl.AppendItem(vectorItem, str(boost::format("Z = %f") % field.vecValue.z));
        } break;
        case Gff::FieldType::StrRef:
            ctrl.AppendItem(
                structItem,
                str(boost::format("%s = %d [%d]") % field.label % field.intValue % static_cast<int>(field.type)),
                -1,
                new GFFTreeItemClientData {gff, field});
            break;
        default:
            ctrl.AppendItem(
                structItem,
                str(boost::format("%s = %s [%d]") % field.label % field.toString() % static_cast<int>(field.type)),
                -1,
                new GFFTreeItemClientData {gff, field});
            break;
        }
    }
}

void GFFResourcePanel::OnGffTreeCtrlItemStartEditing(wxDataViewEvent &event) {
    event.Veto();
}

void GFFResourcePanel::OnGffTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    enum class MenuItemId {
        AppendListItem,
        DuplicateListItem,
        DeleteListItem,
        AppendField,
        RenameField,
        SetFieldValue,
        SetFieldType,
        DeleteField
    };
    auto item = event.GetItem();
    if (!item.IsOk()) {
        return;
    }
    auto control = wxDynamicCast(event.GetEventObject(), wxDataViewTreeCtrl);
    auto clientData = static_cast<GFFTreeItemClientData *>(control->GetItemData(item));
    if (!clientData) {
        return;
    }
    wxMenu menu;
    auto &gff = clientData->gff;
    auto &field = clientData->field;
    auto &parentField = clientData->parentField;
    auto &parentFieldGff = clientData->parentFieldGff;
    auto &parentListIdx = clientData->parentListIdx;
    if (field) {
        if (field->get().type == Gff::FieldType::Struct) {
            menu.Append(static_cast<int>(MenuItemId::AppendField), "Append field");
        } else if (field->get().type == Gff::FieldType::List) {
            menu.Append(static_cast<int>(MenuItemId::AppendListItem), "Append list item");
        } else {
            menu.Append(static_cast<int>(MenuItemId::SetFieldValue), "Set field value...");
        }
        menu.Append(static_cast<int>(MenuItemId::SetFieldType), "Set field type...");
        menu.Append(static_cast<int>(MenuItemId::RenameField), "Rename field...");
        menu.Append(static_cast<int>(MenuItemId::DeleteField), "Delete field");
    } else {
        menu.Append(static_cast<int>(MenuItemId::AppendField), "Append field");
        if (parentField) {
            if (parentListIdx) {
                menu.Append(static_cast<int>(MenuItemId::DuplicateListItem), "Duplicate list item");
                menu.Append(static_cast<int>(MenuItemId::DeleteListItem), "Delete list item");
            } else {
                menu.Append(static_cast<int>(MenuItemId::SetFieldType), "Set field type...");
                menu.Append(static_cast<int>(MenuItemId::RenameField), "Rename field...");
                menu.Append(static_cast<int>(MenuItemId::DeleteField), "Delete field");
            }
        }
    }
    menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [&](wxCommandEvent &event) {
        bool refresh = false;
        switch (static_cast<MenuItemId>(event.GetId())) {
        case MenuItemId::AppendListItem: {
            auto child = Gff::Builder().build();
            field->get().children.push_back(std::move(child));
            refresh = true;
            break;
        }
        case MenuItemId::DuplicateListItem: {
            auto listIter = parentField->get().children.begin();
            std::advance(listIter, *parentListIdx);
            auto copy = parentField->get().children.at(*parentListIdx)->deepCopy();
            parentField->get().children.insert(listIter, std::move(copy));
            refresh = true;
            break;
        }
        case MenuItemId::DeleteListItem: {
            auto listIter = parentField->get().children.begin();
            std::advance(listIter, *parentListIdx);
            parentField->get().children.erase(listIter);
            refresh = true;
        } break;
        case MenuItemId::AppendField: {
            auto field = Gff::Field::newInt("New Field", 0);
            gff.fields().push_back(std::move(field));
            refresh = true;
            break;
        }
        case MenuItemId::RenameField: {
            wxTextEntryDialog dialog(nullptr, "New field name:", "Field rename", field->get().label);
            if (dialog.ShowModal() == wxID_OK) {
                field->get().label = dialog.GetValue().ToStdString();
                refresh = true;
            }
            break;
        }
        case MenuItemId::SetFieldValue: {
            switch (field->get().type) {
            case Gff::FieldType::Char:
            case Gff::FieldType::Short:
            case Gff::FieldType::Int:
            case Gff::FieldType::StrRef: {
                auto value = std::to_string(field->get().intValue);
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto value = std::stol(dialog.GetValue().ToStdString());
                    if (field->get().intValue != value) {
                        field->get().intValue = value;
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Byte:
            case Gff::FieldType::Word:
            case Gff::FieldType::Dword: {
                auto value = std::to_string(field->get().uintValue);
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto value = std::stoul(dialog.GetValue().ToStdString());
                    if (field->get().uintValue != value) {
                        field->get().uintValue = value;
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Int64: {
                auto value = std::to_string(field->get().int64Value);
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto value = std::stoll(dialog.GetValue().ToStdString());
                    if (field->get().int64Value != value) {
                        field->get().int64Value = value;
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Dword64: {
                auto value = std::to_string(field->get().uint64Value);
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto value = std::stoull(dialog.GetValue().ToStdString());
                    if (field->get().uint64Value != value) {
                        field->get().uint64Value = value;
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Float: {
                auto value = std::to_string(field->get().floatValue);
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = std::stof(dialog.GetValue().ToStdString());
                    if (field->get().floatValue != newValue) {
                        field->get().floatValue = newValue;
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Double: {
                auto value = std::to_string(field->get().doubleValue);
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = std::stod(dialog.GetValue().ToStdString());
                    if (field->get().doubleValue != newValue) {
                        field->get().doubleValue = newValue;
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::CExoString:
            case Gff::FieldType::ResRef: {
                auto value = field->get().toString();
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = dialog.GetValue().ToStdString();
                    if (field->get().strValue != newValue) {
                        field->get().strValue = std::move(newValue);
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::CExoLocString: {
                wxDialog dialog {nullptr, wxID_ANY, "Field value change"};
                auto strRefLabel = new wxStaticText(&dialog, wxID_ANY, "StrRef:");
                auto strRefCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().intValue));
                strRefCtrl->SetMinSize(wxSize {400, strRefCtrl->GetMinSize().GetHeight()});
                auto substringLabel = new wxStaticText(&dialog, wxID_ANY, "Substring:");
                auto substringCtrl = new wxTextCtrl(&dialog, wxID_ANY, field->get().strValue);
                substringCtrl->SetMinSize(wxSize {400, substringCtrl->GetMinSize().GetHeight()});
                auto okBtn = new wxButton(&dialog, wxID_ANY, "OK");
                okBtn->Bind(wxEVT_BUTTON, [&dialog](const auto &event) { dialog.EndModal(wxID_OK); });
                auto cancelBtn = new wxButton(&dialog, wxID_ANY, "Cancel");
                cancelBtn->Bind(wxEVT_BUTTON, [&dialog](const auto &event) { dialog.EndModal(wxID_CANCEL); });
                auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
                buttonsSizer->Add(okBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
                buttonsSizer->Add(cancelBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
                auto sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(strRefLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(strRefCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(substringLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(substringCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(buttonsSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
                dialog.SetSizerAndFit(sizer);
                if (dialog.ShowModal() == wxID_OK) {
                    auto newStrRef = std::stoi(strRefCtrl->GetValue().ToStdString());
                    auto newSubstring = substringCtrl->GetValue().ToStdString();
                    if (field->get().intValue != newStrRef || field->get().strValue != newSubstring) {
                        field->get().intValue = newStrRef;
                        field->get().strValue = std::move(newSubstring);
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Orientation: {
                wxDialog dialog {nullptr, wxID_ANY, "Field value change"};
                auto wLabel = new wxStaticText(&dialog, wxID_ANY, "W:");
                auto wCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().quatValue.w));
                wCtrl->SetMinSize(wxSize {400, wCtrl->GetMinSize().GetHeight()});
                auto xLabel = new wxStaticText(&dialog, wxID_ANY, "X:");
                auto xCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().quatValue.x));
                xCtrl->SetMinSize(wxSize {400, xCtrl->GetMinSize().GetHeight()});
                auto yLabel = new wxStaticText(&dialog, wxID_ANY, "Y:");
                auto yCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().quatValue.y));
                yCtrl->SetMinSize(wxSize {400, yCtrl->GetMinSize().GetHeight()});
                auto zLabel = new wxStaticText(&dialog, wxID_ANY, "Z:");
                auto zCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().quatValue.z));
                zCtrl->SetMinSize(wxSize {400, zCtrl->GetMinSize().GetHeight()});
                auto okBtn = new wxButton(&dialog, wxID_ANY, "OK");
                okBtn->Bind(wxEVT_BUTTON, [&dialog](const auto &event) { dialog.EndModal(wxID_OK); });
                auto cancelBtn = new wxButton(&dialog, wxID_ANY, "Cancel");
                cancelBtn->Bind(wxEVT_BUTTON, [&dialog](const auto &event) { dialog.EndModal(wxID_CANCEL); });
                auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
                buttonsSizer->Add(okBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
                buttonsSizer->Add(cancelBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
                auto sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(wLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(wCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(xLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(xCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(yLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(yCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(zLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(zCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(buttonsSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
                dialog.SetSizerAndFit(sizer);
                if (dialog.ShowModal() == wxID_OK) {
                    float w = std::stof(wCtrl->GetValue().ToStdString());
                    float x = std::stof(xCtrl->GetValue().ToStdString());
                    float y = std::stof(yCtrl->GetValue().ToStdString());
                    float z = std::stof(zCtrl->GetValue().ToStdString());
                    auto newValue = glm::quat {w, x, y, z};
                    if (field->get().quatValue != newValue) {
                        field->get().quatValue = std::move(newValue);
                        refresh = true;
                    }
                }
            } break;
            case Gff::FieldType::Vector: {
                wxDialog dialog {nullptr, wxID_ANY, "Field value change"};
                auto xLabel = new wxStaticText(&dialog, wxID_ANY, "X:");
                auto xCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().vecValue.x));
                xCtrl->SetMinSize(wxSize {400, xCtrl->GetMinSize().GetHeight()});
                auto yLabel = new wxStaticText(&dialog, wxID_ANY, "Y:");
                auto yCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().vecValue.y));
                yCtrl->SetMinSize(wxSize {400, yCtrl->GetMinSize().GetHeight()});
                auto zLabel = new wxStaticText(&dialog, wxID_ANY, "Z:");
                auto zCtrl = new wxTextCtrl(&dialog, wxID_ANY, std::to_string(field->get().vecValue.z));
                zCtrl->SetMinSize(wxSize {400, zCtrl->GetMinSize().GetHeight()});
                auto okBtn = new wxButton(&dialog, wxID_ANY, "OK");
                okBtn->Bind(wxEVT_BUTTON, [&dialog](const auto &event) { dialog.EndModal(wxID_OK); });
                auto cancelBtn = new wxButton(&dialog, wxID_ANY, "Cancel");
                cancelBtn->Bind(wxEVT_BUTTON, [&dialog](const auto &event) { dialog.EndModal(wxID_CANCEL); });
                auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
                buttonsSizer->Add(okBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
                buttonsSizer->Add(cancelBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
                auto sizer = new wxBoxSizer(wxVERTICAL);
                sizer->Add(xLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(xCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(yLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(yCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(zLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(zCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
                sizer->Add(buttonsSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
                dialog.SetSizerAndFit(sizer);
                if (dialog.ShowModal() == wxID_OK) {
                    float x = std::stof(xCtrl->GetValue().ToStdString());
                    float y = std::stof(yCtrl->GetValue().ToStdString());
                    float z = std::stof(zCtrl->GetValue().ToStdString());
                    auto newValue = glm::vec3 {x, y, z};
                    if (field->get().vecValue != newValue) {
                        field->get().vecValue = std::move(newValue);
                    }
                    refresh = true;
                }
            } break;
            case Gff::FieldType::Void: {
                auto value = hexify(field->get().data, "");
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = dialog.GetValue().ToStdString();
                    if (value != newValue) {
                        field->get().data = unhexify(newValue);
                        refresh = true;
                    }
                }
            } break;
            default:
                break;
            }
            break;
        }
        case MenuItemId::SetFieldType: {
            const char *choices[] {
                "Byte",          //
                "Char",          //
                "Word",          //
                "Short",         //
                "Dword",         //
                "Int",           //
                "Dword64",       //
                "Int64",         //
                "Float",         //
                "Double",        //
                "CExoString",    //
                "ResRef",        //
                "CExoLocString", //
                "Void",          //
                "Struct",        //
                "List",          //
                "Orientation",   //
                "Vector",        //
                "StrRef"         //
            };
            wxSingleChoiceDialog dialog {nullptr, "New field type:", "Field type change", wxArrayString {19, choices}};
            dialog.SetSelection(static_cast<int>(field->get().type));
            if (dialog.ShowModal() == wxID_OK) {
                auto newType = static_cast<Gff::FieldType>(dialog.GetSelection());
                if (field->get().type != newType) {
                    field->get().type = newType;
                    field->get().strValue = "";
                    field->get().vecValue = glm::vec3 {0.0f};
                    field->get().quatValue = glm::quat {1.0f, 0.0f, 0.0f, 0.0f};
                    field->get().data.clear();
                    field->get().children.clear();
                    if (field->get().type == Gff::FieldType::Struct) {
                        field->get().children.push_back(Gff::Builder().build());
                    }
                    field->get().intValue = 0;
                    field->get().uintValue = 0;
                    field->get().int64Value = 0;
                    field->get().uint64Value = 0;
                    field->get().floatValue = 0.0f;
                    field->get().doubleValue = 0.0f;
                    refresh = true;
                }
            }
        } break;
        case MenuItemId::DeleteField: {
            if (field) {
                for (auto it = gff.fields().begin(); it != gff.fields().end();) {
                    if (it->label == field->get().label) {
                        it = gff.fields().erase(it);
                        break;
                    }
                    ++it;
                }
                refresh = true;
            } else if (parentField) {
                for (auto it = parentFieldGff->get().fields().begin(); it != parentFieldGff->get().fields().end();) {
                    if (it->label == parentField->get().label) {
                        it = gff.fields().erase(it);
                        break;
                    }
                    ++it;
                }
                refresh = true;
            }
            break;
        }
        default:
            break;
        }
        if (refresh) {
            control->Freeze();
            control->DeleteAllItems();
            AppendGffStructToTree(*control, wxDataViewItem(), "/", _viewModel.content());
            control->Thaw();
        }
    });
    PopupMenu(&menu, event.GetPosition());
}

} // namespace reone
