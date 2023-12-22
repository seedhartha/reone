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

#include "imagepanel.h"

#include <wx/mstream.h>

#include "../../viewmodel/resource/image.h"

namespace reone {

ImageResourcePanel::ImageResourcePanel(ImageResourceViewModel &viewModel,
                                       wxWindow *parent) :
    wxPanel(parent),
    m_viewModel(viewModel) {

    InitControls();
    BindEvents();
    BindViewModel();
}

void ImageResourcePanel::InitControls() {
    m_imageSplitter = new wxSplitterWindow(this, wxID_ANY);
    m_imageSplitter->SetMinimumPaneSize(100);

    m_imageCanvas = new wxPanel(m_imageSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);

    m_imageInfoCtrl = new wxTextCtrl(m_imageSplitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_imageInfoCtrl->SetEditable(false);

    m_imageSplitter->SplitHorizontally(m_imageCanvas, m_imageInfoCtrl, std::numeric_limits<int>::max());

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_imageSplitter, wxSizerFlags(1).Expand());
    SetSizer(sizer);
}

void ImageResourcePanel::BindEvents() {
    m_imageCanvas->Bind(wxEVT_PAINT, &ImageResourcePanel::OnImageCanvasPaint, this);
}

void ImageResourcePanel::BindViewModel() {
    m_viewModel.imageContent().addChangedHandler([this](const auto &data) {
        auto stream = wxMemoryInputStream(&(*data.tgaBytes)[0], data.tgaBytes->size());
        auto image = wxImage();
        image.LoadFile(stream, wxBITMAP_TYPE_TGA);
        m_image = std::make_unique<wxBitmap>(image);
        m_imageInfoCtrl->Clear();
        m_imageInfoCtrl->AppendText(std::string(data.txiBytes->begin(), data.txiBytes->end()));
        if (!data.txiBytes->empty()) {
            m_imageSplitter->SplitHorizontally(m_imageCanvas, m_imageInfoCtrl, std::numeric_limits<int>::max());
        } else {
            m_imageSplitter->Unsplit(m_imageInfoCtrl);
        }
    });
}

void ImageResourcePanel::OnImageCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(m_imageCanvas);

    if (!m_image) {
        return;
    }
    int w, h;
    dc.GetSize(&w, &h);
    int x = (w - m_image->GetWidth()) / 2;
    int y = (h - m_image->GetHeight()) / 2;
    dc.DrawBitmap(*m_image, x, y, true);
}

} // namespace reone
