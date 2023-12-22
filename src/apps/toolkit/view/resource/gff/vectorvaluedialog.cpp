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

#include "vectorvaluedialog.h"

namespace reone {

void VectorFieldValueDialog::InitControls(const glm::vec3 &value) {
    auto xLabel = new wxStaticText(this, wxID_ANY, "X:");
    m_xCtrl = new wxTextCtrl(this, wxID_ANY, std::to_string(value.x));
    m_xCtrl->SetMinSize(wxSize {400, m_xCtrl->GetMinSize().GetHeight()});

    auto yLabel = new wxStaticText(this, wxID_ANY, "Y:");
    m_yCtrl = new wxTextCtrl(this, wxID_ANY, std::to_string(value.y));
    m_yCtrl->SetMinSize(wxSize {400, m_yCtrl->GetMinSize().GetHeight()});

    auto zLabel = new wxStaticText(this, wxID_ANY, "Z:");
    m_zCtrl = new wxTextCtrl(this, wxID_ANY, std::to_string(value.z));
    m_zCtrl->SetMinSize(wxSize {400, m_zCtrl->GetMinSize().GetHeight()});

    auto okBtn = new wxButton(this, wxID_ANY, "OK");
    okBtn->Bind(wxEVT_BUTTON, [this](const auto &event) { EndModal(wxID_OK); });

    auto cancelBtn = new wxButton(this, wxID_ANY, "Cancel");
    cancelBtn->Bind(wxEVT_BUTTON, [this](const auto &event) { EndModal(wxID_CANCEL); });

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(okBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
    buttonsSizer->Add(cancelBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(xLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(m_xCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(yLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(m_yCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(zLabel, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(m_zCtrl, wxSizerFlags(0).Expand().Border(wxALL, 3));
    sizer->Add(buttonsSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    SetSizerAndFit(sizer);
}

} // namespace reone
