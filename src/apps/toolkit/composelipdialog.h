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

#include <wx/valnum.h>

namespace reone {

class ComposeLipDialog : public wxDialog {
public:
    ComposeLipDialog(wxWindow *parent,
                     wxWindowID id,
                     const wxString &title,
                     const wxPoint &pos = wxDefaultPosition,
                     const wxSize &size = wxDefaultSize,
                     long style = wxDEFAULT_DIALOG_STYLE);

private:
    wxFloatingPointValidator<float> _soundLengthValidator;
    float _soundLength {1.0f};
    wxTextCtrl *_textCtrl;
    wxTextCtrl *_soundLengthCtrl;
    wxTextCtrl *_pronounciationCtrl;

    void OnSoundLoadCommand(wxCommandEvent &evt);
    void OnHelpCommmand(wxCommandEvent &evt);
    void OnPronounciationSaveCommand(wxCommandEvent &evt);
    void OnComposeCommand(wxCommandEvent &evt);

    wxDECLARE_EVENT_TABLE();
};

} // namespace reone
