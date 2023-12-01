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

#include "composelipdialog.h"

#include <wx/dcbuffer.h>

#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/lipwriter.h"
#include "reone/resource/format/mp3reader.h"
#include "reone/resource/format/wavreader.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"
#include "reone/tools/lip/composer.h"


using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

static const char *kHelpText = R"END(Phoneme Example Translation
------- ------- -----------
AA      odd     AA D
AE      at      AE T
AH      hut     HH AH T
AO      ought   AO T
AW      cow     K AW
AY      hide    HH AY D
B       be      B IY
CH      cheese  CH IY Z
D       dee     D IY
DH      thee    DH IY
EH      Ed      EH D
ER      hurt    HH ER T
EY      ate     EY T
F       fee     F IY
G       green   G R IY N
HH      he      HH IY
IH      it      IH T
IY      eat     IY T
JH      gee     JH IY
K       key     K IY
L       lee     L IY
M       me      M IY
N       knee    N IY
NG      ping    P IH NG
OW      oat     OW T
OY      toy     T OY
P       pee     P IY
R       read    R IY D
S       sea     S IY
SH      she     SH IY
T       tea     T IY
TH      theta   TH EY T AH
UH      hood    HH UH D
UW      two     T UW
V       vee     V IY
W       we      W IY
Y       yield   Y IY L D
Z       zee     Z IY
ZH      seizure S IY ZH ER)END";

