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

#include "locstringvaluedialog.h"

namespace reone {

void LocStringFieldValueDialog::InitControls(int strRef, const std::string &substring) {
    auto strRefLabel = new wxStaticText(this, wxID_ANY, "StrRef:");
    m_strRefCtrl = new wxTextCtrl(this, wxID_ANY, std::to_string(strRef));
    m_strRefCtrl->SetMinSize(wxSize {400, m_strRefCtrl->GetMinSize().GetHeight()});

    auto substringLabel = new wxStaticText(this, wxID_ANY, "Substring:");
    m_substringCtrl = new wxTextCtrl(this, wxID_ANY, substring);
    m_substringCtrl->SetMinSize(wxSize {400, m_substringCtrl->GetMinSize().GetHeight()});

    auto okBtn = new wxButton(this, wxID_ANY, "OK");
    okBtn->Bind(wxEVT_BUTTON, [this](const auto &event) { EndModal(wxID_OK); });

    auto cancelBtn = new wxButton(this, wxID_ANY, "Cancel");
    cancelBtn->Bind(wxEVT_BUTTON, [this](const auto &event) { EndModal(wxID_CANCEL); });

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(okBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
    buttonsSizer->Add(cancelBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(strRefLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(m_strRefCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(substringLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(m_substringCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(buttonsSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    SetSizerAndFit(sizer);
}

} // namespace reone
