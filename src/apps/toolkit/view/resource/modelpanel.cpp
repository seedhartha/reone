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

ModelResourcePanel::ModelResourcePanel(ModelResourceViewModel &viewModel,
                                       wxWindow *parent) :
    wxPanel(parent),
    m_viewModel(viewModel) {

    InitControls();
    BindEvents();
    BindViewModel();
}

void ModelResourcePanel::InitControls() {
    m_renderSplitter = new wxSplitterWindow(this);
    m_renderSplitter->SetMinimumPaneSize(100);

    m_glCanvas = new wxGLCanvas(m_renderSplitter, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);

#if wxCHECK_VERSION(3, 1, 0)
    wxGLContextAttrs glCtxAttrs;
    glCtxAttrs.CoreProfile().OGLVersion(3, 3).EndList();
    auto glContext = new wxGLContext(m_glCanvas, nullptr, &glCtxAttrs);
#else
    auto glContext = new wxGLContext(m_glCanvas);
#endif
    glContext->SetCurrent(*m_glCanvas);

    m_animationPanel = new wxPanel(m_renderSplitter);

    m_animPauseResumeBtn = new wxButton(m_animationPanel, wxID_ANY, "Pause");
    m_animPauseResumeBtn->Enable(false);

    m_animTimeSlider = new wxSlider(m_animationPanel, wxID_ANY, 0, 0, 500, wxDefaultPosition, wxDefaultSize);
    m_animTimeSlider->Enable(false);

    m_animTimeCtrl = new wxTextCtrl(m_animationPanel, wxID_ANY, "0.0", wxDefaultPosition, wxDefaultSize, wxTE_READONLY);

    auto animPlaybackSizer = new wxBoxSizer(wxHORIZONTAL);
    animPlaybackSizer->Add(m_animPauseResumeBtn, wxSizerFlags(0).Center().Border(wxALL, 3));
    animPlaybackSizer->Add(m_animTimeSlider, wxSizerFlags(1).Expand().Border(wxALL, 3));
    animPlaybackSizer->Add(m_animTimeCtrl, wxSizerFlags(0).Center().Border(wxALL, 3));

    m_animationsListBox = new wxListBox(m_animationPanel, wxID_ANY);
    m_animationsListBox->SetMinSize(wxSize(400, 100));

    auto animationsSizer = new wxStaticBoxSizer(wxVERTICAL, m_animationPanel, "Animations");
    animationsSizer->Add(m_animationsListBox, wxSizerFlags(1).Expand().Border(wxALL, 3));

    m_lipLoadBtn = new wxButton(m_animationPanel, wxID_ANY, "Load LIP...");

    auto lipSyncSizer = new wxStaticBoxSizer(wxVERTICAL, m_animationPanel, "Lip Sync");
    lipSyncSizer->Add(m_lipLoadBtn, wxSizerFlags(0).Expand().Border(wxALL, 3));

    auto animationHSizer = new wxBoxSizer(wxHORIZONTAL);
    animationHSizer->Add(animationsSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));
    animationHSizer->Add(lipSyncSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));

    auto animationVSizer = new wxBoxSizer(wxVERTICAL);
    animationVSizer->Add(animPlaybackSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    animationVSizer->Add(animationHSizer, wxSizerFlags(1).Expand().Border(wxALL, 3));
    m_animationPanel->SetSizer(animationVSizer);

    m_renderSplitter->SplitHorizontally(m_glCanvas, m_animationPanel, -200);

    auto sizer = new wxBoxSizer(wxHORIZONTAL);
    sizer->Add(m_renderSplitter, wxSizerFlags(1).Expand());
    SetSizer(sizer);
}

void ModelResourcePanel::BindEvents() {
    m_glCanvas->Bind(wxEVT_PAINT, &ModelResourcePanel::OnGLCanvasPaint, this);
    m_glCanvas->Bind(wxEVT_MOTION, &ModelResourcePanel::OnGLCanvasMouseMotion, this);
    m_glCanvas->Bind(wxEVT_MOUSEWHEEL, &ModelResourcePanel::OnGLCanvasMouseWheel, this);
    m_animPauseResumeBtn->Bind(wxEVT_BUTTON, &ModelResourcePanel::OnAnimPauseResumeCommand, this);
    m_animTimeSlider->Bind(wxEVT_SLIDER, &ModelResourcePanel::OnAnimTimeSliderCommand, this);
    m_animationsListBox->Bind(wxEVT_LISTBOX_DCLICK, &ModelResourcePanel::OnAnimationsListBoxDoubleClick, this);
    m_lipLoadBtn->Bind(wxEVT_BUTTON, &ModelResourcePanel::OnLipLoadCommand, this);
}

void ModelResourcePanel::BindViewModel() {
    m_viewModel.animations().addChangedHandler([this](const auto &animations) {
        if (!animations.empty()) {
            m_animationsListBox->Freeze();
            m_animationsListBox->Clear();
            for (auto &animation : animations) {
                m_animationsListBox->Append(animation);
            }
            m_animationsListBox->Thaw();
            m_renderSplitter->SplitHorizontally(m_glCanvas, m_animationPanel, -200);
        } else {
            m_renderSplitter->Unsplit();
        }
    });
    m_viewModel.animationProgress().addChangedHandler([this](const auto &progress) {
        m_animPauseResumeBtn->Enable(progress.playing);
        int value = static_cast<int>(m_animTimeSlider->GetMax() * (progress.time / progress.duration));
        m_animTimeSlider->SetValue(value);
        m_animTimeSlider->Enable(progress.playing);
        m_animTimeCtrl->SetValue(str(boost::format("%.04f") % progress.time));
    });
}

void ModelResourcePanel::OnGLCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(m_glCanvas);

    auto clientSize = m_glCanvas->GetClientSize();
    m_viewModel.render3D(clientSize.x, clientSize.y);

    m_glCanvas->SwapBuffers();
}

void ModelResourcePanel::OnGLCanvasMouseWheel(wxMouseEvent &event) {
    auto delta = event.GetWheelDelta() * event.GetWheelRotation();
    m_viewModel.onGLCanvasMouseWheel(delta);
}

void ModelResourcePanel::OnGLCanvasMouseMotion(wxMouseEvent &event) {
    wxClientDC dc(m_glCanvas);
    auto position = event.GetLogicalPosition(dc);
    m_viewModel.onGLCanvasMouseMotion(position.x, position.y, event.LeftIsDown(), event.RightIsDown());
}

void ModelResourcePanel::OnAnimPauseResumeCommand(wxCommandEvent &event) {
    if (m_viewModel.isAnimationPlaying()) {
        m_viewModel.pauseAnimation();
        m_animPauseResumeBtn->SetLabelText("Resume");
    } else {
        m_viewModel.resumeAnimation();
        m_animPauseResumeBtn->SetLabelText("Pause");
    }
}

void ModelResourcePanel::OnAnimTimeSliderCommand(wxCommandEvent &event) {
    float duration = m_viewModel.animationProgress()->duration;
    if (duration == 0.0f) {
        return;
    }
    float time = duration * m_animTimeSlider->GetValue() / static_cast<float>(m_animTimeSlider->GetMax());
    m_viewModel.setAnimationTime(time);
}

void ModelResourcePanel::OnAnimationsListBoxDoubleClick(wxCommandEvent &event) {
    int selection = event.GetSelection();
    if (selection == -1) {
        return;
    }
    auto animation = m_animationsListBox->GetString(selection);
    m_viewModel.playAnimation(animation.ToStdString());
    m_animPauseResumeBtn->SetLabelText("Pause");
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
    m_lipAnim = reader.animation();
    m_viewModel.playAnimation("talk", m_lipAnim.get());
}

} // namespace reone
