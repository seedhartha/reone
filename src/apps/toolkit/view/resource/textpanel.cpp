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

#include "textpanel.h"

#include "../../viewmodel/resource/text.h"

namespace reone {

TextResourcePanel::TextResourcePanel(TextResourceViewModel &viewModel, wxWindow *parent) :
    wxPanel(parent),
    _viewModel(viewModel) {

    auto textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    textCtrl->AppendText(viewModel.content());
    textCtrl->Bind(wxEVT_TEXT, [this](const auto &event) {
        auto ctrl = wxDynamicCast(event.GetEventObject(), wxTextCtrl);
        auto text = ctrl->GetValue().ToStdString();
        _viewModel.content() = text;
        _viewModel.modified() = true;
    });
    // textCtrl->SetEditable(false);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(textCtrl, 1, wxEXPAND);
    SetSizer(sizer);
}

} // namespace reone
