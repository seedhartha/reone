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

#include <boost/program_options.hpp>

#include "../common/types.h"

using namespace std;

namespace fs = boost::filesystem;
namespace po = boost::program_options;

namespace reone {

static const char kIconName[] = "reone";
static const char kConfigFilename[] = "reone.cfg";

static const wxSize g_windowSize {640, 400};

LauncherFrame::LauncherFrame() :
    wxFrame(nullptr, wxID_ANY, "reone", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxMINIMIZE_BOX)) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif

    LoadConfiguration();

    // Setup controls

    auto labelGameDir = new wxStaticText(this, wxID_ANY, "Game Directory", wxDefaultPosition, wxDefaultSize);

    _textCtrlGameDir = new wxTextCtrl(this, WindowID::gameDir, _config.gameDir, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    _textCtrlGameDir->Bind(wxEVT_LEFT_DOWN, &LauncherFrame::OnGameDirLeftDown, this, WindowID::gameDir);

    auto gameSizer = new wxBoxSizer(wxVERTICAL);
    gameSizer->Add(labelGameDir, wxSizerFlags(0).Expand().Border(wxALL, 3));
    gameSizer->Add(_textCtrlGameDir, wxSizerFlags(0).Expand().Border(wxALL, 3));

    _checkBoxDev = new wxCheckBox(this, WindowID::devMode, "Developer Mode", wxDefaultPosition, wxDefaultSize);
    _checkBoxDev->SetValue(_config.devMode);

    // END Setup controls

    // Graphics

    // Screen Resolution

    wxArrayString resChoices;
    resChoices.Add("800x600");
    resChoices.Add("1024x768");
    resChoices.Add("1280x1024");
    resChoices.Add("1600x1200");

    string configResolution(str(boost::format("%dx%d") % _config.width % _config.height));
    int resSelection = resChoices.Index(configResolution);
    if (resSelection == wxNOT_FOUND) {
        resChoices.Add(configResolution);
        resSelection = resChoices.GetCount() - 1;
    }

    auto labelResolution = new wxStaticText(this, wxID_ANY, "Screen Resolution", wxDefaultPosition, wxDefaultSize);

    _choiceResolution = new wxChoice(this, WindowID::resolution, wxDefaultPosition, wxDefaultSize, resChoices);
    _choiceResolution->SetSelection(resSelection);

    auto resSizer = new wxBoxSizer(wxVERTICAL);
    resSizer->Add(labelResolution, wxSizerFlags(0).Expand().Border(wxALL, 3));
    resSizer->Add(_choiceResolution, wxSizerFlags(0).Expand().Border(wxALL, 3));

    // END Screen Resolution

    // Anti-Aliasing

    wxArrayString antiAliasingChoices;
    antiAliasingChoices.Add("No AA");
    antiAliasingChoices.Add("MSAA x2");
    antiAliasingChoices.Add("MSAA x4");
    antiAliasingChoices.Add("MSAA x8");

    auto labelAntiAliasing = new wxStaticText(this, wxID_ANY, "Anti-Aliasing", wxDefaultPosition, wxDefaultSize);

    _choiceAntiAliasing = new wxChoice(this, WindowID::antiAliasing, wxDefaultPosition, wxDefaultSize, antiAliasingChoices);
    _choiceAntiAliasing->SetSelection(_config.aasamples);

    auto antiAliasingSizer = new wxBoxSizer(wxVERTICAL);
    antiAliasingSizer->Add(labelAntiAliasing, wxSizerFlags(0).Expand().Border(wxALL, 3));
    antiAliasingSizer->Add(_choiceAntiAliasing, wxSizerFlags(0).Expand().Border(wxALL, 3));

    wxArrayString textureQualityChoices;
    textureQualityChoices.Add("High");
    textureQualityChoices.Add("Medium");
    textureQualityChoices.Add("Low");

    // END Anti-Aliasing

    // Texture Quality

    auto labelTextureQuality = new wxStaticText(this, wxID_ANY, "Texture Quality", wxDefaultPosition, wxDefaultSize);

    _choiceTextureQuality = new wxChoice(this, WindowID::textureQuality, wxDefaultPosition, wxDefaultSize, textureQualityChoices);
    _choiceTextureQuality->SetSelection(_config.texQuality);

    auto textureQualitySizer = new wxBoxSizer(wxVERTICAL);
    textureQualitySizer->Add(labelTextureQuality, wxSizerFlags(0).Expand().Border(wxALL, 3));
    textureQualitySizer->Add(_choiceTextureQuality, wxSizerFlags(0).Expand().Border(wxALL, 3));

    // END Texture Quality

    // Shadow Map Resolution

    wxArrayString shadowResChoices;
    shadowResChoices.Add("1024");
    shadowResChoices.Add("2048");
    shadowResChoices.Add("4096");

    auto labelShadowResolution = new wxStaticText(this, wxID_ANY, "Shadow Map Resolution", wxDefaultPosition, wxDefaultSize);

    _choiceShadowResolution = new wxChoice(this, WindowID::shadowResolution, wxDefaultPosition, wxDefaultSize, shadowResChoices);
    _choiceShadowResolution->SetSelection(_config.shadowres);

    auto shadowResSizer = new wxBoxSizer(wxVERTICAL);
    shadowResSizer->Add(labelShadowResolution, wxSizerFlags(0).Expand().Border(wxALL, 3));
    shadowResSizer->Add(_choiceShadowResolution, wxSizerFlags(0).Expand().Border(wxALL, 3));

    // END Shadow Map Resolution

    _checkBoxFullscreen = new wxCheckBox(this, WindowID::fullscreen, "Enable Fullscreen", wxDefaultPosition, wxDefaultSize);
    _checkBoxFullscreen->SetValue(_config.fullscreen);

    _checkBoxVSync = new wxCheckBox(this, WindowID::vsync, "Enable V-Sync", wxDefaultPosition, wxDefaultSize);
    _checkBoxVSync->SetValue(_config.vsync);

    _checkBoxGrass = new wxCheckBox(this, WindowID::grass, "Enable Grass", wxDefaultPosition, wxDefaultSize);
    _checkBoxGrass->SetValue(_config.grass);

    auto graphicsSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Graphics");
    graphicsSizer->Add(resSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    graphicsSizer->Add(textureQualitySizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    graphicsSizer->Add(antiAliasingSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    graphicsSizer->Add(shadowResSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    graphicsSizer->Add(_checkBoxFullscreen, wxSizerFlags(0).Expand().Border(wxALL, 3));
    graphicsSizer->Add(_checkBoxVSync, wxSizerFlags(0).Expand().Border(wxALL, 3));
    graphicsSizer->Add(_checkBoxGrass, wxSizerFlags(0).Expand().Border(wxALL, 3));

    // END Graphics

    // Audio

    auto labelVolumeMusic = new wxStaticText(this, wxID_ANY, "Music Volume", wxDefaultPosition, wxDefaultSize);
    _sliderVolumeMusic = new wxSlider(this, WindowID::musicVolume, _config.musicvol, 0, 100, wxDefaultPosition, wxDefaultSize);

    auto labelVolumeVoice = new wxStaticText(this, wxID_ANY, "Voice Volume", wxDefaultPosition, wxDefaultSize);
    _sliderVolumeVoice = new wxSlider(this, WindowID::voiceVolume, _config.voicevol, 0, 100, wxDefaultPosition, wxDefaultSize);

    auto labelVolumeSound = new wxStaticText(this, wxID_ANY, "Sound Volume", wxDefaultPosition, wxDefaultSize);
    _sliderVolumeSound = new wxSlider(this, WindowID::soundVolume, _config.soundvol, 0, 100, wxDefaultPosition, wxDefaultSize);

    auto labelVolumeMovie = new wxStaticText(this, wxID_ANY, "Movie Volume", wxDefaultPosition, wxDefaultSize);
    _sliderVolumeMovie = new wxSlider(this, WindowID::movieVolume, _config.movievol, 0, 100, wxDefaultPosition, wxDefaultSize);

    auto audioSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Audio");
    audioSizer->Add(labelVolumeMusic, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(_sliderVolumeMusic, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(labelVolumeVoice, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(_sliderVolumeVoice, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(labelVolumeSound, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(_sliderVolumeSound, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(labelVolumeMovie, wxSizerFlags(0).Expand().Border(wxALL, 3));
    audioSizer->Add(_sliderVolumeMovie, wxSizerFlags(0).Expand().Border(wxALL, 3));

    // END Audio

    // Logging

    wxArrayString logLevelChoices;
    logLevelChoices.Add("Error");
    logLevelChoices.Add("Warning");
    logLevelChoices.Add("Info");
    logLevelChoices.Add("Debug");

    auto labelLogLevel = new wxStaticText(this, wxID_ANY, "Level", wxDefaultPosition, wxDefaultSize);
    _choiceLogLevel = new wxChoice(this, WindowID::logLevel, wxDefaultPosition, wxDefaultSize, logLevelChoices);
    _choiceLogLevel->SetSelection(_config.loglevel);

    wxArrayString logChannelChoices;
    logChannelChoices.Add("Resources");
    logChannelChoices.Add("Resources (verbose)");
    logChannelChoices.Add("Graphics");
    logChannelChoices.Add("Audio");
    logChannelChoices.Add("GUI");
    logChannelChoices.Add("Perception");
    logChannelChoices.Add("Conversation");
    logChannelChoices.Add("Combat");
    logChannelChoices.Add("Script");
    logChannelChoices.Add("Script (verbose)");
    logChannelChoices.Add("Script (very verbose)");

    auto labelLogChannels = new wxStaticText(this, wxID_ANY, "Channels", wxDefaultPosition, wxDefaultSize);

    _checkListBoxLogChannels = new wxCheckListBox(this, WindowID::logChannels, wxDefaultPosition, wxDefaultSize, logChannelChoices);
    _checkListBoxLogChannels->Check(0, _config.logch & LogChannels::resources);
    _checkListBoxLogChannels->Check(1, _config.logch & LogChannels::resources2);
    _checkListBoxLogChannels->Check(2, _config.logch & LogChannels::graphics);
    _checkListBoxLogChannels->Check(3, _config.logch & LogChannels::audio);
    _checkListBoxLogChannels->Check(4, _config.logch & LogChannels::gui);
    _checkListBoxLogChannels->Check(5, _config.logch & LogChannels::perception);
    _checkListBoxLogChannels->Check(6, _config.logch & LogChannels::conversation);
    _checkListBoxLogChannels->Check(7, _config.logch & LogChannels::combat);
    _checkListBoxLogChannels->Check(8, _config.logch & LogChannels::script);
    _checkListBoxLogChannels->Check(9, _config.logch & LogChannels::script2);
    _checkListBoxLogChannels->Check(10, _config.logch & LogChannels::script3);

    _checkBoxLogFile = new wxCheckBox(this, WindowID::logFile, "Log to File", wxDefaultPosition, wxDefaultSize);
    _checkBoxLogFile->SetValue(_config.logfile);

    auto loggingSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Logging");
    loggingSizer->Add(labelLogLevel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    loggingSizer->Add(_choiceLogLevel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    loggingSizer->Add(labelLogChannels, wxSizerFlags(0).Expand().Border(wxALL, 3));
    loggingSizer->Add(_checkListBoxLogChannels, wxSizerFlags(0).Expand().Border(wxALL, 3));
    loggingSizer->Add(_checkBoxLogFile, wxSizerFlags(0).Expand().Border(wxALL, 3));

    // END Logging

    auto topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(graphicsSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));
    topSizer->Add(audioSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));
    topSizer->Add(loggingSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));

    auto topSizer2 = new wxBoxSizer(wxVERTICAL);
    topSizer2->SetMinSize(640, 100);
    topSizer2->Add(gameSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    topSizer2->Add(_checkBoxDev, wxSizerFlags(0).Expand().Border(wxALL, 3));
    topSizer2->Add(topSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    topSizer2->Add(new wxButton(this, WindowID::launch, "Launch"), wxSizerFlags(0).Expand().Border(wxALL, 3));
    topSizer2->Add(new wxButton(this, WindowID::saveConfig, "Save Configuration"), wxSizerFlags(0).Expand().Border(wxALL, 3));

    SetSizerAndFit(topSizer2);
}

void LauncherFrame::LoadConfiguration() {
    po::options_description options;
    options.add_options()                                                               //
        ("game", po::value<string>()->default_value(""))                                //
        ("dev", po::value<bool>()->default_value(false))                                //
        ("width", po::value<int>()->default_value(1024))                                //
        ("height", po::value<int>()->default_value(768))                                //
        ("fullscreen", po::value<bool>()->default_value(false))                         //
        ("vsync", po::value<bool>()->default_value(true))                               //
        ("grass", po::value<bool>()->default_value(true))                               //
        ("texquality", po::value<int>()->default_value(0))                              //
        ("aasamples", po::value<int>()->default_value(0))                               //
        ("shadowres", po::value<int>()->default_value(0))                               //
        ("musicvol", po::value<int>()->default_value(85))                               //
        ("voicevol", po::value<int>()->default_value(85))                               //
        ("soundvol", po::value<int>()->default_value(85))                               //
        ("movievol", po::value<int>()->default_value(85))                               //
        ("loglevel", po::value<int>()->default_value(static_cast<int>(LogLevel::Info))) //
        ("logch", po::value<int>()->default_value(LogChannels::general))                //
        ("logfile", po::value<bool>()->default_value(false));

    po::variables_map vars;
    if (fs::exists(kConfigFilename)) {
        po::store(po::parse_config_file<char>(kConfigFilename, options, true), vars);
    }
    po::notify(vars);

    _config.gameDir = vars["game"].as<string>();
    _config.devMode = vars["dev"].as<bool>();
    _config.width = vars["width"].as<int>();
    _config.height = vars["height"].as<int>();
    _config.fullscreen = vars["fullscreen"].as<bool>();
    _config.vsync = vars["vsync"].as<bool>();
    _config.grass = vars["grass"].as<bool>();
    _config.texQuality = vars["texquality"].as<int>();
    _config.aasamples = vars["aasamples"].as<int>();
    _config.shadowres = vars["shadowres"].as<int>();
    _config.musicvol = vars["musicvol"].as<int>();
    _config.voicevol = vars["voicevol"].as<int>();
    _config.soundvol = vars["soundvol"].as<int>();
    _config.movievol = vars["movievol"].as<int>();
    _config.loglevel = vars["loglevel"].as<int>();
    _config.logch = vars["logch"].as<int>();
    _config.logfile = vars["logfile"].as<bool>();
}

void LauncherFrame::OnLaunch(wxCommandEvent &event) {
    SaveConfiguration();

    string exe("reone");
#ifndef _WIN32
    exe.insert(0, "./");
#endif

    system(exe.c_str());

    Close(true);
}

void LauncherFrame::SaveConfiguration() {
    static set<string> recognized {
        "game=",
        "dev=",
        "width=",
        "height=",
        "fullscreen=",
        "vsync=",
        "grass=",
        "texquality=",
        "aasamples=",
        "shadowres=",
        "musicvol=",
        "voicevol=",
        "soundvol=",
        "movievol=",
        "loglevel=",
        "logch=",
        "logfile="};

    string resolution(_choiceResolution->GetStringSelection());

    vector<string> tokens;
    boost::split(tokens, resolution, boost::is_any_of("x"), boost::token_compress_on);

    int logch = LogChannels::general;
    if (_checkListBoxLogChannels->IsChecked(0)) {
        logch |= LogChannels::resources;
    }
    if (_checkListBoxLogChannels->IsChecked(1)) {
        logch |= LogChannels::resources2;
    }
    if (_checkListBoxLogChannels->IsChecked(2)) {
        logch |= LogChannels::graphics;
    }
    if (_checkListBoxLogChannels->IsChecked(3)) {
        logch |= LogChannels::audio;
    }
    if (_checkListBoxLogChannels->IsChecked(4)) {
        logch |= LogChannels::gui;
    }
    if (_checkListBoxLogChannels->IsChecked(5)) {
        logch |= LogChannels::perception;
    }
    if (_checkListBoxLogChannels->IsChecked(6)) {
        logch |= LogChannels::conversation;
    }
    if (_checkListBoxLogChannels->IsChecked(7)) {
        logch |= LogChannels::combat;
    }
    if (_checkListBoxLogChannels->IsChecked(8)) {
        logch |= LogChannels::script;
    }
    if (_checkListBoxLogChannels->IsChecked(9)) {
        logch |= LogChannels::script2;
    }
    if (_checkListBoxLogChannels->IsChecked(10)) {
        logch |= LogChannels::script3;
    }

    _config.gameDir = _textCtrlGameDir->GetValue();
    _config.devMode = _checkBoxDev->IsChecked();
    _config.width = stoi(tokens[0]);
    _config.height = stoi(tokens[1]);
    _config.fullscreen = _checkBoxFullscreen->IsChecked();
    _config.vsync = _checkBoxVSync->IsChecked();
    _config.grass = _checkBoxGrass->IsChecked();
    _config.texQuality = _choiceTextureQuality->GetSelection();
    _config.aasamples = _choiceAntiAliasing->GetSelection();
    _config.shadowres = _choiceShadowResolution->GetSelection();
    _config.musicvol = _sliderVolumeMusic->GetValue();
    _config.voicevol = _sliderVolumeVoice->GetValue();
    _config.soundvol = _sliderVolumeSound->GetValue();
    _config.movievol = _sliderVolumeMovie->GetValue();
    _config.loglevel = _choiceLogLevel->GetSelection();
    _config.logch = logch;
    _config.logfile = _checkBoxLogFile->IsChecked();

    vector<string> lines;

    fs::ifstream in(kConfigFilename);
    for (string line; getline(in, line);) {
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
    config << "fullscreen=" << (_config.fullscreen ? 1 : 0) << endl;
    config << "vsync=" << (_config.vsync ? 1 : 0) << endl;
    config << "grass=" << (_config.grass ? 1 : 0) << endl;
    config << "texquality=" << _config.texQuality << endl;
    config << "aasamples=" << _config.aasamples << endl;
    config << "shadowres=" << _config.shadowres << endl;
    config << "musicvol=" << _config.musicvol << endl;
    config << "voicevol=" << _config.voicevol << endl;
    config << "soundvol=" << _config.soundvol << endl;
    config << "movievol=" << _config.movievol << endl;
    config << "loglevel=" << _config.loglevel << endl;
    config << "logch=" << _config.logch << endl;
    config << "logfile=" << (_config.logfile ? 1 : 0) << endl;
    for (auto &line : lines) {
        config << line << endl;
    }
}

void LauncherFrame::OnSaveConfig(wxCommandEvent &event) {
    SaveConfiguration();
}

void LauncherFrame::OnGameDirLeftDown(wxMouseEvent &event) {
    wxDirDialog dlg(nullptr, "Choose game directory", _textCtrlGameDir->GetValue(), wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dlg.ShowModal() == wxID_OK) {
        _textCtrlGameDir->SetValue(dlg.GetPath());
    }
}

wxBEGIN_EVENT_TABLE(LauncherFrame, wxFrame)                       //
    EVT_BUTTON(WindowID::launch, LauncherFrame::OnLaunch)         //
    EVT_BUTTON(WindowID::saveConfig, LauncherFrame::OnSaveConfig) //
    wxEND_EVENT_TABLE()

} // namespace reone
