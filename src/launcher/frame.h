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

#include <wx/wxprec.h>

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

namespace reone {

struct WindowID {
    static constexpr wxWindowID gameDir = wxID_HIGHEST + 1;
    static constexpr wxWindowID launch = wxID_HIGHEST + 2;
    static constexpr wxWindowID saveConfig = wxID_HIGHEST + 3;
};

class LauncherFrame : public wxFrame {
public:
    LauncherFrame();

private:
    struct Configuration {
        std::string gameDir;
        bool devMode {false};
        int width {0};
        int height {0};
        bool fullscreen {false};
        bool vsync {true};
        int texQuality {0};
        bool grass {true};
        bool dynroomlighting {false};
        int aasamples {0};
        int shadowres {0};
        int drawdist {64};
        int musicvol {0};
        int voicevol {0};
        int soundvol {0};
        int movievol {0};
        int loglevel {0};
        int logch {0};
        bool logfile {false};
    } _config;

    wxTextCtrl *_textCtrlGameDir;
    wxCheckBox *_checkBoxDev;
    wxChoice *_choiceResolution;
    wxChoice *_choiceTextureQuality;
    wxChoice *_choiceAntiAliasing;
    wxChoice *_choiceShadowResolution;
    wxSlider *_sliderDrawDistance;
    wxCheckBox *_checkBoxFullscreen;
    wxCheckBox *_checkBoxVSync;
    wxCheckBox *_checkBoxGrass;
    wxCheckBox *_checkBoxDynamicRoomLighting;
    wxSlider *_sliderVolumeMusic;
    wxSlider *_sliderVolumeVoice;
    wxSlider *_sliderVolumeSound;
    wxSlider *_sliderVolumeMovie;
    wxChoice *_choiceLogLevel;
    wxCheckListBox *_checkListBoxLogChannels;
    wxCheckBox *_checkBoxLogFile;

    void OnLaunch(wxCommandEvent &event);
    void OnSaveConfig(wxCommandEvent &event);
    void OnGameDirLeftDown(wxMouseEvent &event);

    void LoadConfiguration();
    void SaveConfiguration();

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone
