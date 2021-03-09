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

#include "../src/common/types.h"

using namespace std;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const char kIconName[] = "reone";
static const char kConfigFilename[] = "reone.cfg";

static const wxSize g_windowSize { 400, 250 };

LauncherFrame::LauncherFrame() : wxFrame(nullptr, wxID_ANY, "reone", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)) {
    // Configure this frame

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif

    SetSize(g_windowSize);


    LoadConfiguration();

    // Setup controls

    _textCtrlGameDir = new wxTextCtrl(this, WindowID::gameDir, _config.gameDir, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    _textCtrlGameDir->Bind(wxEVT_LEFT_DOWN, &LauncherFrame::OnGameDirLeftDown, this, WindowID::gameDir);

    auto gameSizer = new wxBoxSizer(wxHORIZONTAL);
    gameSizer->Add(new wxStaticText(this, wxID_ANY, "Game Directory", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    gameSizer->Add(_textCtrlGameDir, 1, wxEXPAND | wxALL, 3);

    _checkBoxDev = new wxCheckBox(this, WindowID::devMode, "Developer Mode", wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
    _checkBoxDev->SetValue(_config.devMode);

    wxArrayString resChoices;
    resChoices.Add("800x600");
    resChoices.Add("1024x768");
    resChoices.Add("1280x960");
    resChoices.Add("1280x1024");

    string configResolution(str(boost::format("%dx%d") % _config.width % _config.height));
    int resSelection = resChoices.Index(configResolution);
    if (resSelection == wxNOT_FOUND) {
        resSelection = 1;
    }

    _choiceResolution = new wxChoice(this, WindowID::resolution, wxDefaultPosition, wxDefaultSize, resChoices);
    _choiceResolution->SetSelection(resSelection);

    auto resSizer = new wxBoxSizer(wxHORIZONTAL);
    resSizer->Add(new wxStaticText(this, wxID_ANY, "Screen Resolution", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    resSizer->Add(_choiceResolution, 1, wxEXPAND | wxALL, 3);

    wxArrayString debugChoices;
    debugChoices.Add("0");
    debugChoices.Add("1");
    debugChoices.Add("2");
    debugChoices.Add("3");

    int debugSelection = _config.debug >= 0 && _config.debug <= 3 ? _config.debug : 0;

    _choiceDebug = new wxChoice(this, WindowID::debug, wxDefaultPosition, wxDefaultSize, debugChoices);
    _choiceDebug->SetSelection(debugSelection);

    auto debugSizer = new wxBoxSizer(wxHORIZONTAL);
    debugSizer->Add(new wxStaticText(this, wxID_ANY, "Debug Log Level", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    debugSizer->Add(_choiceDebug, 1, wxEXPAND | wxALL, 3);

    wxArrayString debugChannelsChoices;
    debugChannelsChoices.Add("All");
    debugChannelsChoices.Add("General");
    debugChannelsChoices.Add("GUI");
    debugChannelsChoices.Add("Script");
    debugChannelsChoices.Add("Conversation");
    debugChannelsChoices.Add("Combat");

    int debugChannelsSelection;
    switch (_config.debugch) {
        case DebugChannels::general:
            debugChannelsSelection = 1;
            break;
        case DebugChannels::gui:
            debugChannelsSelection = 2;
            break;
        case DebugChannels::script:
            debugChannelsSelection = 3;
            break;
        case DebugChannels::conversation:
            debugChannelsSelection = 4;
            break;
        case DebugChannels::combat:
            debugChannelsSelection = 5;
            break;
        default:
            debugChannelsSelection = 0;
            break;
    }

    _choiceDebugChannels = new wxChoice(this, WindowID::debugChannels, wxDefaultPosition, wxDefaultSize, debugChannelsChoices);
    _choiceDebugChannels->SetSelection(debugChannelsSelection);

    auto debugChannelsSizer = new wxBoxSizer(wxHORIZONTAL);
    debugChannelsSizer->Add(new wxStaticText(this, wxID_ANY, "Debug Channels", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE_HORIZONTAL), 1, wxEXPAND | wxALL, 3);
    debugChannelsSizer->Add(_choiceDebugChannels, 1, wxEXPAND | wxALL, 3);

    auto topSizer = new wxBoxSizer(wxVERTICAL);
    topSizer->Add(new wxButton(this, WindowID::launch, "Launch"), 0, wxEXPAND | wxALL, 3);
    topSizer->Add(gameSizer, 0, wxEXPAND, 0);
    topSizer->Add(_checkBoxDev, 0, wxEXPAND | wxALL, 3);
    topSizer->Add(resSizer, 0, wxEXPAND, 0);
    topSizer->Add(debugSizer, 0, wxEXPAND, 0);
    topSizer->Add(debugChannelsSizer, 0, wxEXPAND, 0);

    SetSizer(topSizer);
}

void LauncherFrame::LoadConfiguration() {
    po::options_description options;
    options.add_options()
        ("game", po::value<string>())
        ("dev", po::value<bool>())
        ("width", po::value<int>())
        ("height", po::value<int>())
        ("debug", po::value<int>())
        ("debugch", po::value<int>());

    po::variables_map vars;
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, options, true), vars);
    }
    po::notify(vars);

    _config.gameDir = vars.count("game") > 0 ? vars["game"].as<string>() : "";
    _config.devMode = vars.count("dev") > 0 ? vars["dev"].as<bool>() : false;
    _config.width = vars.count("width") > 0 ? vars["width"].as<int>() : 1024;
    _config.height = vars.count("height") > 0 ? vars["height"].as<int>() : 768;
    _config.debug = vars.count("debug") > 0 ? vars["debug"].as<int>() : 0;
    _config.debugch = vars.count("debugch") > 0 ? vars["debugch"].as<int>() : DebugChannels::all;
}

void LauncherFrame::OnLaunch(wxCommandEvent &event) {
    string resolution(_choiceResolution->GetStringSelection());

    vector<string> tokens;
    boost::split(tokens, resolution, boost::is_any_of("x"), boost::token_compress_on);

    int debugch = 0;
    switch (_choiceDebugChannels->GetSelection()) {
        case 1:
            debugch = DebugChannels::general;
            break;
        case 2:
            debugch = DebugChannels::gui;
            break;
        case 3:
            debugch = DebugChannels::script;
            break;
        case 4:
            debugch = DebugChannels::conversation;
            break;
        case 5:
            debugch = DebugChannels::combat;
            break;
        default:
            debugch = DebugChannels::all;
            break;
    }

    _config.gameDir = _textCtrlGameDir->GetValue();
    _config.devMode = _checkBoxDev->IsChecked();
    _config.width = stoi(tokens[0]);
    _config.height = stoi(tokens[1]);
    _config.debug = stoi(string(_choiceDebug->GetStringSelection()));
    _config.debugch = debugch;

    SaveConfiguration();

    string exe("reone");
#ifndef _WIN32
    exe.insert(0, "./");
#endif

    system(exe.c_str());

    Close(true);
}

void LauncherFrame::SaveConfiguration() {
    static set<string> recognized { "game=", "width=", "height=", "dev=", "debug=", "debugch=" };

    vector<string> lines;

    fs::ifstream in(kConfigFilename);
    for (string line; getline(in, line); ) {
        bool add = true;
        for (auto &opt : recognized) {
            if (boost::starts_with(line, opt)) {
                add = false;
                break;
            }
        }
        if (add) {
            lines.push_back(line);
        }
    }

    fs::ofstream config(kConfigFilename);
    config << "game=" << _config.gameDir << endl;
    config << "dev=" << (_config.devMode ? 1 : 0) << endl;
    config << "width=" << _config.width << endl;
    config << "height=" << _config.height << endl;
    config << "debug=" << _config.debug << endl;
    config << "debugch=" << _config.debugch << endl;
    for (auto &line : lines) {
        config << line << endl;
    }
}

void LauncherFrame::OnGameDirLeftDown(wxMouseEvent &event) {
    wxDirDialog dlg(nullptr, "Choose game directory", _textCtrlGameDir->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK) {
        _textCtrlGameDir->SetValue(dlg.GetPath());
    }
}

wxBEGIN_EVENT_TABLE(LauncherFrame, wxFrame)
EVT_BUTTON(WindowID::launch, LauncherFrame::OnLaunch)
wxEND_EVENT_TABLE()

} // namespace reone
