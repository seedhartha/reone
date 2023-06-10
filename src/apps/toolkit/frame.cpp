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

#include <wx/dataview.h>

namespace reone {

static constexpr char kIconName[] = "reone";
static constexpr int kMinPanelWidth = 640;

struct EventHandlerID {
    static constexpr int openFolderMenuItem = wxID_HIGHEST + 1;
};

ToolkitFrame::ToolkitFrame() :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(800, 600));

    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openFolderMenuItem, "&Open Folder");
    auto menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    _splitter = new wxSplitterWindow(this, wxID_ANY);
    _splitter->Bind(wxEVT_SIZE, &ToolkitFrame::OnSplitterSize, this);
    _splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &ToolkitFrame::OnSplitterSashPosChanging, this);
    _splitter->SetMinimumPaneSize(100);

    auto dataSplitter = new wxSplitterWindow(_splitter, wxID_ANY);
    dataSplitter->SetMinimumPaneSize(100);

    auto filesPanel = new wxPanel(dataSplitter);
    auto filesTreeCtrl = new wxDataViewTreeCtrl(filesPanel, wxID_ANY);
    // auto filesRootItem = filesTreeCtrl->AppendContainer(wxDataViewItem(0), "...");
    auto filesSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, "Files");
    filesSizer->Add(filesTreeCtrl, 1, wxEXPAND);
    filesPanel->SetSizer(filesSizer);

    auto modulesPanel = new wxPanel(dataSplitter);
    auto modulesListBox = new wxListBox(modulesPanel, wxID_ANY);
    // modulesListBox->AppendString("...");
    auto modulesSizer = new wxStaticBoxSizer(wxVERTICAL, modulesPanel, "Modules");
    modulesSizer->Add(modulesListBox, 1, wxEXPAND);
    modulesPanel->SetSizer(modulesSizer);

    dataSplitter->SetSashGravity(0.5);
    dataSplitter->SplitHorizontally(filesPanel, modulesPanel);

    auto glAttributes = wxGLAttributes().Defaults();
    glAttributes.EndList();
    _glCanvas = new wxGLCanvas(_splitter, glAttributes, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    _glCanvas->Bind(wxEVT_PAINT, &ToolkitFrame::OnGLCanvasPaint, this);
    auto glContext = new wxGLContext(_glCanvas);
    glContext->SetCurrent(*_glCanvas);

    _splitter->SplitVertically(dataSplitter, _glCanvas);

    CreateStatusBar();
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

void ToolkitFrame::OnGLCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(_glCanvas);

    glViewport(0, 0, _glCanvas->GetClientSize().x, _glCanvas->GetClientSize().y);

    _glCanvas->ClearBackground();
    _glCanvas->SwapBuffers();
}

wxBEGIN_EVENT_TABLE(ToolkitFrame, wxFrame) //
    wxEND_EVENT_TABLE()

} // namespace reone
