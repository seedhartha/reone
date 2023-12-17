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

#include "ncspanel.h"

#include "reone/game/script/routines.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/tools/script/format/pcodereader.h"

#include "../../viewmodel/resource/ncs.h"

using namespace reone::game;
using namespace reone::resource;
using namespace reone::script;

namespace reone {

NCSResourcePanel::NCSResourcePanel(GameID gameId,
                                   NCSResourceViewModel &viewModel,
                                   wxWindow *parent) :
    wxPanel(parent),
    m_gameId(gameId),
    m_viewModel(viewModel) {

    auto textCtrl = new wxTextCtrl(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    textCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    textCtrl->AppendText(viewModel.content());
    textCtrl->Bind(wxEVT_TEXT, [this](const auto &event) {
        auto ctrl = wxDynamicCast(event.GetEventObject(), wxTextCtrl);
        auto text = ctrl->GetValue().ToStdString();
        m_viewModel.content() = text;
        m_viewModel.modified() = true;
    });
    // textCtrl->SetEditable(false);

    auto compileCtrl = new wxButton(this, wxID_ANY, "Compile");
    compileCtrl->Bind(wxEVT_BUTTON, [this](const auto &event) {
        MemoryInputStream stream {m_viewModel.content()};
        Routines routines {m_gameId, nullptr, nullptr};
        routines.init();
        PcodeReader reader {"", stream, routines};
        try {
            reader.load();
            auto program = reader.program();
        } catch (const std::exception &ex) {
            wxMessageBox(ex.what(), "Error", wxICON_ERROR);
        }
    });

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(compileCtrl, wxSizerFlags(0).Border(wxALL, 3));
    sizer->Add(textCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    SetSizer(sizer);
}

} // namespace reone
