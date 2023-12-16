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

#include "audiopanel.h"

#include "../../viewmodel/resource/audio.h"

using namespace reone::audio;

namespace reone {

AudioResourcePanel::AudioResourcePanel(wxWindow *parent) :
    wxPanel(parent) {

    auto stopAudioButton = new wxButton(this, wxID_ANY, "Stop");
    stopAudioButton->Bind(wxEVT_BUTTON, &AudioResourcePanel::OnStopAudioCommand, this);

    auto sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(stopAudioButton);
    SetSizer(sizer);
}

void AudioResourcePanel::OnEngineLoadRequested() {
    _viewModel->get().audioStream().addChangedHandler([this](const auto &stream) {
        if (stream) {
            _audioSource = std::make_unique<AudioSource>(stream, false, 1.0f, false, glm::vec3());
            _audioSource->init();
            _audioSource->play();
            wxWakeUpIdle();
        } else {
            _audioSource.reset();
        }
    });
}

void AudioResourcePanel::OnStopAudioCommand(wxCommandEvent &event) {
    if (_audioSource) {
        _audioSource->stop();
        _audioSource.reset();
    }
}

} // namespace reone
