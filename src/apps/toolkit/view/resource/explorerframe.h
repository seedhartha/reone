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

#include "reone/audio/context.h"
#include "reone/audio/source.h"
#include "reone/game/types.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/gff.h"
#include "reone/resource/id.h"
#include "reone/system/stream/input.h"

#include "../../viewmodel/resource/audio.h"
#include "../../viewmodel/resource/explorer.h"
#include "../../viewmodel/resource/image.h"
#include "../../viewmodel/resource/model.h"

namespace reone {

class ImageResourcePanel;
class ModelResourcePanel;
class AudioResourcePanel;

class ResourceExplorerFrame : public wxFrame {
public:
    ResourceExplorerFrame();

private:
    std::unique_ptr<ResourceExplorerViewModel> _viewModel;

    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;

    std::unique_ptr<audio::AudioSource> _audioSource;

    // Menus

    wxMenuItem *_saveFileMenuItem {nullptr};

    // END Menus

    // Widgets

    wxSplitterWindow *_splitter {nullptr};
    wxProgressDialog *_progressDialog {nullptr};

    wxDataViewTreeCtrl *_filesTreeCtrl {nullptr};
    wxListBox *_modulesListBox {nullptr};
    wxAuiNotebook *_notebook {nullptr};

    ImageResourcePanel *_imagePanel {nullptr};
    ModelResourcePanel *_modelPanel {nullptr};
    AudioResourcePanel *_audioPanel {nullptr};

    // END Widgets

    void AppendGffStructToTree(wxDataViewTreeCtrl &ctrl, wxDataViewItem parent, const std::string &text, const resource::Gff &gff);

    void InvokeTool(Operation operation);

    void SaveFile();

    wxWindow *NewPageWindow(Page &page);
    wxWindow *GetStaticPageWindow(PageType type) const;

    // Events

    void OnIdle(wxIdleEvent &event);
    void OnClose(wxCloseEvent &event);

    void OnOpenGameDirectoryCommand(wxCommandEvent &event);
    void OnSaveFileCommand(wxCommandEvent &event);
    void OnExtractAllBifsCommand(wxCommandEvent &event);
    void OnBatchConvertTpcToTgaCommand(wxCommandEvent &event);
    void OnComposeLipCommand(wxCommandEvent &event);

    void OnExtractToolCommand(wxCommandEvent &event);
    void OnUnwrapToolCommand(wxCommandEvent &event);
    void OnToRimToolCommand(wxCommandEvent &event);
    void OnToErfToolCommand(wxCommandEvent &event);
    void OnToModToolCommand(wxCommandEvent &event);
    void OnToXmlToolCommand(wxCommandEvent &event);
    void OnToTwoDaToolCommand(wxCommandEvent &event);
    void OnToGffToolCommand(wxCommandEvent &event);
    void OnToTlkToolCommand(wxCommandEvent &event);
    void OnToLipToolCommand(wxCommandEvent &event);
    void OnToSsfToolCommand(wxCommandEvent &event);
    void OnToTgaToolCommand(wxCommandEvent &event);
    void OnToPcodeToolCommand(wxCommandEvent &event);
    void OnToNcsToolCommand(wxCommandEvent &event);
    void OnToNssToolCommand(wxCommandEvent &event);

    void OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event);
    void OnFilesTreeCtrlItemActivated(wxDataViewEvent &event);
    void OnFilesTreeCtrlItemContextMenu(wxDataViewEvent &event);
    void OnFilesTreeCtrlItemStartEditing(wxDataViewEvent &event);

    void OnNotebookPageClose(wxAuiNotebookEvent &event);
    void OnNotebookPageChanged(wxAuiNotebookEvent &event);

    void OnGffTreeCtrlItemStartEditing(wxDataViewEvent &event);
    void OnGffTreeCtrlItemContextMenu(wxDataViewEvent &event);

    void OnPopupCommandSelected(wxCommandEvent &event);

    void OnStopAudioCommand(wxCommandEvent &event);

    // END Events

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone
