/*
* Copyright (c) 2020-2021 The reone project contributors
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

#include <cstdlib>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace std;

namespace reone {

static const char kIconName[] = "reone";

static const wxSize g_windowSize { 400, 150 };

LauncherFrame::LauncherFrame() : wxFrame(nullptr, wxID_ANY, "reone", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)) {
    SetIcon(wxIcon(kIconName));
    SetSize(g_windowSize);

    wxArrayString choices;
    choices.Add("800x600");
    choices.Add("1024x768");
    choices.Add("1280x960");
    choices.Add("1280x1024");

    _choiceResolution = new wxChoice(this, WindowID::choiceResolution, wxDefaultPosition, wxDefaultSize, choices);
    _choiceResolution->SetSelection(1);

    auto resSizer = new wxBoxSizer(wxHORIZONTAL);
    resSizer->Add(new wxStaticText(this, wxID_ANY, "Screen Resolution", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    resSizer->Add(_choiceResolution, 1, wxEXPAND | wxALL, 3);

    auto topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(new wxButton(this, WindowID::btnLaunch, "Launch"), 0, wxEXPAND | wxALL, 3);
    topSizer->Add(resSizer, 0, wxEXPAND, 0);

    SetSizer(topSizer);
}

void LauncherFrame::OnLaunch(wxCommandEvent &event) {
    string resolution(_choiceResolution->GetStringSelection());
    if (resolution.empty()) return;

    vector<string> tokens;
    boost::split(tokens, resolution, boost::is_any_of("x"), boost::token_compress_on);

    int width = stoi(tokens[0]);
    int height = stoi(tokens[1]);

    string cmd(str(boost::format("reone --width=%d --height=%d") % width % height));
    system(cmd.c_str());

    Close(true);
}

wxBEGIN_EVENT_TABLE(LauncherFrame, wxFrame)
EVT_BUTTON(WindowID::btnLaunch, LauncherFrame::OnLaunch)
wxEND_EVENT_TABLE()

} // namespace reone