ComposeLipDialog::ComposeLipDialog(wxWindow *parent,
                                   wxWindowID id,
                                   const wxString &title,
                                   const wxPoint &pos,
                                   const wxSize &size,
                                   long style) :
    wxDialog(parent, id, title, pos, size, style),
    _soundDurationValidator(4, &_soundDuration) {

    _textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _textCtrl->SetMinSize(wxSize(500, 100));
    auto textSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Text");
    textSizer->Add(_textCtrl, wxSizerFlags(0).Border(wxALL, 3));

    _soundWaveformPanel = new wxPanel(this, -1);
    _soundWaveformPanel->SetMinSize(wxSize(500, 100));
    _soundWaveformPanel->Bind(wxEVT_PAINT, &ComposeLipDialog::OnSoundWavePanelPaint, this);
    auto soundDurationLabel = new wxStaticText(this, wxID_ANY, "Duration (seconds)");
    _soundDurationCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _soundDurationValidator);
    auto soundDurationSizer = new wxBoxSizer(wxHORIZONTAL);
    soundDurationSizer->Add(soundDurationLabel, wxSizerFlags(0).Center().Border(wxALL, 3));
    soundDurationSizer->Add(_soundDurationCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto minSilenceDurationLabel = new wxStaticText(this, wxID_ANY, "Min silence duration (millis)");
    _minSilenceDurationSlider = new wxSlider(this, wxID_ANY, 125, 25, 250, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
    _minSilenceDurationSlider->Bind(wxEVT_SLIDER, &ComposeLipDialog::OnMinSilenceDurationCommand, this);
    auto minSilenceDurationSizer = new wxBoxSizer(wxHORIZONTAL);
    minSilenceDurationSizer->Add(minSilenceDurationLabel, wxSizerFlags(0).Center().Border(wxALL, 3));
    minSilenceDurationSizer->Add(_minSilenceDurationSlider, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto maxSilenceAmplitudeLabel = new wxStaticText(this, wxID_ANY, "Max silence amplitude (1/1000)");
    _maxSilenceAmplitudeSlider = new wxSlider(this, wxID_ANY, 25, 5, 50, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
    _maxSilenceAmplitudeSlider->Bind(wxEVT_SLIDER, &ComposeLipDialog::OnMaxSilenceAmplitudeCommand, this);
    auto maxSilenceAmplitudeSizer = new wxBoxSizer(wxHORIZONTAL);
    maxSilenceAmplitudeSizer->Add(maxSilenceAmplitudeLabel, wxSizerFlags(0).Center().Border(wxALL, 3));
    maxSilenceAmplitudeSizer->Add(_maxSilenceAmplitudeSlider, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto soundLoadBtn = new wxButton(this, wxID_ANY, "Load...");
    soundLoadBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnSoundLoadCommand, this);
    auto soundResetBtn = new wxButton(this, wxID_ANY, "Reset");
    soundResetBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnSoundResetCommand, this);
    auto soundBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    soundBtnSizer->Add(soundLoadBtn, wxSizerFlags(0).Border(wxALL, 3));
    soundBtnSizer->Add(soundResetBtn, wxSizerFlags(0).Border(wxALL, 3));
    auto soundSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Sound");
    soundSizer->Add(_soundWaveformPanel, wxSizerFlags(0).Border(wxALL, 3));
    soundSizer->Add(soundDurationSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(minSilenceDurationSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(maxSilenceAmplitudeSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(soundBtnSizer, wxSizerFlags(0).Border(wxALL, 3));

    _pronounciationCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _pronounciationCtrl->SetMinSize(wxSize(500, 0));
    auto pronounciationSaveBtn = new wxButton(this, wxID_ANY, "Save");
    pronounciationSaveBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnPronounciationSaveCommand, this);
    auto pronounciationSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Pronounciation");
    pronounciationSizer->Add(_pronounciationCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    pronounciationSizer->Add(pronounciationSaveBtn, wxSizerFlags(0).Border(wxALL, 3));

    auto helpBtn = new wxButton(this, wxID_ANY, "Help");
    helpBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnHelpCommmand, this);

    auto composeBtn = new wxButton(this, wxID_ANY, "Compose");
    composeBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnComposeCommand, this);

    auto buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonsSizer->Add(helpBtn, wxSizerFlags(0).Border(wxALL, 3));
    buttonsSizer->Add(composeBtn, wxSizerFlags(0).Border(wxALL, 3));

    auto composeSizer = new wxBoxSizer(wxVERTICAL);
    composeSizer->Add(textSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    composeSizer->Add(soundSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    composeSizer->Add(buttonsSizer, wxSizerFlags(0).Border(wxALL, 3));

    auto topSizer = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(composeSizer, wxSizerFlags(0).Border(wxALL, 3));
    topSizer->Add(pronounciationSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    SetSizerAndFit(topSizer);

    auto pronouncingDictPath = std::filesystem::current_path();
    pronouncingDictPath.append("pronouncing.dict");
    if (std::filesystem::exists(pronouncingDictPath)) {
        auto pronouncingDict = FileInputStream(pronouncingDictPath);
        auto length = pronouncingDict.length();
        auto buffer = std::string(length, '\0');
        pronouncingDict.read(&buffer[0], length);
        _pronounciationCtrl->SetValue(buffer);
    }
}

void ComposeLipDialog::analyzeAudio() {
    if (!_sound) {
        return;
    }
    float minSilenceDuration = _minSilenceDurationSlider->GetValue() / 1000.0f;
    float maxSilenceAmplitude = _maxSilenceAmplitudeSlider->GetValue() / 1000.0f;
    AudioAnalyzer audioAnalyzer;
    _silentSpans = audioAnalyzer.silentSpans(*_sound, minSilenceDuration, maxSilenceAmplitude);
    int w, h;
    _soundWaveformPanel->GetClientSize(&w, &h);
    _soundWaveform = audioAnalyzer.waveform(*_sound, w);
    _soundWaveformPanel->Refresh();
}

void ComposeLipDialog::OnSoundWavePanelPaint(wxPaintEvent &evt) {
    wxBufferedPaintDC dc(_soundWaveformPanel);
    dc.SetBackground(*wxGREY_BRUSH);
    dc.SetPen(*wxWHITE_PEN);
    dc.Clear();

    if (_soundWaveform.empty()) {
        return;
    }
    int w, h;
    _soundWaveformPanel->GetSize(&w, &h);
    float prevSample = 0.0f;
    bool prevSilent = false;
    for (int x = 0; x < w; ++x) {
        float waveformTime = (x / static_cast<float>(w)) * _sound->duration();
        float sample = _soundWaveform[x];
        bool silent = std::any_of(_silentSpans.begin(),
                                  _silentSpans.end(),
                                  [&waveformTime](const auto &span) { return span.contains(waveformTime); });
        if (silent && prevSilent) {
            dc.SetPen(*wxBLACK_PEN);
        } else {
            dc.SetPen(*wxWHITE_PEN);
        }
        if (x > 0) {
            dc.DrawLine(
                x - 1,
                h / 2 + static_cast<int>(prevSample * h),
                x,
                h / 2 + static_cast<int>(sample * h));
        }
        prevSample = sample;
        prevSilent = silent;
    }
}

void ComposeLipDialog::OnMinSilenceDurationCommand(wxCommandEvent &evt) {
    analyzeAudio();
}

void ComposeLipDialog::OnMaxSilenceAmplitudeCommand(wxCommandEvent &evt) {
    analyzeAudio();
}

void ComposeLipDialog::OnSoundLoadCommand(wxCommandEvent &evt) {
    auto dialog = wxFileDialog(
        this,
        "Choose audio file",
        wxEmptyString,
        wxEmptyString,
        "*.wav;*.mp3",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (dialog.ShowModal() != wxID_OK) {
        return;
    }
    auto path = std::filesystem::path(dialog.GetPath().ToStdString());

    auto lowerExt = boost::to_lower_copy(path.extension().string());
    if (lowerExt == ".wav") {
        auto wav = FileInputStream(path);
        auto mp3ReaderFactory = Mp3ReaderFactory();
        auto reader = WavReader(wav, mp3ReaderFactory);
        reader.load();
        _sound = reader.stream();
    } else if (lowerExt == ".mp3") {
        auto mp3 = FileInputStream(path);
        auto reader = Mp3Reader();
        reader.load(mp3);
        _sound = reader.stream();
    } else {
        _sound.reset();
        return;
    }

    analyzeAudio();

    _soundDuration = _sound->duration();
    _soundDurationCtrl->SetValue(str(boost::format("%.04f") % _soundDuration));
    _soundDurationCtrl->SetEditable(false);
}

void ComposeLipDialog::OnSoundResetCommand(wxCommandEvent &evt) {
    _sound.reset();
    _silentSpans.clear();
    _soundWaveform.clear();
    _soundWaveformPanel->Refresh();
    _soundDuration = 1.0f;
    _soundDurationCtrl->SetValue(str(boost::format("%.04f") % _soundDuration));
    _soundDurationCtrl->SetEditable(true);
    _minSilenceDurationSlider->SetValue(125);
    _maxSilenceAmplitudeSlider->SetValue(25);
}

void ComposeLipDialog::OnHelpCommmand(wxCommandEvent &evt) {
    wxMessageBox(kHelpText);
}

void ComposeLipDialog::OnPronounciationSaveCommand(wxCommandEvent &evt) {
    auto pronounciationValue = _pronounciationCtrl->GetValue();
    auto pronouncingPath = std::filesystem::current_path();
    pronouncingPath.append("pronouncing.dict");
    auto pronouncing = FileOutputStream(pronouncingPath);
    pronouncing.write(pronounciationValue.c_str(), pronounciationValue.length());
}

void ComposeLipDialog::OnComposeCommand(wxCommandEvent &evt) {
    auto text = _textCtrl->GetValue().ToStdString();
    if (text.empty()) {
        return;
    }

    if (_cmudictBytes.empty()) {
        auto cmudictPath = std::filesystem::current_path();
        cmudictPath.append("cmudict.dict");
        if (std::filesystem::exists(cmudictPath)) {
            auto cmudict = FileInputStream(cmudictPath);
            int length = cmudict.length();
            _cmudictBytes.resize(length);
            cmudict.read(&_cmudictBytes[0], length);
        }
    }
    auto cmudict = MemoryInputStream(_cmudictBytes);

    auto pronounciationValue = _pronounciationCtrl->GetValue();
    auto pronouncingBuffer = ByteBuffer(pronounciationValue.begin(), pronounciationValue.end());
    auto pronouncing = MemoryInputStream(pronouncingBuffer);

    auto dict = PronouncingDictionary();
    dict.load(cmudict);
    dict.load(pronouncing);

    std::unique_ptr<LipAnimation> anim;
    auto composer = LipComposer(dict);
    try {
        anim = composer.compose("composed", text, _soundDuration, _silentSpans);
    } catch (const WordGroupsSoundSpansMismatchedException &ex) {
        std::vector<std::string> spans;
        for (const auto &span : ex.soundSpans()) {
            spans.push_back(span.string());
        }
        auto spansStr = boost::join(spans, ", ");
        StringBuilder message;
        message.append("Mismatch between word groups and sound spans.\n");
        message.append(str(boost::format("Expected %llu word groups at the following time spans: %s\n") % spans.size() % spansStr));
        message.append("Wrap words into word groups with parentheses.");
        wxMessageBox(message.string());
        return;
    } catch (const WordPhonemesNotFoundException &ex) {
        wxMessageBox(ex.what());
        auto pronounciationText = _pronounciationCtrl->GetValue();
        _pronounciationCtrl->AppendText(ex.word() + " [phonemes]\n");
        return;
    } catch (const std::exception &ex) {
        wxMessageBox(ex.what());
        return;
    }
    auto destFileDialog = wxFileDialog(
        this,
        "Choose destination file",
        wxEmptyString,
        "composed.lip",
        "*.lip",
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (destFileDialog.ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path(destFileDialog.GetPath().ToStdString());
    auto writer = LipWriter(*anim);
    writer.save(destPath);
}

wxBEGIN_EVENT_TABLE(ComposeLipDialog, wxDialog)
    wxEND_EVENT_TABLE()

} // namespace reone
