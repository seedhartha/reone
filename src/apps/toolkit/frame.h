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
#include <wx/glcanvas.h>
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/stc/stc.h>

#include "reone/audio/context.h"
#include "reone/audio/source.h"
#include "reone/game/types.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/gff.h"
#include "reone/resource/id.h"
#include "reone/resource/strings.h"
#include "reone/system/stream/input.h"
#include "reone/tools/tool.h"

namespace reone {

class ToolkitFrame : public wxFrame {
public:
    ToolkitFrame(audio::AudioContext &audioCtx);

private:
    struct FilesEntry {
        boost::filesystem::path path;
        std::unique_ptr<resource::ResourceId> resId;
        bool loaded {false};
        bool archived {false};
    };

    boost::filesystem::path _gamePath;
    reone::game::GameID _gameId {reone::game::GameID::KotOR};
    std::vector<resource::KeyReader::KeyEntry> _keyKeys;
    std::vector<resource::KeyReader::FileEntry> _keyFiles;
    resource::Strings _strings;

    std::map<void *, FilesEntry> _files;
    std::vector<std::shared_ptr<Tool>> _tools;

    audio::AudioContext &_audioCtx;
    std::unique_ptr<audio::AudioSource> _audioSource;
    resource::ResourceId _audioResId;

    // Widgets

    wxSplitterWindow *_splitter {nullptr};

    wxDataViewTreeCtrl *_filesTreeCtrl {nullptr};
    wxListBox *_modulesListBox {nullptr};
    wxNotebook *_notebook {nullptr};

    wxPanel *_textPanel {nullptr};
    wxPanel *_tablePanel {nullptr};
    wxPanel *_talkTablePanel {nullptr};
    wxPanel *_gffPanel {nullptr};
    wxPanel *_xmlPanel {nullptr};
    wxPanel *_nssPanel {nullptr};
    wxPanel *_pcodePanel {nullptr};
    wxSplitterWindow *_imageSplitter {nullptr};
    wxPanel *_renderPanel {nullptr};
    wxPanel *_audioPanel {nullptr};

    wxTextCtrl *_plainTextCtrl {nullptr};
    wxDataViewListCtrl *_tableCtrl {nullptr};
    wxDataViewListCtrl *_talkTableCtrl {nullptr};
    wxDataViewTreeCtrl *_gffTreeCtrl {nullptr};
    wxStyledTextCtrl *_xmlTextCtrl {nullptr};
    wxTextCtrl *_pcodeTextCtrl {nullptr};
    wxStyledTextCtrl *_nssTextCtrl {nullptr};
    wxPanel *_imageCanvas {nullptr};
    wxTextCtrl *_imageInfoCtrl {nullptr};
    wxGLCanvas *_glCanvas {nullptr};

    std::unique_ptr<wxBitmap> _image;

    wxTimer _audioTimer;

    // END Widgets

    void OpenFile(FilesEntry &entry);
    void OpenResource(resource::ResourceId &id, IInputStream &data);

    void AppendGffStructToTree(wxDataViewItem parent, const std::string &text, const resource::Gff &gff);

    void InvokeTool(Operation operation);

    // Events

    void OnOpenGameDirectoryCommand(wxCommandEvent &event);
    void OnExtractAllBifsCommand(wxCommandEvent &event);
    void OnBatchConvertTpcToTgaCommand(wxCommandEvent &event);

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

    void OnSplitterSize(wxSizeEvent &event);
    void OnSplitterSashPosChanging(wxSplitterEvent &event);

    void OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event);
    void OnFilesTreeCtrlItemActivated(wxDataViewEvent &event);
    void OnFilesTreeCtrlItemContextMenu(wxDataViewEvent &event);
    void OnFilesTreeCtrlItemEditingDone(wxDataViewEvent &event);

    void OnGffTreeCtrlItemEditingDone(wxDataViewEvent &event);

    void OnPopupCommandSelected(wxCommandEvent &event);

    void OnImageCanvasPaint(wxPaintEvent &event);
    void OnGLCanvasPaint(wxPaintEvent &event);

    void OnAudioTimer(wxTimerEvent &event);
    void OnStopAudioCommand(wxCommandEvent &event);

    // END Events

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone
