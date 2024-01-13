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

#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/graphics/format/lipwriter.h"
#include "reone/graphics/lipanimation.h"
#include "reone/system/logutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stringbuilder.h"
#include "reone/tools/lip/composer.h"

using namespace reone::audio;
using namespace reone::graphics;

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
    m_soundDurationValidator(4, &m_soundDuration) {

    m_textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_textCtrl->SetMinSize(wxSize(500, 100));
    auto textSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Text");
    textSizer->Add(m_textCtrl, wxSizerFlags(0).Border(wxALL, 3));

    m_soundWaveformPanel = new wxPanel(this, -1);
    m_soundWaveformPanel->SetMinSize(wxSize(500, 100));
    m_soundWaveformPanel->Bind(wxEVT_PAINT, &ComposeLipDialog::OnSoundWavePanelPaint, this);
    auto soundDurationLabel = new wxStaticText(this, wxID_ANY, "Duration (seconds)");
    m_soundDurationCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, m_soundDurationValidator);
    auto soundDurationSizer = new wxBoxSizer(wxHORIZONTAL);
    soundDurationSizer->Add(soundDurationLabel, wxSizerFlags(0).Center().Border(wxALL, 3));
    soundDurationSizer->Add(m_soundDurationCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto minSilenceDurationLabel = new wxStaticText(this, wxID_ANY, "Min silence duration (millis)");
    m_minSilenceDurationSlider = new wxSlider(this, wxID_ANY, 125, 25, 250, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
    m_minSilenceDurationSlider->Bind(wxEVT_SLIDER, &ComposeLipDialog::OnMinSilenceDurationCommand, this);
    auto minSilenceDurationSizer = new wxBoxSizer(wxHORIZONTAL);
    minSilenceDurationSizer->Add(minSilenceDurationLabel, wxSizerFlags(0).Center().Border(wxALL, 3));
    minSilenceDurationSizer->Add(m_minSilenceDurationSlider, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto maxSilenceAmplitudeLabel = new wxStaticText(this, wxID_ANY, "Max silence amplitude (1/1000)");
    m_maxSilenceAmplitudeSlider = new wxSlider(this, wxID_ANY, 25, 5, 50, wxDefaultPosition, wxDefaultSize, wxSL_VALUE_LABEL);
    m_maxSilenceAmplitudeSlider->Bind(wxEVT_SLIDER, &ComposeLipDialog::OnMaxSilenceAmplitudeCommand, this);
    auto maxSilenceAmplitudeSizer = new wxBoxSizer(wxHORIZONTAL);
    maxSilenceAmplitudeSizer->Add(maxSilenceAmplitudeLabel, wxSizerFlags(0).Center().Border(wxALL, 3));
    maxSilenceAmplitudeSizer->Add(m_maxSilenceAmplitudeSlider, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto soundLoadBtn = new wxButton(this, wxID_ANY, "Load...");
    soundLoadBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnSoundLoadCommand, this);
    auto soundResetBtn = new wxButton(this, wxID_ANY, "Reset");
    soundResetBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnSoundResetCommand, this);
    auto soundBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    soundBtnSizer->Add(soundLoadBtn, wxSizerFlags(0).Border(wxALL, 3));
    soundBtnSizer->Add(soundResetBtn, wxSizerFlags(0).Border(wxALL, 3));
    auto soundSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Sound");
    soundSizer->Add(m_soundWaveformPanel, wxSizerFlags(0).Border(wxALL, 3));
    soundSizer->Add(soundDurationSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(minSilenceDurationSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(maxSilenceAmplitudeSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(soundBtnSizer, wxSizerFlags(0).Border(wxALL, 3));

    m_pronounciationCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    m_pronounciationCtrl->SetMinSize(wxSize(500, 0));
    auto pronounciationSaveBtn = new wxButton(this, wxID_ANY, "Save");
    pronounciationSaveBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnPronounciationSaveCommand, this);
    auto pronounciationSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Pronounciation");
    pronounciationSizer->Add(m_pronounciationCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
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
        m_pronounciationCtrl->SetValue(wxString::FromUTF8(buffer));
    }
}

void ComposeLipDialog::analyzeAudio() {
    if (!m_sound) {
        return;
    }
    float minSilenceDuration = m_minSilenceDurationSlider->GetValue() / 1000.0f;
    float maxSilenceAmplitude = m_maxSilenceAmplitudeSlider->GetValue() / 1000.0f;
    AudioAnalyzer audioAnalyzer;
    m_silentSpans = audioAnalyzer.silentSpans(*m_sound, minSilenceDuration, maxSilenceAmplitude);
    int w, h;
    m_soundWaveformPanel->GetClientSize(&w, &h);
    m_soundWaveform = audioAnalyzer.waveform(*m_sound, w);
    m_soundWaveformPanel->Refresh();
}

void ComposeLipDialog::OnSoundWavePanelPaint(wxPaintEvent &evt) {
    wxBufferedPaintDC dc(m_soundWaveformPanel);
    dc.SetBackground(*wxGREY_BRUSH);
    dc.SetPen(*wxWHITE_PEN);
    dc.Clear();

    if (m_soundWaveform.empty()) {
        return;
    }
    int w, h;
    m_soundWaveformPanel->GetSize(&w, &h);
    float prevSample = 0.0f;
    bool prevSilent = false;
    for (int x = 0; x < w; ++x) {
        float waveformTime = (x / static_cast<float>(w)) * m_sound->duration();
        float sample = m_soundWaveform[x];
        bool silent = std::any_of(m_silentSpans.begin(),
                                  m_silentSpans.end(),
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
        m_sound = reader.stream();
    } else if (lowerExt == ".mp3") {
        auto mp3 = FileInputStream(path);
        auto reader = Mp3Reader();
        reader.load(mp3);
        m_sound = reader.stream();
    } else {
        m_sound.reset();
        return;
    }

    analyzeAudio();

    m_soundDuration = m_sound->duration();
    m_soundDurationCtrl->SetValue(str(boost::format("%.04f") % m_soundDuration));
    m_soundDurationCtrl->SetEditable(false);
}

void ComposeLipDialog::OnSoundResetCommand(wxCommandEvent &evt) {
    m_sound.reset();
    m_silentSpans.clear();
    m_soundWaveform.clear();
    m_soundWaveformPanel->Refresh();
    m_soundDuration = 1.0f;
    m_soundDurationCtrl->SetValue(str(boost::format("%.04f") % m_soundDuration));
    m_soundDurationCtrl->SetEditable(true);
    m_minSilenceDurationSlider->SetValue(125);
    m_maxSilenceAmplitudeSlider->SetValue(25);
}

void ComposeLipDialog::OnHelpCommmand(wxCommandEvent &evt) {
    wxMessageBox(kHelpText);
}

void ComposeLipDialog::OnPronounciationSaveCommand(wxCommandEvent &evt) {
    auto pronounciationValue = std::string {m_pronounciationCtrl->GetValue().ToUTF8().data()};
    auto pronouncingPath = std::filesystem::current_path();
    pronouncingPath.append("pronouncing.dict");
    auto pronouncing = FileOutputStream(pronouncingPath);
    pronouncing.write(pronounciationValue.c_str(), pronounciationValue.length());
}

void ComposeLipDialog::OnComposeCommand(wxCommandEvent &evt) {
    auto text = std::string {m_textCtrl->GetValue().ToUTF8().data()};
    if (text.empty()) {
        wxMessageBox("Text is empty", "Error", wxICON_ERROR);
        return;
    }

    if (m_cmudictBytes.empty()) {
        auto cmudictPath = std::filesystem::current_path();
        cmudictPath.append("cmudict.dict");
        if (std::filesystem::exists(cmudictPath)) {
            auto cmudict = FileInputStream(cmudictPath);
            int length = cmudict.length();
            m_cmudictBytes.resize(length);
            cmudict.read(&m_cmudictBytes[0], length);
        }
    }
    auto cmudict = MemoryInputStream(m_cmudictBytes);

    if (m_rudicBytes.empty()) {
        auto rudicPath = std::filesystem::current_path();
        rudicPath.append("ru.dic");
        if (std::filesystem::exists(rudicPath)) {
            auto rudic = FileInputStream(rudicPath);
            int length = rudic.length();
            m_rudicBytes.resize(length);
            rudic.read(&m_rudicBytes[0], length);
        }
    }
    auto rudic = MemoryInputStream(m_rudicBytes);

    auto pronounciationValue = std::string {m_pronounciationCtrl->GetValue().ToUTF8().data()};
    auto pronouncingBuffer = ByteBuffer(pronounciationValue.begin(), pronounciationValue.end());
    auto pronouncing = MemoryInputStream(pronouncingBuffer);

    auto dict = PronouncingDictionary();
    dict.load(cmudict);
    dict.load(rudic);
    dict.load(pronouncing);

    std::unique_ptr<LipAnimation> anim;
    auto composer = LipComposer(dict);
    try {
        anim = composer.compose("composed", text, m_soundDuration, m_silentSpans);
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
        wxMessageBox(message.string(), "Error", wxICON_ERROR);
        return;
    } catch (const WordPhonemesNotFoundException &ex) {
        wxMessageBox(wxString::FromUTF8(ex.what()), "Error", wxICON_ERROR);
        auto pronounciationText = m_pronounciationCtrl->GetValue();
        std::string appendix = ex.word() + " [phonemes]\n";
        if (!pronounciationText.EndsWith("\n")) {
            appendix.append("\n");
        }
        m_pronounciationCtrl->AppendText(wxString::FromUTF8(appendix));
        return;
    } catch (const std::exception &ex) {
        wxMessageBox(wxString::FromUTF8(ex.what()), "Error", wxICON_ERROR);
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

} // namespace reone
