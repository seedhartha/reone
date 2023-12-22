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
class GFFTreeNode;

struct GFFTreeItemClientData : public wxClientData {
    std::string nodeId;

    GFFTreeItemClientData(std::string nodeId) :
        nodeId(std::move(nodeId)) {
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

    std::map<std::string, wxDataViewItem> m_nodeIdToDataViewItem;

    void InitControls();
    void BindEvents();
    void BindViewModel();

    void RefreshTreeControl();
    void AppendTreeNode(const GFFTreeNode &node);

    void OnTreeCtrlItemStartEditing(wxDataViewEvent &event);
    void OnTreeCtrlItemContextMenu(wxDataViewEvent &event);
};

} // namespace reone
