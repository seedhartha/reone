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
    _viewModel(viewModel) {

    _imageSplitter = new wxSplitterWindow(this, wxID_ANY);
    _imageCanvas = new wxPanel(_imageSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    _imageCanvas->Bind(wxEVT_PAINT, &ImageResourcePanel::OnImageCanvasPaint, this);
    _imageInfoCtrl = new wxTextCtrl(_imageSplitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _imageInfoCtrl->SetEditable(false);
    _imageSplitter->SetMinimumPaneSize(100);
    _imageSplitter->SplitHorizontally(_imageCanvas, _imageInfoCtrl, std::numeric_limits<int>::max());

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(_imageSplitter, wxSizerFlags(1).Expand());
    SetSizer(sizer);

    _viewModel.imageContent().addChangedHandler([this](const auto &data) {
        auto stream = wxMemoryInputStream(&(*data.tgaBytes)[0], data.tgaBytes->size());
        auto image = wxImage();
        image.LoadFile(stream, wxBITMAP_TYPE_TGA);
        _image = std::make_unique<wxBitmap>(image);
        _imageInfoCtrl->Clear();
        _imageInfoCtrl->AppendText(std::string(data.txiBytes->begin(), data.txiBytes->end()));
        if (!data.txiBytes->empty()) {
            _imageSplitter->SplitHorizontally(_imageCanvas, _imageInfoCtrl, std::numeric_limits<int>::max());
        } else {
            _imageSplitter->Unsplit(_imageInfoCtrl);
        }
    });
}

void ImageResourcePanel::OnImageCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(_imageCanvas);

    if (!_image) {
        return;
    }
    int w, h;
    dc.GetSize(&w, &h);
    int x = (w - _image->GetWidth()) / 2;
    int y = (h - _image->GetHeight()) / 2;
    dc.DrawBitmap(*_image, x, y, true);
}

} // namespace reone
