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

#include "nsspanel.h"

#include <wx/stc/stc.h>

#include "../../viewmodel/resource/nss.h"

namespace reone {

NSSResourcePanel::NSSResourcePanel(NSSResourceViewModel &viewModel,
                                   wxWindow *parent) :
    wxPanel(parent),
    _viewModel(viewModel) {

    auto textCtrl = new wxStyledTextCtrl(this);
    textCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    textCtrl->SetLexer(wxSTC_LEX_CPP);
    textCtrl->SetKeyWords(0, "break case continue default do else for if return switch while");
    textCtrl->SetKeyWords(1, "action command const effect event float int itemproperty location object std::string struct talent vector void");
    textCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(128, 64, 0));
    textCtrl->StyleSetForeground(wxSTC_C_DEFAULT, wxColour(0, 0, 0));
    textCtrl->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
    textCtrl->StyleSetForeground(wxSTC_C_WORD2, wxColour(128, 0, 255));
    textCtrl->StyleSetForeground(wxSTC_C_NUMBER, wxColour(255, 128, 0));
    textCtrl->StyleSetForeground(wxSTC_C_STRING, wxColour(128, 128, 128));
    textCtrl->StyleSetForeground(wxSTC_C_CHARACTER, wxColour(128, 128, 128));
    textCtrl->StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 128));
    textCtrl->StyleSetForeground(wxSTC_C_VERBATIM, wxColour(0, 0, 0));
    textCtrl->StyleSetForeground(wxSTC_C_REGEX, wxColour(0, 0, 0));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 128));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTLINEDOC, wxColour(0, 128, 128));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD, wxColour(0, 128, 128));
    textCtrl->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0, 128, 128));
    textCtrl->StyleSetForeground(wxSTC_C_PREPROCESSORCOMMENT, wxColour(0, 128, 0));
    textCtrl->SetText(viewModel.content());
    textCtrl->SetEditable(false);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(textCtrl, 1, wxEXPAND);
    SetSizer(sizer);
}

} // namespace reone
