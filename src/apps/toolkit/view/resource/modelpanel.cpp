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

#include "modelpanel.h"

#include "reone/graphics/format/lipreader.h"
#include "reone/system/stream/fileinput.h"

#include "../../viewmodel/resource/model.h"

using namespace reone::graphics;

namespace reone {

ModelResourcePanel::ModelResourcePanel(wxWindow *parent) :
    wxPanel(parent) {

    _renderSplitter = new wxSplitterWindow(this);
    _renderSplitter->SetMinimumPaneSize(100);

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(_renderSplitter, wxSizerFlags(1).Expand());
    SetSizer(sizer);
}

void ModelResourcePanel::OnEngineLoadRequested() {
    _glCanvas = new wxGLCanvas(_renderSplitter, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    _glCanvas->Bind(wxEVT_PAINT, &ModelResourcePanel::OnGLCanvasPaint, this);
    _glCanvas->Bind(wxEVT_MOTION, &ModelResourcePanel::OnGLCanvasMouseMotion, this);
    _glCanvas->Bind(wxEVT_MOUSEWHEEL, &ModelResourcePanel::OnGLCanvasMouseWheel, this);

#if wxCHECK_VERSION(3, 1, 0)
    wxGLContextAttrs glCtxAttrs;
    glCtxAttrs.CoreProfile().OGLVersion(3, 3).EndList();
    auto glContext = new wxGLContext(_glCanvas, nullptr, &glCtxAttrs);
#else
    auto glContext = new wxGLContext(_glCanvas);
#endif
    glContext->SetCurrent(*_glCanvas);

    _animationPanel = new wxPanel(_renderSplitter);
    _animPauseResumeBtn = new wxButton(_animationPanel, wxID_ANY, "Pause");
    _animPauseResumeBtn->Bind(wxEVT_BUTTON, &ModelResourcePanel::OnAnimPauseResumeCommand, this);
    _animTimeSlider = new wxSlider(_animationPanel, wxID_ANY, 0, 0, 500, wxDefaultPosition, wxDefaultSize);
    _animTimeSlider->Bind(wxEVT_SLIDER, &ModelResourcePanel::OnAnimTimeSliderCommand, this);
    _animTimeCtrl = new wxTextCtrl(_animationPanel, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
    auto animPlaybackSizer = new wxBoxSizer(wxHORIZONTAL);
    animPlaybackSizer->Add(_animPauseResumeBtn, wxSizerFlags(0).Center().Border(wxALL, 3));
    animPlaybackSizer->Add(_animTimeSlider, wxSizerFlags(1).Expand().Border(wxALL, 3));
    animPlaybackSizer->Add(_animTimeCtrl, wxSizerFlags(0).Center().Border(wxALL, 3));
    _animationsListBox = new wxListBox(_animationPanel, wxID_ANY);
    _animationsListBox->SetMinSize(wxSize(400, 100));
    _animationsListBox->Bind(wxEVT_LISTBOX_DCLICK, &ModelResourcePanel::OnAnimationsListBoxDoubleClick, this);
    auto animationsSizer = new wxStaticBoxSizer(wxVERTICAL, _animationPanel, "Animations");
    animationsSizer->Add(_animationsListBox, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto lipLoadBtn = new wxButton(_animationPanel, wxID_ANY, "Load LIP...");
    lipLoadBtn->Bind(wxEVT_BUTTON, &ModelResourcePanel::OnLipLoadCommand, this);
    auto lipSyncSizer = new wxStaticBoxSizer(wxVERTICAL, _animationPanel, "Lip Sync");
    lipSyncSizer->Add(lipLoadBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));
    auto animationHSizer = new wxBoxSizer(wxHORIZONTAL);
    animationHSizer->Add(animationsSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));
    animationHSizer->Add(lipSyncSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    auto animationVSizer = new wxBoxSizer(wxVERTICAL);
    animationVSizer->Add(animPlaybackSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    animationVSizer->Add(animationHSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));
    _animationPanel->SetSizer(animationVSizer);

    _renderSplitter->SplitHorizontally(_glCanvas, _animationPanel, std::numeric_limits<int>::max());

    _viewModel->get().animations().addChangedHandler([this](const auto &animations) {
        if (!animations.empty()) {
            _animationsListBox->Freeze();
            _animationsListBox->Clear();
            for (auto &animation : animations) {
                _animationsListBox->Append(animation);
            }
            _animationsListBox->Thaw();
            _renderSplitter->SplitHorizontally(_glCanvas, _animationPanel, std::numeric_limits<int>::max());
        } else {
            _renderSplitter->Unsplit();
        }
    });
    _viewModel->get().animationProgress().addChangedHandler([this](const auto &progress) {
        _animTimeCtrl->SetValue(str(boost::format("%.04f") % progress.time));
        int value = static_cast<int>(_animTimeSlider->GetMax() * (progress.time / progress.duration));
        _animTimeSlider->SetValue(value);
    });
}

void ModelResourcePanel::OnGLCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(_glCanvas);

    auto clientSize = _glCanvas->GetClientSize();
    _viewModel->get().render3D(clientSize.x, clientSize.y);

    _glCanvas->SwapBuffers();
}

void ModelResourcePanel::OnGLCanvasMouseWheel(wxMouseEvent &event) {
    auto delta = event.GetWheelDelta() * event.GetWheelRotation();
    _viewModel->get().onGLCanvasMouseWheel(delta);
}

void ModelResourcePanel::OnGLCanvasMouseMotion(wxMouseEvent &event) {
    wxClientDC dc(_glCanvas);
    auto position = event.GetLogicalPosition(dc);
    _viewModel->get().onGLCanvasMouseMotion(position.x, position.y, event.LeftIsDown(), event.RightIsDown());
}

void ModelResourcePanel::OnAnimPauseResumeCommand(wxCommandEvent &event) {
    if (_viewModel->get().isAnimationPlaying()) {
        _viewModel->get().pauseAnimation();
        _animPauseResumeBtn->SetLabelText("Resume");
    } else {
        _viewModel->get().resumeAnimation();
        _animPauseResumeBtn->SetLabelText("Pause");
    }
}

void ModelResourcePanel::OnAnimTimeSliderCommand(wxCommandEvent &event) {
    float duration = _viewModel->get().animationProgress()->duration;
    if (duration == 0.0f) {
        return;
    }
    float time = duration * _animTimeSlider->GetValue() / static_cast<float>(_animTimeSlider->GetMax());
    _viewModel->get().setAnimationTime(time);
}

void ModelResourcePanel::OnAnimationsListBoxDoubleClick(wxCommandEvent &event) {
    int selection = event.GetSelection();
    if (selection == -1) {
        return;
    }
    auto animation = _animationsListBox->GetString(selection);
    _viewModel->get().playAnimation(animation.ToStdString());
    _animPauseResumeBtn->SetLabelText("Pause");
}

void ModelResourcePanel::OnLipLoadCommand(wxCommandEvent &event) {
    auto dialog = wxFileDialog(
        this,
        "Choose LIP file",
        wxEmptyString,
        wxEmptyString,
        "*.lip",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() != wxID_OK) {
        return;
    }
    auto path = std::filesystem::path(dialog.GetPath().ToStdString());
    auto lip = FileInputStream(path);
    auto reader = LipReader(lip, "");
    reader.load();
    _lipAnim = reader.animation();
    _viewModel->get().playAnimation("talk", _lipAnim.get());
}

} // namespace reone
