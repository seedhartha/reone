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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dataview.h>
#include <wx/panel.h>

#include "reone/resource/gff.h"

namespace reone {

namespace resource {

class TalkTable;

}

class GFFResourceViewModel;

struct GFFTreeItemClientData : public wxClientData {
    resource::Gff &gff;
    std::optional<std::reference_wrapper<resource::Gff::Field>> field;
    std::optional<std::reference_wrapper<resource::Gff::Field>> parentField;
    std::optional<std::reference_wrapper<resource::Gff>> parentFieldGff;
    std::optional<int> parentListIdx;

    GFFTreeItemClientData(resource::Gff &gff,
                          std::optional<std::reference_wrapper<resource::Gff::Field>> field = std::nullopt,
                          std::optional<std::reference_wrapper<resource::Gff::Field>> parentField = std::nullopt,
                          std::optional<std::reference_wrapper<resource::Gff>> parentFieldGff = std::nullopt,
                          std::optional<int> parentListIdx = std::nullopt) :
        gff(gff),
        field(std::move(field)),
        parentField(std::move(parentField)),
        parentFieldGff(std::move(parentFieldGff)),
        parentListIdx(std::move(parentListIdx)) {
    }
};

class GFFResourcePanel : public wxPanel {
public:
    GFFResourcePanel(GFFResourceViewModel &viewModel,
                     const resource::TalkTable &talkTable,
                     wxWindow *parent);

private:
    GFFResourceViewModel &m_viewModel;
    const resource::TalkTable &m_talkTable;

    wxDataViewTreeCtrl *m_treeCtrl {nullptr};

    void InitControls();
    void BindEvents();

    void AppendGffStructToTree(wxDataViewItem parent,
                               const std::string &text,
                               resource::Gff &gff,
                               std::optional<std::reference_wrapper<resource::Gff::Field>> parentField = std::nullopt,
                               std::optional<std::reference_wrapper<resource::Gff>> parentFieldGff = std::nullopt,
                               std::optional<int> parentListIdx = std::nullopt);

    void OnGffTreeCtrlItemStartEditing(wxDataViewEvent &event);
    void OnGffTreeCtrlItemContextMenu(wxDataViewEvent &event);
};

} // namespace reone
