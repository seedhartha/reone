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

#include "../viewmodel/main.h"

namespace reone {

class MainFrame : public wxFrame {
public:
    MainFrame();

private:
    std::unique_ptr<MainViewModel> _viewModel;

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

    wxSplitterWindow *_imageSplitter {nullptr};
    wxSplitterWindow *_renderSplitter {nullptr};
    wxPanel *_animationPanel {nullptr};
    wxPanel *_audioPanel {nullptr};

    wxPanel *_imageCanvas {nullptr};
    wxTextCtrl *_imageInfoCtrl {nullptr};
    wxButton *_animPauseResumeBtn {nullptr};
    wxSlider *_animTimeSlider {nullptr};
    wxTextCtrl *_animTimeCtrl {nullptr};
    wxListBox *_animationsListBox {nullptr};
    wxGLCanvas *_glCanvas {nullptr};

    std::shared_ptr<graphics::LipAnimation> _lipAnim;
    std::unique_ptr<wxBitmap> _image;

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
    void OnFilesTreeCtrlItemEditingDone(wxDataViewEvent &event);

    void OnNotebookPageClose(wxAuiNotebookEvent &event);
    void OnNotebookPageChanged(wxAuiNotebookEvent &event);

    void OnGffTreeCtrlItemEditingDone(wxDataViewEvent &event);

    void OnPopupCommandSelected(wxCommandEvent &event);

    void OnImageCanvasPaint(wxPaintEvent &event);

    void OnGLCanvasPaint(wxPaintEvent &event);
    void OnGLCanvasMouseWheel(wxMouseEvent &event);
    void OnGLCanvasMouseMotion(wxMouseEvent &event);

    void OnAnimPauseResumeCommand(wxCommandEvent &event);
    void OnAnimTimeSliderCommand(wxCommandEvent &event);
    void OnAnimationsListBoxDoubleClick(wxCommandEvent &event);
    void OnLipLoadCommand(wxCommandEvent &event);

    void OnStopAudioCommand(wxCommandEvent &event);

    // END Events

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone
