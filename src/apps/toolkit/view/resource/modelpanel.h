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

#include <wx/button.h>
#include <wx/glcanvas.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/slider.h>
#include <wx/splitter.h>
#include <wx/textctrl.h>

// Xlib conflicts workaround
#ifdef None
#undef None
#endif
#ifdef Always
#undef Always
#endif

#include "reone/graphics/lipanimation.h"

namespace reone {

class ModelResourceViewModel;

class ModelResourcePanel : public wxPanel {
public:
    ModelResourcePanel(wxWindow *parent);

    void SetViewModel(ModelResourceViewModel &viewModel) {
        _viewModel = viewModel;
    }

    void RefreshGL() {
        _glCanvas->Refresh();
    }

    void OnEngineLoadRequested();

private:
    wxSplitterWindow *_renderSplitter {nullptr};
    wxGLCanvas *_glCanvas {nullptr};
    wxPanel *_animationPanel {nullptr};
    wxButton *_animPauseResumeBtn {nullptr};
    wxSlider *_animTimeSlider {nullptr};
    wxTextCtrl *_animTimeCtrl {nullptr};
    wxListBox *_animationsListBox {nullptr};

    std::optional<std::reference_wrapper<ModelResourceViewModel>> _viewModel;

    std::shared_ptr<graphics::LipAnimation> _lipAnim;

    void OnGLCanvasPaint(wxPaintEvent &event);
    void OnGLCanvasMouseWheel(wxMouseEvent &event);
    void OnGLCanvasMouseMotion(wxMouseEvent &event);

    void OnAnimPauseResumeCommand(wxCommandEvent &event);
    void OnAnimTimeSliderCommand(wxCommandEvent &event);
    void OnAnimationsListBoxDoubleClick(wxCommandEvent &event);
    void OnLipLoadCommand(wxCommandEvent &event);
};

} // namespace reone
