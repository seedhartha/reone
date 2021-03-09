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
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/program_options.hpp>

using namespace std;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const char kIconName[] = "reone";
static const char kConfigFilename[] = "reone.cfg";

static const wxSize g_windowSize { 400, 150 };

LauncherFrame::LauncherFrame() : wxFrame(nullptr, wxID_ANY, "reone", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)) {
    // Configure this frame

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif

    SetSize(g_windowSize);


    LoadConfiguration();

    // Setup controls

    _textGameDir = new wxTextCtrl(this, WindowID::gameDir, _config.gameDir, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    _textGameDir->Bind(wxEVT_LEFT_DOWN, &LauncherFrame::OnGameDirLeftDown, this, WindowID::gameDir);

    wxArrayString choices;
    choices.Add("800x600");
    choices.Add("1024x768");
    choices.Add("1280x960");
    choices.Add("1280x1024");

    string configResolution(str(boost::format("%dx%d") % _config.width % _config.height));
    int selection = choices.Index(configResolution);
    if (selection == wxNOT_FOUND) {
        selection = 1;
    }

    auto gameSizer = new wxBoxSizer(wxHORIZONTAL);
    gameSizer->Add(new wxStaticText(this, wxID_ANY, "Game Directory", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    gameSizer->Add(_textGameDir, 1, wxEXPAND | wxALL, 3);

    _choiceResolution = new wxChoice(this, WindowID::resolution, wxDefaultPosition, wxDefaultSize, choices);
    _choiceResolution->SetSelection(selection);

    auto resSizer = new wxBoxSizer(wxHORIZONTAL);
    resSizer->Add(new wxStaticText(this, wxID_ANY, "Screen Resolution", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    resSizer->Add(_choiceResolution, 1, wxEXPAND | wxALL, 3);

    auto topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(new wxButton(this, WindowID::launch, "Launch"), 0, wxEXPAND | wxALL, 3);
    topSizer->Add(gameSizer, 0, wxEXPAND, 0);
    topSizer->Add(resSizer, 0, wxEXPAND, 0);

    SetSizer(topSizer);
}

void LauncherFrame::LoadConfiguration() {
    po::options_description options;
    options.add_options()
        ("game", po::value<string>()->default_value(""))
        ("width", po::value<int>()->default_value(1024))
        ("height", po::value<int>()->default_value(768));

    po::variables_map vars;
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, options, true), vars);
    }
    po::notify(vars);

    _config.gameDir = vars["game"].as<string>();
    _config.width = vars["width"].as<int>();
    _config.height = vars["height"].as<int>();
}

void LauncherFrame::OnLaunch(wxCommandEvent &event) {
    string resolution(_choiceResolution->GetStringSelection());

    vector<string> tokens;
    boost::split(tokens, resolution, boost::is_any_of("x"), boost::token_compress_on);

    _config.gameDir = _textGameDir->GetValue();
    _config.width = stoi(tokens[0]);
    _config.height = stoi(tokens[1]);

    SaveConfiguration();

    string exe("reone");
#ifndef _WIN32
    exe.insert(0, "./");
#endif

    string cmd(str(boost::format("%s --game=\"%s\" --width=%d --height=%d") % exe % _config.gameDir % _config.width % _config.height));
    system(cmd.c_str());

    Close(true);
}

void LauncherFrame::SaveConfiguration() {
    vector<string> lines;

    fs::ifstream in(kConfigFilename);
    for (string line; getline(in, line); ) {
        if (boost::starts_with(line, "game=") ||
            boost::starts_with(line, "width=") ||
            boost::starts_with(line, "height=")) continue;

        lines.push_back(line);
    }

    fs::ofstream config(kConfigFilename);
    config << "game=" << _config.gameDir << endl;
    config << "width=" << _config.width << endl;
    config << "height=" << _config.height << endl;
    for (auto &line : lines) {
        config << line << endl;
    }
}

void LauncherFrame::OnGameDirLeftDown(wxMouseEvent &event) {
    wxDirDialog dlg(nullptr, "Choose game directory", _textGameDir->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK) {
        _textGameDir->SetValue(dlg.GetPath());
    }
}

wxBEGIN_EVENT_TABLE(LauncherFrame, wxFrame)
EVT_BUTTON(WindowID::launch, LauncherFrame::OnLaunch)
wxEND_EVENT_TABLE()

} // namespace reone
