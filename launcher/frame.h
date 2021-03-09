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

#pragma once

#include <string>

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace reone {

struct WindowID {
    static constexpr wxWindowID launch = wxID_HIGHEST + 1;
    static constexpr wxWindowID gameDir = wxID_HIGHEST + 2;
    static constexpr wxWindowID devMode = wxID_HIGHEST + 3;
    static constexpr wxWindowID resolution = wxID_HIGHEST + 4;
    static constexpr wxWindowID fullscreen = wxID_HIGHEST + 5;
    static constexpr wxWindowID debug = wxID_HIGHEST + 6;
    static constexpr wxWindowID debugChannels = wxID_HIGHEST + 7;
    static constexpr wxWindowID logFile = wxID_HIGHEST + 8;
};

class LauncherFrame : public wxFrame {
public:
    LauncherFrame();

private:
    struct Configuration {
        std::string gameDir;
        bool devMode { false };
        int width { 0 };
        int height { 0 };
        bool fullscreen { false };
        int debug { 0 };
        int debugch { 0 };
        bool logfile { false };
    } _config;

    wxTextCtrl *_textCtrlGameDir;
    wxChoice *_choiceResolution;
    wxCheckBox *_checkBoxDev;
    wxCheckBox *_checkBoxFullscreen;
    wxChoice *_choiceDebugLevel;
    wxChoice *_choiceDebugChannels;
    wxCheckBox *_checkBoxLogFile;

    void OnLaunch(wxCommandEvent &event);
    void OnGameDirLeftDown(wxMouseEvent &event);

    void LoadConfiguration();
    void SaveConfiguration();

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone
