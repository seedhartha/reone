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
#include "reone/system/hexutil.h"

#include "../../viewmodel/resource/gff.h"

using namespace reone::resource;

namespace reone {

GFFResourcePanel::GFFResourcePanel(GFFResourceViewModel &viewModel,
                                   wxWindow *parent) :
    wxPanel(parent),
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
                                             const Gff &gff) {
    auto structItem = ctrl.AppendContainer(parent, str(boost::format("%s [%d]") % text % static_cast<int>(gff.type())));
    for (auto &field : gff.fields()) {
        switch (field.type) {
        case Gff::FieldType::CExoString:
        case Gff::FieldType::ResRef: {
            auto cleaned = boost::replace_all_copy(field.strValue, "\n", "\\n");
            ctrl.AppendItem(structItem, str(boost::format("%s = \"%s\" [%d]") % field.label % cleaned % static_cast<int>(field.type)));
        } break;
        case Gff::FieldType::CExoLocString: {
            auto locStringItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            ctrl.AppendItem(locStringItem, str(boost::format("StrRef = %d") % field.intValue));
            ctrl.AppendItem(locStringItem, str(boost::format("Substring = \"%s\"") % field.strValue));
            if (field.intValue != -1) {
                // TODO [toolkit]: accept talk table in args
                // auto tlkText = _viewModel->getTalkTableText(field.intValue);
                // auto cleanedTlkText = boost::replace_all_copy(tlkText, "\n", "\\n");
                // ctrl.AppendItem(locStringItem, str(boost::format("TalkTableText = \"%s\"") % cleanedTlkText));
            }
        } break;
        case Gff::FieldType::Void:
            ctrl.AppendItem(structItem, str(boost::format("%s = \"%s\" [%d]") % field.label % hexify(field.data, "") % static_cast<int>(field.type)));
            break;
        case Gff::FieldType::Struct:
            AppendGffStructToTree(ctrl, structItem, field.label, *field.children[0]);
            break;
        case Gff::FieldType::List: {
            auto listItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            for (auto it = field.children.begin(); it != field.children.end(); ++it) {
                auto childIdx = std::distance(field.children.begin(), it);
                AppendGffStructToTree(ctrl, listItem, std::to_string(childIdx), **it);
            }
        } break;
        case Gff::FieldType::Orientation: {
            auto orientationItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            ctrl.AppendItem(orientationItem, str(boost::format("W = %f") % field.quatValue.w));
            ctrl.AppendItem(orientationItem, str(boost::format("X = %f") % field.quatValue.x));
            ctrl.AppendItem(orientationItem, str(boost::format("Y = %f") % field.quatValue.y));
            ctrl.AppendItem(orientationItem, str(boost::format("Z = %f") % field.quatValue.z));
        } break;
        case Gff::FieldType::Vector: {
            auto vectorItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            ctrl.AppendItem(vectorItem, str(boost::format("X = %f") % field.vecValue.x));
            ctrl.AppendItem(vectorItem, str(boost::format("Y = %f") % field.vecValue.y));
            ctrl.AppendItem(vectorItem, str(boost::format("Z = %f") % field.vecValue.z));
        } break;
        case Gff::FieldType::StrRef:
            ctrl.AppendItem(structItem, str(boost::format("%s = %d [%d]") % field.label % field.intValue % static_cast<int>(field.type)));
            break;
        default:
            ctrl.AppendItem(structItem, str(boost::format("%s = %s [%d]") % field.label % field.toString() % static_cast<int>(field.type)));
            break;
        }
    }
}

void GFFResourcePanel::OnGffTreeCtrlItemStartEditing(wxDataViewEvent &event) {
    event.Veto();
}

void GFFResourcePanel::OnGffTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    enum class MenuItemId {
        AddField,
        RenameField,
        SetFieldValue,
        SetFieldType,
        DeleteField,
        AddListItem,
        DuplicateListItem,
        DeleteListItem
    };
    auto item = event.GetItem();
    if (!item.IsOk()) {
        return;
    }
    auto control = wxDynamicCast(event.GetEventObject(), wxDataViewTreeCtrl);
    wxMenu menu;
    menu.Append(static_cast<int>(MenuItemId::AddField), "Add field...");
    menu.Append(static_cast<int>(MenuItemId::SetFieldValue), "Set field value...");
    menu.Append(static_cast<int>(MenuItemId::SetFieldType), "Set field type...");
    menu.Append(static_cast<int>(MenuItemId::RenameField), "Rename field...");
    menu.Append(static_cast<int>(MenuItemId::DeleteField), "Delete field");
    menu.Append(static_cast<int>(MenuItemId::AddListItem), "Add list item...");
    menu.Append(static_cast<int>(MenuItemId::DuplicateListItem), "Duplicate list item");
    menu.Append(static_cast<int>(MenuItemId::DeleteListItem), "Delete list item");
    menu.Bind(wxEVT_COMMAND_MENU_SELECTED, [](wxCommandEvent &event) {
        wxMessageBox("Hello, world!");
    });
    PopupMenu(&menu, event.GetPosition());
}

} // namespace reone
