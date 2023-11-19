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

#include "reone/audio/buffer.h"
#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/graphics/format/lipwriter.h"
#include "reone/graphics/lipanimation.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/tools/lipcomposer.h"

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
    _soundLengthValidator(4, &_soundLength) {

    _textCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _textCtrl->SetMinSize(wxSize(400, 100));
    auto textSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Text");
    textSizer->Add(_textCtrl, wxSizerFlags(0).Border(wxALL, 3));

    auto soundLengthLabel = new wxStaticText(this, wxID_ANY, "Length (seconds)");
    _soundLengthCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, _soundLengthValidator);
    auto soundLengthSizer = new wxBoxSizer(wxHORIZONTAL);
    soundLengthSizer->Add(soundLengthLabel, wxSizerFlags(0).Border(wxALL, 3));
    soundLengthSizer->Add(_soundLengthCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3));
    auto soundLoadBtn = new wxButton(this, wxID_ANY, "Load...");
    soundLoadBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnSoundLoadCommand, this);
    auto soundSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Sound");
    soundSizer->Add(soundLengthSizer, wxSizerFlags(0).Expand().Border(wxALL, 3));
    soundSizer->Add(soundLoadBtn, wxSizerFlags(0).Border(wxALL, 3));

    _pronounciationCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _pronounciationCtrl->SetMinSize(wxSize(400, 200));
    auto pronounciationSaveBtn = new wxButton(this, wxID_ANY, "Save");
    pronounciationSaveBtn->Bind(wxEVT_BUTTON, &ComposeLipDialog::OnPronounciationSaveCommand, this);
    auto pronounciationSizer = new wxStaticBoxSizer(wxVERTICAL, this, "Pronounciation");
    pronounciationSizer->Add(_pronounciationCtrl, wxSizerFlags(0).Border(wxALL, 3));
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
    topSizer->Add(pronounciationSizer, wxSizerFlags(0).Border(wxALL, 3));
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
    std::shared_ptr<AudioBuffer> audio;
    if (lowerExt == ".wav") {
        auto wav = FileInputStream(path);
        auto mp3ReaderFactory = Mp3ReaderFactory();
        auto reader = WavReader(wav, mp3ReaderFactory);
        reader.load();
        audio = reader.stream();
    } else if (lowerExt == ".mp3") {
        auto mp3 = FileInputStream(path);
        auto reader = Mp3Reader();
        reader.load(mp3);
        audio = reader.stream();
    }
    if (!audio) {
        return;
    }
    _soundLength = audio->duration();
    auto lengthStr = str(boost::format("%.04f") % audio->duration());
    _soundLengthCtrl->SetValue(lengthStr);
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
    auto cmudictPath = std::filesystem::current_path();
    cmudictPath.append("cmudict.dict");
    auto cmudict = FileInputStream(cmudictPath);

    auto pronounciationValue = _pronounciationCtrl->GetValue();
    auto pronouncingBuffer = ByteBuffer(pronounciationValue.begin(), pronounciationValue.end());
    auto pronouncing = MemoryInputStream(pronouncingBuffer);

    auto dict = PronouncingDictionary();
    dict.load(cmudict);
    dict.load(pronouncing);

    std::unique_ptr<LipAnimation> anim;
    auto composer = LipComposer(dict);
    try {
        anim = composer.compose("composed", text, _soundLength);
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
