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

#include "frame.h"

#include <wx/dirdlg.h>

#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/rimreader.h"
#include "reone/resource/typeutil.h"
#include "reone/system/pathutil.h"
#include "reone/system/stream/fileinput.h"

using namespace std;

namespace reone {

static constexpr char kIconName[] = "reone";
static constexpr int kMinPanelWidth = 640;

static const set<string> kFilesSubdirectoryWhitelist {
    "data", "lips", "modules", "override", "rims", "saves",      //
    "streammusic", "streamsounds", "streamwaves", "streamvoice", //
    "texturepacks"                                               //
};

static const set<string> kFilesArchiveWhitelist {
    ".bif", ".erf", ".sav", ".rim", ".mod" //
};

static const set<string> kFilesRegularBlacklist {
    ".lnk", ".bat", ".exe", ".dll", ".ini", ".ico",                    //
    ".zip", ".pdf", ".txt",                                            //
    ".hashdb", ".info", ".script", ".dat", ".msg", ".sdb", ".ds_store" //
};

struct EventHandlerID {
    static constexpr int openGameDir = wxID_HIGHEST + 1;
};

ToolkitFrame::ToolkitFrame() :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(1024, 768));
    Maximize();

    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openGameDir, "&Open game directory...");
    auto menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    _splitter = new wxSplitterWindow(this, wxID_ANY);
    _splitter->Bind(wxEVT_SIZE, &ToolkitFrame::OnSplitterSize, this);
    _splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &ToolkitFrame::OnSplitterSashPosChanging, this);
    _splitter->SetMinimumPaneSize(300);

    auto dataSplitter = new wxSplitterWindow(_splitter, wxID_ANY);
    dataSplitter->SetMinimumPaneSize(300);

    auto filesPanel = new wxPanel(dataSplitter);
    _filesTreeCtrl = new wxDataViewTreeCtrl(filesPanel, wxID_ANY);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &ToolkitFrame::OnFilesTreeCtrlItemExpanding, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &ToolkitFrame::OnFilesTreeCtrlItemEditingDone, this);
    auto filesSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, "Files");
    filesSizer->Add(_filesTreeCtrl, 1, wxEXPAND);
    filesPanel->SetSizer(filesSizer);

    auto modulesPanel = new wxPanel(dataSplitter);
    _modulesListBox = new wxListBox(modulesPanel, wxID_ANY);
    auto modulesSizer = new wxStaticBoxSizer(wxVERTICAL, modulesPanel, "Modules");
    modulesSizer->Add(_modulesListBox, 1, wxEXPAND);
    modulesPanel->SetSizer(modulesSizer);

    dataSplitter->SetSashGravity(0.5);
    dataSplitter->SplitHorizontally(filesPanel, modulesPanel);

    auto renderPanel = new wxPanel(_splitter);
    auto renderSizer = new wxStaticBoxSizer(wxVERTICAL, renderPanel, "3D");
    auto glAttributes = wxGLAttributes().Defaults();
    glAttributes.EndList();
    //_glCanvas = new wxGLCanvas(renderPanel, glAttributes, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    //_glCanvas->Bind(wxEVT_PAINT, &ToolkitFrame::OnGLCanvasPaint, this);
    // auto glContext = new wxGLContext(_glCanvas);
    // glContext->SetCurrent(*_glCanvas);
    // renderSizer->Add(_glCanvas, 1, wxEXPAND);
    renderPanel->SetSizer(renderSizer);

    _splitter->SplitVertically(dataSplitter, renderPanel);

    CreateStatusBar();
}

void ToolkitFrame::OnOpenGameDirectoryMenu(wxCommandEvent &event) {
    auto dialog = new wxDirDialog(nullptr, "Choose game directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog->ShowModal() != wxID_OK) {
        return;
    }
    auto gamePath = boost::filesystem::path((string)dialog->GetPath());
    auto keyPath = getPathIgnoreCase(gamePath, "chitin.key", false);
    auto modulesPath = getPathIgnoreCase(gamePath, "modules", false);
    if (keyPath.empty() || modulesPath.empty()) {
        wxMessageBox("Not a valid game directory", "Error", wxICON_ERROR);
        return;
    }

    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = resource::KeyReader();
    keyReader.load(key);
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    _filesTreeCtrl->DeleteAllItems();
    for (auto &file : boost::filesystem::directory_iterator(gamePath)) {
        auto filename = boost::to_lower_copy(file.path().filename().string());
        auto extension = boost::to_lower_copy(file.path().extension().string());
        void *itemId;
        if ((file.status().type() == boost::filesystem::directory_file && kFilesSubdirectoryWhitelist.count(filename) > 0) || kFilesArchiveWhitelist.count(filename) > 0) {
            auto item = _filesTreeCtrl->AppendContainer(wxDataViewItem(), filename);
            itemId = item.GetID();
        } else if (file.status().type() == boost::filesystem::regular_file && kFilesRegularBlacklist.count(extension) == 0) {
            auto item = _filesTreeCtrl->AppendItem(wxDataViewItem(), filename);
            itemId = item.GetID();
        } else {
            continue;
        }
        auto entry = FilesEntry();
        entry.path = file.path();
        _files.insert(make_pair(itemId, std::move(entry)));
    }

    _modulesListBox->Clear();
    for (auto &entry : boost::filesystem::directory_iterator(modulesPath)) {
        auto filename = boost::to_lower_copy(entry.path().filename().string());
        auto extension = boost::to_lower_copy(entry.path().extension().string());
        if (extension != ".rim" && extension != ".mod") {
            continue;
        }
        if (extension == ".rim" && boost::ends_with(filename, "_s.rim")) {
            continue;
        }
        auto moduleName = boost::to_lower_copy(entry.path().filename().replace_extension().string());
        _modulesListBox->AppendString(moduleName);
    }
}

void ToolkitFrame::OnSplitterSize(wxSizeEvent &event) {
    int requestedPanelSize = event.GetSize().x - _splitter->GetSashSize() - _splitter->GetSashPosition();
    if (requestedPanelSize < kMinPanelWidth) {
        _splitter->SetSashPosition(event.GetSize().x - _splitter->GetSashSize() - kMinPanelWidth);
    }
    event.Skip();
}

void ToolkitFrame::OnSplitterSashPosChanging(wxSplitterEvent &event) {
    int requestedPanelSize = _splitter->GetSize().x - _splitter->GetSashSize() - event.GetSashPosition();
    if (requestedPanelSize < kMinPanelWidth) {
        event.SetSashPosition(_splitter->GetSize().x - _splitter->GetSashSize() - kMinPanelWidth);
    }
}

void ToolkitFrame::OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    auto &item = _files.at(itemId);
    if (item.loaded) {
        return;
    }
    if (boost::filesystem::is_directory(item.path)) {
        for (auto &file : boost::filesystem::directory_iterator(item.path)) {
            auto filename = boost::to_lower_copy(file.path().filename().string());
            auto extension = boost::to_lower_copy(file.path().extension().string());
            void *itemId;
            if (file.status().type() == boost::filesystem::directory_file || kFilesArchiveWhitelist.count(extension) > 0) {
                auto item = _filesTreeCtrl->AppendContainer(event.GetItem(), filename);
                itemId = item.GetID();
            } else if (file.status().type() == boost::filesystem::regular_file && kFilesRegularBlacklist.count(extension) == 0) {
                auto item = _filesTreeCtrl->AppendItem(event.GetItem(), filename);
                itemId = item.GetID();
            } else {
                continue;
            }
            auto entry = FilesEntry();
            entry.path = file.path();
            _files.insert(make_pair(itemId, std::move(entry)));
        }
    } else {
        auto extension = boost::to_lower_copy(item.path.extension().string());
        if (boost::ends_with(extension, ".bif")) {
            auto filename = str(boost::format("data/%s") % boost::to_lower_copy(item.path.filename().string()));
            auto maybeFile = std::find_if(_keyFiles.begin(), _keyFiles.end(), [&filename](auto &file) {
                return boost::to_lower_copy(file.filename) == filename;
            });
            if (maybeFile != _keyFiles.end()) {
                auto bifIdx = std::distance(_keyFiles.begin(), maybeFile);
                for (auto &key : _keyKeys) {
                    if (key.bifIdx != bifIdx) {
                        continue;
                    }
                    auto itemText = str(boost::format("%s.%s") % key.resId.resRef % resource::getExtByResType(key.resId.type));
                    _filesTreeCtrl->AppendItem(event.GetItem(), itemText);
                }
            }
        } else if (boost::ends_with(extension, ".erf") || boost::ends_with(extension, ".sav") || boost::ends_with(extension, ".mod")) {
            auto erf = FileInputStream(item.path, OpenMode::Binary);
            auto erfReader = resource::ErfReader();
            erfReader.load(erf);
            auto &keys = erfReader.keys();
            for (auto &key : keys) {
                auto itemText = str(boost::format("%s.%s") % key.resId.resRef % resource::getExtByResType(key.resId.type));
                _filesTreeCtrl->AppendItem(event.GetItem(), itemText);
            }
        } else if (boost::ends_with(extension, ".rim")) {
            auto rim = FileInputStream(item.path, OpenMode::Binary);
            auto rimReader = resource::RimReader();
            rimReader.load(rim);
            auto &resources = rimReader.resources();
            for (auto &resource : resources) {
                auto itemText = str(boost::format("%s.%s") % resource.resId.resRef % resource::getExtByResType(resource.resId.type));
                _filesTreeCtrl->AppendItem(event.GetItem(), itemText);
            }
        }
    }
    item.loaded = true;
}

void ToolkitFrame::OnFilesTreeCtrlItemEditingDone(wxDataViewEvent &event) {
    event.Veto();
}

void ToolkitFrame::OnGLCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(_glCanvas);

    glViewport(0, 0, _glCanvas->GetClientSize().x, _glCanvas->GetClientSize().y);

    _glCanvas->ClearBackground();
    _glCanvas->SwapBuffers();
}

wxBEGIN_EVENT_TABLE(ToolkitFrame, wxFrame)                                       //
    EVT_MENU(EventHandlerID::openGameDir, ToolkitFrame::OnOpenGameDirectoryMenu) //
    wxEND_EVENT_TABLE()

} // namespace reone
