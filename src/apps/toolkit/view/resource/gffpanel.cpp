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

#include "reone/resource/gff.h"
#include "reone/resource/talktable.h"
#include "reone/system/hexutil.h"

#include "../../viewmodel/resource/gff.h"

#include "gff/locstringvaluedialog.h"
#include "gff/orientvaluedialog.h"
#include "gff/vectorvaluedialog.h"

using namespace reone::resource;

namespace reone {

GFFResourcePanel::GFFResourcePanel(GFFResourceViewModel &viewModel,
                                   const TalkTable &talkTable,
                                   wxWindow *parent) :
    wxPanel(parent),
    m_talkTable(talkTable),
    m_viewModel(viewModel) {

    InitControls();
    BindEvents();
    BindViewModel();
    RefreshTreeControl();
}

void GFFResourcePanel::InitControls() {
    m_treeCtrl = new wxDataViewTreeCtrl(this, wxID_ANY);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_treeCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    SetSizer(sizer);
}

void GFFResourcePanel::BindEvents() {
    m_treeCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &GFFResourcePanel::OnTreeCtrlItemStartEditing, this);
    m_treeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &GFFResourcePanel::OnTreeCtrlItemContextMenu, this);
}

void GFFResourcePanel::BindViewModel() {
    m_viewModel.treeNodes().addChangedHandler([this](const auto &args) {
        switch (args.type) {
        case CollectionChangeType::Add:
            AppendTreeNode(*args.addedItem->get());
            break;
        case CollectionChangeType::Remove: {
            auto nodeId = args.removedItem->get()->id;
            auto item = m_nodeIdToDataViewItem.at(nodeId);
            m_nodeIdToDataViewItem.erase(nodeId);
            m_treeCtrl->DeleteItem(item);
            break;
        }
        case CollectionChangeType::Reset:
            RefreshTreeControl();
            break;
        }
    });
}

void GFFResourcePanel::RefreshTreeControl() {
    std::set<GFFTreeNodeId> viewModelNodes;
    for (const auto &node : *m_viewModel.treeNodes()) {
        viewModelNodes.emplace(node->id);
    }
    std::set<GFFTreeNodeId> viewNodes;
    for (const auto &[nodeId, item] : m_nodeIdToDataViewItem) {
        viewNodes.emplace(nodeId);
    }
    m_treeCtrl->Freeze();
    for (const auto &nodeId : viewNodes) {
        if (viewModelNodes.count(nodeId) > 0) {
            continue;
        }
        m_treeCtrl->DeleteItem(m_nodeIdToDataViewItem.at(nodeId));
        m_nodeIdToDataViewItem.erase(nodeId);
    }
    for (const auto &node : *m_viewModel.treeNodes()) {
        if (viewNodes.count(node->id) == 0) {
            AppendTreeNode(m_viewModel.treeNodeById(node->id));
        } else {
            bool viewModelContainer = m_viewModel.isContainerNode(node->id);
            auto &viewModelText = node->displayName;
            auto &item = m_nodeIdToDataViewItem.at(node->id);
            bool viewContainer = m_treeCtrl->IsContainer(item);
            auto viewText = m_treeCtrl->GetItemText(item);
            if (viewContainer == viewModelContainer && viewText != viewModelText) {
                m_treeCtrl->SetItemText(item, viewModelText);
            } else if (viewContainer != viewModelContainer) {
                m_treeCtrl->DeleteItem(item);
                m_nodeIdToDataViewItem.erase(node->id);
                AppendTreeNode(*node);
            }
        }
    }
    m_treeCtrl->Thaw();
}

void GFFResourcePanel::AppendTreeNode(const GFFTreeNode &node) {
    wxDataViewItem parentItem;
    if (node.parentId) {
        parentItem = m_nodeIdToDataViewItem.at(*node.parentId);
    }
    wxDataViewItem item;
    if (m_viewModel.isContainerNode(node.id)) {
        item = m_treeCtrl->AppendContainer(parentItem, node.displayName, -1, -1, new GFFTreeItemClientData {node.id});
    } else {
        item = m_treeCtrl->AppendItem(parentItem, node.displayName, -1, new GFFTreeItemClientData {node.id});
    }
    m_nodeIdToDataViewItem.insert({node.id, item});
}

void GFFResourcePanel::OnTreeCtrlItemStartEditing(wxDataViewEvent &event) {
    event.Veto();
}

void GFFResourcePanel::OnTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    enum class MenuItemId {
        SetStructType,
        AppendField,
        RenameField,
        SetFieldType,
        SetFieldValue,
        DeleteField,
        ClearListItems,
        AppendListItem,
        DuplicateListItem,
        DeleteListItem,
    };
    auto item = event.GetItem();
    if (!item.IsOk()) {
        return;
    }
    auto control = wxDynamicCast(event.GetEventObject(), wxDataViewTreeCtrl);
    auto data = static_cast<GFFTreeItemClientData *>(control->GetItemData(item));
    if (!data) {
        return;
    }
    const auto &node = m_viewModel.treeNodeById(data->nodeId);
    wxMenu menu;
    switch (node.type) {
    case GFFTreeNodeType::Struct:
    case GFFTreeNodeType::StructField:
        menu.Append(static_cast<int>(MenuItemId::SetStructType), "Set struct type...");
        menu.Append(static_cast<int>(MenuItemId::AppendField), "Append field");
        if (node.type == GFFTreeNodeType::StructField) {
            menu.AppendSeparator();
            menu.Append(static_cast<int>(MenuItemId::RenameField), "Rename field...");
            menu.Append(static_cast<int>(MenuItemId::SetFieldType), "Set field type...");
            menu.Append(static_cast<int>(MenuItemId::DeleteField), "Delete field");
        }
        break;
    case GFFTreeNodeType::Field:
    case GFFTreeNodeType::ListField:
        menu.Append(static_cast<int>(MenuItemId::RenameField), "Rename field...");
        if (node.type == GFFTreeNodeType::ListField) {
            menu.Append(static_cast<int>(MenuItemId::ClearListItems), "Clear list items");
            menu.Append(static_cast<int>(MenuItemId::AppendListItem), "Append list item");
        } else {
            menu.Append(static_cast<int>(MenuItemId::SetFieldValue), "Set field value...");
        }
        menu.Append(static_cast<int>(MenuItemId::SetFieldType), "Set field type...");
        menu.Append(static_cast<int>(MenuItemId::DeleteField), "Delete field");
        break;
    case GFFTreeNodeType::FieldComponent:
        break;
    case GFFTreeNodeType::ListItemStruct:
        menu.Append(static_cast<int>(MenuItemId::SetStructType), "Set struct type...");
        menu.Append(static_cast<int>(MenuItemId::AppendField), "Append field");
        menu.Append(static_cast<int>(MenuItemId::DuplicateListItem), "Duplicate list item");
        menu.Append(static_cast<int>(MenuItemId::DeleteListItem), "Delete list item");
        break;
    }
    menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [&](wxCommandEvent &event) {
        switch (static_cast<MenuItemId>(event.GetId())) {
        case MenuItemId::SetStructType: {
            auto &gff = m_viewModel.gffByTreeNodeId(node.id);
            wxTextEntryDialog dialog {nullptr, "New struct type:", "Struct type change", std::to_string(gff.type())};
            if (dialog.ShowModal() == wxID_OK) {
                auto newValue = std::stoul(dialog.GetValue().ToStdString());
                if (newValue != gff.type()) {
                    m_viewModel.setStructType(node.id, newValue);
                }
            }
            break;
        }
        case MenuItemId::AppendField: {
            m_viewModel.appendField(node.id);
            break;
        }
        case MenuItemId::RenameField: {
            auto &field = m_viewModel.fieldByTreeNodeId(node.id);
            wxTextEntryDialog dialog {nullptr, "New field name:", "Field rename", field.label};
            if (dialog.ShowModal() == wxID_OK) {
                auto newName = dialog.GetValue().ToStdString();
                if (newName != field.label) {
                    m_viewModel.renameField(node.id, newName);
                }
            }
            break;
        }
        case MenuItemId::SetFieldType: {
            const char *choices[] {
                "Byte", "Char", "Word", "Short", "Dword", "Int", "Dword64", "Int64", //
                "Float", "Double", "CExoString", "ResRef", "CExoLocString", "Void",  //
                "Struct", "List", "Orientation", "Vector", "StrRef"                  //
            };
            wxSingleChoiceDialog dialog {nullptr, "New field type:", "Field type change", wxArrayString {19, choices}};
            auto &field = m_viewModel.fieldByTreeNodeId(node.id);
            dialog.SetSelection(static_cast<int>(field.type));
            if (dialog.ShowModal() == wxID_OK) {
                auto newType = static_cast<Gff::FieldType>(dialog.GetSelection());
                if (newType != field.type) {
                    m_viewModel.setFieldType(node.id, newType);
                }
            }
            break;
        }
        case MenuItemId::SetFieldValue: {
            auto &field = m_viewModel.fieldByTreeNodeId(node.id);
            switch (field.type) {
            case Gff::FieldType::CExoLocString: {
                LocStringFieldValueDialog dialog {nullptr, "Field value change", field.intValue, field.strValue};
                if (dialog.ShowModal() == wxID_OK) {
                    int strRef = dialog.GetStrRef();
                    auto substring = dialog.GetSubstring();
                    if (strRef != field.intValue || substring != field.strValue) {
                        m_viewModel.modifyField(node.id, [&strRef, &substring](auto &field) {
                            field.intValue = strRef;
                            field.strValue = substring;
                        });
                    }
                }
                break;
            }
            case Gff::FieldType::Orientation: {
                OrientationFieldValueDialog dialog {nullptr, "Field value change", field.quatValue};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = dialog.GetValue();
                    if (newValue != field.quatValue) {
                        m_viewModel.modifyField(node.id, [&newValue](auto &field) {
                            field.quatValue = newValue;
                        });
                    }
                }
                break;
            }
            case Gff::FieldType::Vector: {
                VectorFieldValueDialog dialog {nullptr, "Field value change", field.vecValue};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = dialog.GetValue();
                    if (newValue != field.vecValue) {
                        m_viewModel.modifyField(node.id, [&newValue](auto &field) {
                            field.vecValue = newValue;
                        });
                    }
                }
                break;
            }
            default: {
                std::string value;
                switch (field.type) {
                case Gff::FieldType::Byte:
                case Gff::FieldType::Word:
                case Gff::FieldType::Dword:
                    value = std::to_string(field.uintValue);
                    break;
                case Gff::FieldType::Char:
                case Gff::FieldType::Short:
                case Gff::FieldType::Int:
                case Gff::FieldType::StrRef:
                    value = std::to_string(field.intValue);
                    break;
                case Gff::FieldType::Dword64:
                    value = std::to_string(field.uint64Value);
                    break;
                case Gff::FieldType::Int64:
                    value = std::to_string(field.int64Value);
                    break;
                case Gff::FieldType::Float:
                    value = std::to_string(field.floatValue);
                    break;
                case Gff::FieldType::Double:
                    value = std::to_string(field.doubleValue);
                    break;
                case Gff::FieldType::CExoString:
                case Gff::FieldType::ResRef:
                    value = field.strValue;
                    break;
                case Gff::FieldType::Void:
                    value = hexify(field.data);
                    break;
                default:
                    throw std::logic_error("Unexpected field type: " + std::to_string(static_cast<int>(field.type)));
                }
                wxTextEntryDialog dialog {nullptr, "New field value:", "Field value change", value};
                if (dialog.ShowModal() == wxID_OK) {
                    auto newValue = dialog.GetValue().ToStdString();
                    if (newValue != value) {
                        m_viewModel.modifyField(node.id, [&newValue](auto &field) {
                            switch (field.type) {
                            case Gff::FieldType::Byte:
                            case Gff::FieldType::Word:
                            case Gff::FieldType::Dword:
                                field.uintValue = std::stoul(newValue);
                                break;
                            case Gff::FieldType::Char:
                            case Gff::FieldType::Short:
                            case Gff::FieldType::Int:
                            case Gff::FieldType::StrRef:
                                field.intValue = std::stol(newValue);
                                break;
                            case Gff::FieldType::Dword64:
                                field.uint64Value = std::stoull(newValue);
                                break;
                            case Gff::FieldType::Int64:
                                field.int64Value = std::stoll(newValue);
                                break;
                            case Gff::FieldType::Float:
                                field.floatValue = std::stof(newValue);
                                break;
                            case Gff::FieldType::Double:
                                field.doubleValue = std::stod(newValue);
                                break;
                            case Gff::FieldType::CExoString:
                            case Gff::FieldType::ResRef:
                                field.strValue = newValue;
                                break;
                            case Gff::FieldType::Void:
                                field.data = unhexify(newValue);
                                break;
                            default:
                                throw std::logic_error("Unexpected field type: " + std::to_string(static_cast<int>(field.type)));
                            }
                        });
                    }
                }
                break;
            }
            }
            break;
        }
        case MenuItemId::DeleteField: {
            m_viewModel.deleteField(node.id);
            break;
        }
        case MenuItemId::ClearListItems: {
            m_viewModel.clearListItems(node.id);
            break;
        }
        case MenuItemId::AppendListItem: {
            m_viewModel.appendListItem(node.id);
            break;
        }
        case MenuItemId::DuplicateListItem: {
            m_viewModel.duplicateListItem(node.id);
            break;
        }
        case MenuItemId::DeleteListItem: {
            m_viewModel.deleteListItem(node.id);
            break;
        }
        }
    });
    PopupMenu(&menu, event.GetPosition());
}

} // namespace reone
