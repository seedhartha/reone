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

#include <wx/aui/auibook.h>
#include <wx/dataview.h>
#include <wx/glcanvas.h>
#include <wx/progdlg.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>

// Xlib conflicts workaround
#ifdef None
#undef None
#endif
#ifdef Always
#undef Always
#endif

#include "../../viewmodel/resource/explorer.h"

namespace reone {

class ImageResourcePanel;
class ModelResourcePanel;
class AudioResourcePanel;

class ResourceExplorerFrame : public wxFrame {
public:
    ResourceExplorerFrame(ResourceExplorerViewModel &viewModel);

private:
    ResourceExplorerViewModel &m_viewModel;

    std::map<ResourcesItemId, wxDataViewItem> _resItemIdToTreeItem;

    // Menus

    wxMenuItem *m_saveFileMenuItem {nullptr};

    // END Menus

    // Widgets

    wxSplitterWindow *m_splitter {nullptr};
    wxProgressDialog *m_progressDialog {nullptr};

    wxDataViewTreeCtrl *m_resourcesTreeCtrl {nullptr};
    wxListBox *m_modulesListBox {nullptr};
    wxAuiNotebook *m_notebook {nullptr};

    ImageResourcePanel *m_imagePanel {nullptr};
    ModelResourcePanel *m_modelPanel {nullptr};
    AudioResourcePanel *m_audioPanel {nullptr};

    // END Widgets

    void InitControls();
    void InitMenu();
    void BindEvents();
    void BindViewModel();

    void SaveFile();

    wxWindow *NewPageWindow(Page &page);
    wxWindow *GetStaticPageWindow(PageType type) const;

    // Events

    void OnIdle(wxIdleEvent &event);
    void OnClose(wxCloseEvent &event);

    void OnOpenDirectoryCommand(wxCommandEvent &event);
    void OnExtractAllBifsCommand(wxCommandEvent &event);
    void OnBatchConvertTpcToTgaCommand(wxCommandEvent &event);
    void OnComposeLipCommand(wxCommandEvent &event);

    void OnResourcesTreeCtrlItemExpanding(wxDataViewEvent &event);
    void OnResourcesTreeCtrlItemActivated(wxDataViewEvent &event);
    void OnResourcesTreeCtrlItemContextMenu(wxDataViewEvent &event);
    void OnResourcesTreeCtrlItemStartEditing(wxDataViewEvent &event);

    void OnNotebookPageClose(wxAuiNotebookEvent &event);
    void OnNotebookPageChanged(wxAuiNotebookEvent &event);

    void OnPopupCommandSelected(wxCommandEvent &event);

    // END Events
};

} // namespace reone
