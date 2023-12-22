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

#include <wx/dialog.h>
#include <wx/textctrl.h>

namespace reone {

class VectorFieldValueDialog : public wxDialog {
public:
    VectorFieldValueDialog(wxWindow *parent,
                           const wxString &title,
                           glm::vec3 value = glm::vec3 {0.0f}) :
        wxDialog(parent, wxID_ANY, title) {

        InitControls(value);
    }

    glm::vec3 GetValue() const {
        float x = std::stof(m_xCtrl->GetValue().ToStdString());
        float y = std::stof(m_yCtrl->GetValue().ToStdString());
        float z = std::stof(m_zCtrl->GetValue().ToStdString());
        return glm::vec3(x, y, z);
    }

private:
    wxTextCtrl *m_xCtrl {nullptr};
    wxTextCtrl *m_yCtrl {nullptr};
    wxTextCtrl *m_zCtrl {nullptr};

    void InitControls(const glm::vec3 &value);
};

} // namespace reone
