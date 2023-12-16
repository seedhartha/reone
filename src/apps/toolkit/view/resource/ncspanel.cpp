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

#include "ncspanel.h"

#include "../../viewmodel/resource/ncs.h"

namespace reone {

NCSResourcePanel::NCSResourcePanel(NCSResourceViewModel &viewModel,
                                   wxWindow *parent) :
    wxPanel(parent),
    _viewModel(viewModel) {

    auto textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    textCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    textCtrl->AppendText(viewModel.content());
    textCtrl->SetEditable(false);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(textCtrl, 1, wxEXPAND);
    SetSizer(sizer);
}

} // namespace reone
