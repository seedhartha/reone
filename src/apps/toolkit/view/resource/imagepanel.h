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

#include <wx/bitmap.h>
#include <wx/panel.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>

namespace reone {

class ImageResourceViewModel;

class ImageResourcePanel : public wxPanel {
public:
    ImageResourcePanel(ImageResourceViewModel &viewModel,
                       wxWindow *parent);

private:
    ImageResourceViewModel &m_viewModel;

    wxSplitterWindow *m_imageSplitter {nullptr};
    wxPanel *m_imageCanvas {nullptr};
    wxTextCtrl *m_imageInfoCtrl {nullptr};
    std::unique_ptr<wxBitmap> m_image;

    void OnImageCanvasPaint(wxPaintEvent &event);
};

} // namespace reone
