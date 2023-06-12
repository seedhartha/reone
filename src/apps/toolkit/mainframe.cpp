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

#include "mainframe.h"

#include <wx/dirdlg.h>
#include <wx/mstream.h>

#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/audio/stream.h"
#include "reone/game/format/ssfreader.h"
#include "reone/game/script/routines.h"
#include "reone/graphics/format/lipreader.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/rimreader.h"
#include "reone/resource/format/tlkreader.h"
#include "reone/resource/talktable.h"
#include "reone/resource/typeutil.h"
#include "reone/system/hexutil.h"
#include "reone/system/pathutil.h"
#include "reone/system/stream/bytearrayinput.h"
#include "reone/system/stream/bytearrayoutput.h"
#include "reone/system/stream/fileinput.h"
#include "reone/tools/2da.h"
#include "reone/tools/audio.h"
#include "reone/tools/erf.h"
#include "reone/tools/gff.h"
#include "reone/tools/keybif.h"
#include "reone/tools/lip.h"
#include "reone/tools/ncs.h"
#include "reone/tools/rim.h"
#include "reone/tools/ssf.h"
#include "reone/tools/tlk.h"
#include "reone/tools/tpc.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;

namespace reone {

static constexpr char kIconName[] = "reone";
static constexpr int kMinPanelWidth = 640;

static const set<string> kFilesSubdirectoryWhitelist {
    "data", "lips", "localvault", "modules", "movies", "override", "rims", "saves", "texturepacks", //
    "streammusic", "streamsounds", "streamwaves", "streamvoice"};

static const set<string> kFilesArchiveExtensions {".bif", ".erf", ".sav", ".rim", ".mod"};

static const set<string> kFilesExtensionBlacklist {
    ".key",                                         //
    ".lnk", ".bat", ".exe", ".dll", ".ini", ".ico", //
    ".zip", ".pdf",                                 //
    ".hashdb", ".info", ".script", ".dat", ".msg", ".sdb", ".ds_store"};

static const set<ResourceType> kFilesPlaintextExtensions {
    ResourceType::Txt,
    ResourceType::Txi,
    ResourceType::Lyt,
    ResourceType::Vis};

struct EventHandlerID {
    static constexpr int openGameDir = wxID_HIGHEST + 1;
    static constexpr int extractAllBifs = wxID_HIGHEST + 2;
    static constexpr int batchTpcToTga = wxID_HIGHEST + 3;
    static constexpr int extractTool = wxID_HIGHEST + 4;
    static constexpr int unwrapTool = wxID_HIGHEST + 5;
    static constexpr int toRimTool = wxID_HIGHEST + 6;
    static constexpr int toErfTool = wxID_HIGHEST + 7;
    static constexpr int toModTool = wxID_HIGHEST + 8;
    static constexpr int toXmlTool = wxID_HIGHEST + 9;
    static constexpr int toTwoDaTool = wxID_HIGHEST + 10;
    static constexpr int toGffTool = wxID_HIGHEST + 11;
    static constexpr int toTlkTool = wxID_HIGHEST + 12;
    static constexpr int toLipTool = wxID_HIGHEST + 13;
    static constexpr int toSsfTool = wxID_HIGHEST + 14;
    static constexpr int toTgaTool = wxID_HIGHEST + 15;
    static constexpr int toPcodeTool = wxID_HIGHEST + 16;
    static constexpr int toNcsTool = wxID_HIGHEST + 17;
    static constexpr int toNssTool = wxID_HIGHEST + 18;
};

struct CommandID {
    static constexpr int extract = 1;
};

struct TimerID {
    static constexpr int audio = 1;
};

MainFrame::MainFrame() :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
    _audioTimer(this, TimerID::audio) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(1024, 768));
    Maximize();

    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openGameDir, "&Open game directory...");
    auto toolsMenu = new wxMenu();
    toolsMenu->Append(EventHandlerID::extractAllBifs, "Extract all BIF archives...");
    toolsMenu->Append(EventHandlerID::batchTpcToTga, "Batch convert TPC to TGA/TXI...");
    toolsMenu->AppendSeparator();
    toolsMenu->Append(EventHandlerID::extractTool, "Extract BIF/RIM/ERF archive...");
    toolsMenu->Append(EventHandlerID::unwrapTool, "Unwrap WAV to WAV/MP3...");
    toolsMenu->Append(EventHandlerID::toRimTool, "Create RIM from directory...");
    toolsMenu->Append(EventHandlerID::toErfTool, "Create ERF from directory...");
    toolsMenu->Append(EventHandlerID::toModTool, "Create MOD from directory...");
    toolsMenu->Append(EventHandlerID::toXmlTool, "Convert 2DA/GFF/TLK/LIP/SSF to XML...");
    toolsMenu->Append(EventHandlerID::toTwoDaTool, "Convert XML to 2DA...");
    toolsMenu->Append(EventHandlerID::toGffTool, "Convert XML to GFF...");
    toolsMenu->Append(EventHandlerID::toTlkTool, "Convert XML to TLK...");
    toolsMenu->Append(EventHandlerID::toLipTool, "Convert XML to LIP...");
    toolsMenu->Append(EventHandlerID::toSsfTool, "Convert XML to SSF...");
    toolsMenu->Append(EventHandlerID::toTgaTool, "Convert TPC to TGA/TXI...");
    toolsMenu->Append(EventHandlerID::toPcodeTool, "Disassemble NCS to PCODE...");
    toolsMenu->Append(EventHandlerID::toNcsTool, "Assemble NCS from PCODE...");
    toolsMenu->Append(EventHandlerID::toNssTool, "Decompile NCS (experimental)...");
    auto menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    menuBar->Append(toolsMenu, "&Tools");
    SetMenuBar(menuBar);

    _splitter = new wxSplitterWindow(this, wxID_ANY);
    _splitter->Bind(wxEVT_SIZE, &MainFrame::OnSplitterSize, this);
    _splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &MainFrame::OnSplitterSashPosChanging, this);
    _splitter->SetMinimumPaneSize(300);

    /*
    auto dataSplitter = new wxSplitterWindow(_splitter, wxID_ANY);
    dataSplitter->SetMinimumPaneSize(300);
    */

    auto filesPanel = new wxPanel(_splitter);
    _filesTreeCtrl = new wxDataViewTreeCtrl(filesPanel, wxID_ANY);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &MainFrame::OnFilesTreeCtrlItemExpanding, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainFrame::OnFilesTreeCtrlItemContextMenu, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MainFrame::OnFilesTreeCtrlItemActivated, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &MainFrame::OnFilesTreeCtrlItemEditingDone, this);
    auto filesSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, "Game Directory");
    filesSizer->Add(_filesTreeCtrl, 1, wxEXPAND);
    filesPanel->SetSizer(filesSizer);

    /*
    auto modulesPanel = new wxPanel(dataSplitter);
    _modulesListBox = new wxListBox(modulesPanel, wxID_ANY);
    auto modulesSizer = new wxStaticBoxSizer(wxVERTICAL, modulesPanel, "Modules");
    modulesSizer->Add(_modulesListBox, 1, wxEXPAND);
    modulesPanel->SetSizer(modulesSizer);

    dataSplitter->SplitHorizontally(filesPanel, modulesPanel);
    */

    _notebook = new wxNotebook(_splitter, wxID_ANY);

    _textPanel = new wxPanel(_notebook);
    auto textSizer = new wxBoxSizer(wxVERTICAL);
    _plainTextCtrl = new wxTextCtrl(_textPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _plainTextCtrl->SetEditable(false);
    textSizer->Add(_plainTextCtrl, 1, wxEXPAND);
    _textPanel->SetSizer(textSizer);

    _tablePanel = new wxPanel(_notebook);
    auto tableSizer = new wxBoxSizer(wxVERTICAL);
    _tableCtrl = new wxDataViewListCtrl(_tablePanel, wxID_ANY);
    tableSizer->Add(_tableCtrl, 1, wxEXPAND);
    _tablePanel->SetSizer(tableSizer);

    _talkTablePanel = new wxPanel(_notebook);
    auto talkTableSizer = new wxBoxSizer(wxVERTICAL);
    _talkTableCtrl = new wxDataViewListCtrl(_talkTablePanel, wxID_ANY);
    talkTableSizer->Add(_talkTableCtrl, 1, wxEXPAND);
    _talkTablePanel->SetSizer(talkTableSizer);

    _gffPanel = new wxPanel(_notebook);
    auto gffSizer = new wxBoxSizer(wxVERTICAL);
    _gffTreeCtrl = new wxDataViewTreeCtrl(_gffPanel, wxID_ANY);
    _gffTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &MainFrame::OnGffTreeCtrlItemEditingDone, this);
    gffSizer->Add(_gffTreeCtrl, 1, wxEXPAND);
    _gffPanel->SetSizer(gffSizer);

    _xmlPanel = new wxPanel(_notebook);
    auto xmlSizer = new wxBoxSizer(wxVERTICAL);
    _xmlTextCtrl = new wxStyledTextCtrl(_xmlPanel);
    _xmlTextCtrl->SetLexer(wxSTC_LEX_XML);
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_XMLSTART, wxColour(255, 0, 0));
    _xmlTextCtrl->StyleSetBackground(wxSTC_H_XMLSTART, wxColour(255, 255, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_XMLEND, wxColour(255, 0, 0));
    _xmlTextCtrl->StyleSetBackground(wxSTC_H_XMLEND, wxColour(255, 255, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_DEFAULT, wxColour(0, 0, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_COMMENT, wxColour(0, 128, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_NUMBER, wxColour(255, 0, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_DOUBLESTRING, wxColour(128, 0, 255));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_SINGLESTRING, wxColour(128, 0, 255));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_TAG, wxColour(0, 0, 255));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_TAGEND, wxColour(0, 0, 255));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_TAGUNKNOWN, wxColour(0, 0, 255));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_ATTRIBUTE, wxColour(255, 0, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN, wxColour(255, 0, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_ENTITY, wxColour(0, 0, 0));
    _xmlTextCtrl->StyleSetForeground(wxSTC_H_CDATA, wxColour(255, 128, 0));
    _xmlTextCtrl->SetEditable(false);
    xmlSizer->Add(_xmlTextCtrl, 1, wxEXPAND);
    _xmlPanel->SetSizer(xmlSizer);

    _nssPanel = new wxPanel(_notebook);
    auto nssSizer = new wxBoxSizer(wxVERTICAL);
    _nssTextCtrl = new wxStyledTextCtrl(_nssPanel);
    _nssTextCtrl->SetEditable(false);
    _nssTextCtrl->SetLexer(wxSTC_LEX_CPP);
    _nssTextCtrl->SetKeyWords(0, "break case continue default do else for if return switch while");
    _nssTextCtrl->SetKeyWords(1, "action command const effect event float int itemproperty location object string struct talent vector void");
    _nssTextCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(128, 64, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_DEFAULT, wxColour(0, 0, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_WORD2, wxColour(128, 0, 255));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_NUMBER, wxColour(255, 128, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_STRING, wxColour(128, 128, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_CHARACTER, wxColour(128, 128, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_VERBATIM, wxColour(0, 0, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_REGEX, wxColour(0, 0, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTLINEDOC, wxColour(0, 128, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD, wxColour(0, 128, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0, 128, 128));
    _nssTextCtrl->StyleSetForeground(wxSTC_C_PREPROCESSORCOMMENT, wxColour(0, 128, 0));
    nssSizer->Add(_nssTextCtrl, 1, wxEXPAND);
    _nssPanel->SetSizer(nssSizer);

    _pcodePanel = new wxPanel(_notebook);
    auto pcodeSizer = new wxBoxSizer(wxVERTICAL);
    _pcodeTextCtrl = new wxTextCtrl(_pcodePanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _pcodeTextCtrl->SetEditable(false);
    pcodeSizer->Add(_pcodeTextCtrl, 1, wxEXPAND);
    _pcodePanel->SetSizer(pcodeSizer);

    _imageSplitter = new wxSplitterWindow(_notebook, wxID_ANY);
    _imageCanvas = new wxPanel(_imageSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    _imageCanvas->Bind(wxEVT_PAINT, &MainFrame::OnImageCanvasPaint, this);
    _imageInfoCtrl = new wxTextCtrl(_imageSplitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _imageInfoCtrl->SetEditable(false);
    _imageSplitter->SetMinimumPaneSize(100);
    _imageSplitter->SplitHorizontally(_imageCanvas, _imageInfoCtrl, std::numeric_limits<int>::max());

    _renderPanel = new wxPanel(_notebook);
    auto renderSizer = new wxBoxSizer(wxVERTICAL);
    _glCanvas = new wxGLCanvas(_renderPanel, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    _glCanvas->Bind(wxEVT_PAINT, &MainFrame::OnGLCanvasPaint, this);
    auto glContext = new wxGLContext(_glCanvas);
    glContext->SetCurrent(*_glCanvas);
    renderSizer->Add(_glCanvas, 1, wxEXPAND);
    _renderPanel->SetSizer(renderSizer);

    _audioPanel = new wxPanel(_notebook);
    auto stopAudioButton = new wxButton(_audioPanel, wxID_ANY, "Stop");
    stopAudioButton->Bind(wxEVT_BUTTON, &MainFrame::OnStopAudioCommand, this);
    auto audioSizer = new wxBoxSizer(wxVERTICAL);
    audioSizer->Add(stopAudioButton);
    _audioPanel->SetSizer(audioSizer);

    _splitter->SplitVertically(filesPanel, _notebook, 1);

    _textPanel->Hide();
    _tablePanel->Hide();
    _talkTablePanel->Hide();
    _gffPanel->Hide();
    _xmlPanel->Hide();
    _nssPanel->Hide();
    _pcodePanel->Hide();
    _imageSplitter->Hide();
    _renderPanel->Hide();
    _audioPanel->Hide();

    _viewModel = make_unique<MainViewModel>();
    _viewModel->onViewCreated();

    _audioCtx = make_unique<AudioContext>();
    _audioCtx->init();

    // CreateStatusBar();
}

void MainFrame::OnClose(wxCloseEvent &event) {
    Destroy();
    _viewModel->onViewDestroyed();
}

void MainFrame::OnOpenGameDirectoryCommand(wxCommandEvent &event) {
    auto dialog = new wxDirDialog(nullptr, "Choose game directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog->ShowModal() != wxID_OK) {
        return;
    }
    auto gamePath = boost::filesystem::path((string)dialog->GetPath());
    auto keyPath = getPathIgnoreCase(gamePath, "chitin.key", false);
    auto modulesPath = getPathIgnoreCase(gamePath, "modules", false);
    if (keyPath.empty() || modulesPath.empty()) {
        wxMessageBox("Not a valid game directory", "Error", wxICON_ERROR);
        return;
    }
    _gamePath = gamePath;

    auto tslExePath = getPathIgnoreCase(gamePath, "swkotor2.exe", false);
    _gameId = !tslExePath.empty() ? GameID::TSL : GameID::KotOR;

    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = KeyReader();
    keyReader.load(key);
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    _strings.init(gamePath);

    _filesTreeCtrl->DeleteAllItems();
    for (auto &file : boost::filesystem::directory_iterator(gamePath)) {
        auto filename = boost::to_lower_copy(file.path().filename().string());
        auto extension = boost::to_lower_copy(file.path().extension().string());
        void *itemId;
        if ((file.status().type() == boost::filesystem::directory_file && kFilesSubdirectoryWhitelist.count(filename) > 0) ||
            (file.status().type() == boost::filesystem::regular_file && kFilesArchiveExtensions.count(extension) > 0)) {
            auto item = _filesTreeCtrl->AppendContainer(wxDataViewItem(), filename);
            itemId = item.GetID();
        } else if (file.status().type() == boost::filesystem::regular_file && (kFilesExtensionBlacklist.count(extension) == 0 && extension != ".txt")) {
            auto item = _filesTreeCtrl->AppendItem(wxDataViewItem(), filename);
            itemId = item.GetID();
        } else {
            continue;
        }
        auto entry = FilesEntry();
        entry.path = file.path();
        if (extension == ".tlk") {
            auto resRef = filename.substr(0, filename.size() - 4);
            entry.resId = make_unique<ResourceId>(resRef, ResourceType::Tlk);
        }
        _files.insert(make_pair(itemId, std::move(entry)));
    }

    /*
    _modulesListBox->Clear();
    for (auto &entry : boost::filesystem::directory_iterator(modulesPath)) {
        auto filename = boost::to_lower_copy(entry.path().filename().string());
        auto extension = boost::to_lower_copy(entry.path().extension().string());
        if (extension != ".rim" && extension != ".mod") {
            continue;
        }
        if (extension == ".rim" && boost::ends_with(filename, "_s.rim")) {
            continue;
        }
        auto moduleName = boost::to_lower_copy(entry.path().filename().replace_extension().string());
        _modulesListBox->AppendString(moduleName);
    }
    */

    _viewModel->onGameDirectoryChanged(_gamePath);
}

void MainFrame::OnExtractAllBifsCommand(wxCommandEvent &event) {
    if (_gamePath.empty()) {
        wxMessageBox("Game directory must be open", "Error", wxICON_ERROR);
        return;
    }
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = boost::filesystem::path((string)destDirDialog->GetPath());
    auto tool = KeyBifTool();

    auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key");
    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = KeyReader();
    keyReader.load(key);

    int bifIdx = 0;
    for (auto &file : _keyFiles) {
        auto cleanedFilename = boost::replace_all_copy(file.filename, "\\", "/");
        auto bifPath = getPathIgnoreCase(_gamePath, cleanedFilename);
        if (bifPath.empty()) {
            warn("BIF not found: " + bifPath.string());
            continue;
        }
        tool.extractBIF(keyReader, bifIdx++, bifPath, destPath);
    }

    wxMessageBox("Operation completed successfully", "Success");
}

void MainFrame::OnBatchConvertTpcToTgaCommand(wxCommandEvent &event) {
    auto srcDirDialog = new wxDirDialog(nullptr, "Choose source directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (srcDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto srcPath = boost::filesystem::path((string)srcDirDialog->GetPath());
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = boost::filesystem::path((string)destDirDialog->GetPath());
    auto tool = TpcTool();
    for (auto &file : boost::filesystem::directory_iterator(srcPath)) {
        auto extension = boost::to_lower_copy(file.path().extension().string());
        if (file.status().type() != boost::filesystem::regular_file || extension != ".tpc") {
            continue;
        }
        tool.toTGA(file.path(), destPath);
    }
    wxMessageBox("Operation completed successfully", "Success");
}

void MainFrame::OnExtractToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::Extract);
}

void MainFrame::OnUnwrapToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::Unwrap);
}

void MainFrame::OnToRimToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToRIM);
}

void MainFrame::OnToErfToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToERF);
}

void MainFrame::OnToModToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToMOD);
}

void MainFrame::OnToXmlToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToXML);
}

void MainFrame::OnToTwoDaToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::To2DA);
}

void MainFrame::OnToGffToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToGFF);
}

void MainFrame::OnToTlkToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToTLK);
}

void MainFrame::OnToLipToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToLIP);
}

void MainFrame::OnToSsfToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToSSF);
}

void MainFrame::OnToTgaToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToTGA);
}

void MainFrame::OnToPcodeToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToPCODE);
}

void MainFrame::OnToNcsToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToNCS);
}

void MainFrame::OnToNssToolCommand(wxCommandEvent &event) {
    InvokeTool(Operation::ToNSS);
}

void MainFrame::InvokeTool(Operation operation) {
    auto srcFileDialog = new wxFileDialog(
        nullptr,
        "Choose source file",
        "",
        "",
        wxString::FromAscii(wxFileSelectorDefaultWildcardStr),
        wxFD_DEFAULT_STYLE | wxFD_FILE_MUST_EXIST);
    if (srcFileDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto srcPath = boost::filesystem::path((string)srcFileDialog->GetPath());
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = boost::filesystem::path((string)destDirDialog->GetPath());
    if (_viewModel->invokeTool(operation, srcPath, destPath)) {
        wxMessageBox("Operation completed successfully", "Success");
    } else {
        wxMessageBox("Tool not found", "Error", wxICON_ERROR);
    }
}

void MainFrame::OnSplitterSize(wxSizeEvent &event) {
    int requestedPanelSize = event.GetSize().x - _splitter->GetSashSize() - _splitter->GetSashPosition();
    if (requestedPanelSize < kMinPanelWidth) {
        _splitter->SetSashPosition(event.GetSize().x - _splitter->GetSashSize() - kMinPanelWidth);
    }
    event.Skip();
}

void MainFrame::OnSplitterSashPosChanging(wxSplitterEvent &event) {
    int requestedPanelSize = _splitter->GetSize().x - _splitter->GetSashSize() - event.GetSashPosition();
    if (requestedPanelSize < kMinPanelWidth) {
        event.SetSashPosition(_splitter->GetSize().x - _splitter->GetSashSize() - kMinPanelWidth);
    }
}

void MainFrame::OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event) {
    auto expandingItemId = event.GetItem().GetID();
    auto &expandingItem = _files.at(expandingItemId);
    if (expandingItem.loaded) {
        return;
    }
    if (boost::filesystem::is_directory(expandingItem.path)) {
        for (auto &file : boost::filesystem::directory_iterator(expandingItem.path)) {
            auto filename = boost::to_lower_copy(file.path().filename().string());
            auto extension = boost::to_lower_copy(file.path().extension().string());
            void *itemId;
            if (file.status().type() == boost::filesystem::directory_file || kFilesArchiveExtensions.count(extension) > 0) {
                auto item = _filesTreeCtrl->AppendContainer(event.GetItem(), filename);
                itemId = item.GetID();
            } else if (file.status().type() == boost::filesystem::regular_file && kFilesExtensionBlacklist.count(extension) == 0) {
                auto item = _filesTreeCtrl->AppendItem(event.GetItem(), filename);
                itemId = item.GetID();
            } else {
                continue;
            }
            auto entry = FilesEntry();
            entry.path = file.path();
            if (!extension.empty()) {
                auto resType = getResTypeByExt(extension.substr(1), false);
                if (resType != ResourceType::Invalid) {
                    auto resRef = filename.substr(0, filename.size() - 4);
                    entry.resId = make_unique<ResourceId>(resRef, resType);
                }
            }
            _files.insert(make_pair(itemId, std::move(entry)));
        }
    } else {
        auto extension = boost::to_lower_copy(expandingItem.path.extension().string());
        if (boost::ends_with(extension, ".bif")) {
            auto filename = str(boost::format("data/%s") % boost::to_lower_copy(expandingItem.path.filename().string()));
            auto maybeFile = std::find_if(_keyFiles.begin(), _keyFiles.end(), [&filename](auto &file) {
                return boost::to_lower_copy(file.filename) == filename;
            });
            if (maybeFile != _keyFiles.end()) {
                auto bifIdx = std::distance(_keyFiles.begin(), maybeFile);
                for (auto &key : _keyKeys) {
                    if (key.bifIdx != bifIdx) {
                        continue;
                    }
                    auto itemText = str(boost::format("%s.%s") % key.resId.resRef % getExtByResType(key.resId.type));
                    auto item = _filesTreeCtrl->AppendItem(event.GetItem(), itemText);
                    auto entry = FilesEntry();
                    entry.path = expandingItem.path;
                    entry.resId = make_unique<ResourceId>(key.resId);
                    entry.archived = true;
                    _files.insert(make_pair(item.GetID(), std::move(entry)));
                }
            }
        } else if (boost::ends_with(extension, ".erf") || boost::ends_with(extension, ".sav") || boost::ends_with(extension, ".mod")) {
            auto erf = FileInputStream(expandingItem.path, OpenMode::Binary);
            auto erfReader = ErfReader();
            erfReader.load(erf);
            auto &keys = erfReader.keys();
            for (auto &key : keys) {
                auto itemText = str(boost::format("%s.%s") % key.resId.resRef % getExtByResType(key.resId.type));
                auto item = _filesTreeCtrl->AppendItem(event.GetItem(), itemText);
                auto entry = FilesEntry();
                entry.path = expandingItem.path;
                entry.resId = make_unique<ResourceId>(key.resId);
                entry.archived = true;
                _files.insert(make_pair(item.GetID(), std::move(entry)));
            }
        } else if (boost::ends_with(extension, ".rim")) {
            auto rim = FileInputStream(expandingItem.path, OpenMode::Binary);
            auto rimReader = RimReader();
            rimReader.load(rim);
            auto &resources = rimReader.resources();
            for (auto &resource : resources) {
                auto itemText = str(boost::format("%s.%s") % resource.resId.resRef % getExtByResType(resource.resId.type));
                auto item = _filesTreeCtrl->AppendItem(event.GetItem(), itemText);
                auto entry = FilesEntry();
                entry.path = expandingItem.path;
                entry.resId = make_unique<ResourceId>(resource.resId);
                entry.archived = true;
                _files.insert(make_pair(item.GetID(), std::move(entry)));
            }
        }
    }
    expandingItem.loaded = true;
}

void MainFrame::OnFilesTreeCtrlItemActivated(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    if (_files.count(itemId) == 0) {
        return;
    }
    auto &item = _files.at(itemId);
    OpenFile(item);
}

void MainFrame::OnFilesTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    if (_files.count(itemId) == 0) {
        return;
    }
    auto &item = _files.at(itemId);
    if (item.archived) {
        return;
    }
    auto extension = boost::to_lower_copy(item.path.extension().string());
    if (!boost::filesystem::is_regular_file(item.path) || kFilesArchiveExtensions.count(extension) == 0) {
        return;
    }
    auto menu = wxMenu();
    menu.Append(CommandID::extract, "Extract...");
    menu.SetClientData(&item);
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPopupCommandSelected), nullptr, this);
    PopupMenu(&menu, event.GetPosition());
}

void MainFrame::OpenFile(FilesEntry &entry) {
    if (!entry.resId) {
        return;
    }
    if (entry.archived) {
        auto extension = boost::to_lower_copy(entry.path.extension().string());
        if (extension == ".bif") {
            auto maybeKey = std::find_if(_keyKeys.begin(), _keyKeys.end(), [&entry](auto &key) {
                return key.resId == *entry.resId;
            });
            if (maybeKey == _keyKeys.end()) {
                return;
            }
            auto resIdx = maybeKey->resIdx;
            auto bif = FileInputStream(entry.path, OpenMode::Binary);
            auto bifReader = BifReader();
            bifReader.load(bif);
            if (bifReader.resources().size() <= resIdx) {
                return;
            }
            auto &bifEntry = bifReader.resources().at(resIdx);
            auto resBytes = make_unique<ByteArray>();
            resBytes->resize(bifEntry.fileSize);
            bif.seek(bifEntry.offset, SeekOrigin::Begin);
            bif.read(&(*resBytes)[0], bifEntry.fileSize);
            auto res = ByteArrayInputStream(*resBytes);
            OpenResource(*entry.resId, res);
        } else if (extension == ".erf" || extension == ".sav" || extension == ".mod") {
            auto erf = FileInputStream(entry.path, OpenMode::Binary);
            auto erfReader = ErfReader();
            erfReader.load(erf);
            auto maybeKey = std::find_if(erfReader.keys().begin(), erfReader.keys().end(), [&entry](auto &key) {
                return key.resId == *entry.resId;
            });
            if (maybeKey == erfReader.keys().end()) {
                return;
            }
            auto resIdx = std::distance(erfReader.keys().begin(), maybeKey);
            auto &erfEntry = erfReader.resources().at(resIdx);
            auto resBytes = make_unique<ByteArray>();
            resBytes->resize(erfEntry.size);
            erf.seek(erfEntry.offset, SeekOrigin::Begin);
            erf.read(&(*resBytes)[0], erfEntry.size);
            auto res = ByteArrayInputStream(*resBytes);
            OpenResource(*entry.resId, res);
        } else if (extension == ".rim") {
            auto rim = FileInputStream(entry.path, OpenMode::Binary);
            auto rimReader = RimReader();
            rimReader.load(rim);
            auto maybeRes = std::find_if(rimReader.resources().begin(), rimReader.resources().end(), [&entry](auto &res) {
                return res.resId == *entry.resId;
            });
            if (maybeRes == rimReader.resources().end()) {
                return;
            }
            auto &rimRes = *maybeRes;
            auto resBytes = make_unique<ByteArray>();
            resBytes->resize(rimRes.size);
            rim.seek(rimRes.offset, SeekOrigin::Begin);
            rim.read(&(*resBytes)[0], rimRes.size);
            auto res = ByteArrayInputStream(*resBytes);
            OpenResource(*entry.resId, res);
        }
    } else {
        auto res = FileInputStream(entry.path, OpenMode::Binary);
        OpenResource(*entry.resId, res);
    }
}

void MainFrame::OpenResource(ResourceId &id, IInputStream &data) {
    bool talkTableOpen = false;
    size_t audioPageIdx = -1;
    for (size_t i = 0; i < _notebook->GetPageCount(); ++i) {
        if (_notebook->GetPage(i) == _talkTablePanel) {
            talkTableOpen = true;
        }
        if (_notebook->GetPage(i) == _audioPanel && _audioSource && _audioSource->isPlaying()) {
            audioPageIdx = i;
        }
    }
    while (_notebook->GetPageCount() > 0) {
        _notebook->RemovePage(0);
    }
    if (talkTableOpen) {
        _notebook->AddPage(_talkTablePanel, "dialog.tlk");
    }
    if (audioPageIdx != -1) {
        _notebook->AddPage(_audioPanel, _audioResId.string());
    }

    _textPanel->Hide();
    _tablePanel->Hide();
    _gffPanel->Hide();
    _xmlPanel->Hide();
    _nssPanel->Hide();
    _pcodePanel->Hide();
    _imageSplitter->Hide();
    _renderPanel->Hide();

    if (id.type == ResourceType::TwoDa) {
        _tableCtrl->ClearColumns();
        _tableCtrl->DeleteAllItems();
        auto reader = TwoDaReader();
        reader.load(data);
        auto twoDa = reader.twoDa();
        _tableCtrl->AppendTextColumn("Index");
        for (auto &column : twoDa->columns()) {
            _tableCtrl->AppendTextColumn(column);
        }
        for (int i = 0; i < twoDa->getRowCount(); ++i) {
            auto &row = twoDa->rows()[i];
            auto values = wxVector<wxVariant>();
            values.push_back(wxVariant(to_string(i)));
            for (auto &value : row.values) {
                values.push_back(wxVariant(value));
            }
            _tableCtrl->AppendItem(values);
        }
        _notebook->AddPage(_tablePanel, id.string(), true);
        _tablePanel->Show();

    } else if (isGFFCompatibleResType(id.type)) {
        _gffTreeCtrl->DeleteAllItems();
        auto reader = GffReader();
        reader.load(data);
        auto root = reader.root();
        AppendGffStructToTree(wxDataViewItem(), "/", *root);
        _notebook->AddPage(_gffPanel, id.string(), true);
        _gffPanel->Show();

    } else if (id.type == ResourceType::Tlk) {
        if (_talkTableCtrl->GetItemCount() == 0) {
            _talkTableCtrl->ClearColumns();
            _talkTableCtrl->DeleteAllItems();
            auto reader = TlkReader();
            reader.load(data);
            auto tlk = reader.table();
            _talkTableCtrl->AppendTextColumn("Index");
            _talkTableCtrl->AppendTextColumn("Text");
            _talkTableCtrl->AppendTextColumn("Sound");
            for (int i = 0; i < tlk->getStringCount(); ++i) {
                auto &str = tlk->getString(i);
                auto values = wxVector<wxVariant>();
                auto cleaned = boost::replace_all_copy(str.text, "\n", "\\n");
                values.push_back(wxVariant(to_string(i)));
                values.push_back(wxVariant(cleaned));
                values.push_back(wxVariant(str.soundResRef));
                _talkTableCtrl->AppendItem(values);
            }
        }
        if (!talkTableOpen) {
            _notebook->AddPage(_talkTablePanel, id.string(), true);
        }
        _talkTablePanel->Show();

    } else if (kFilesPlaintextExtensions.count(id.type) > 0) {
        data.seek(0, SeekOrigin::End);
        auto length = data.position();
        data.seek(0, SeekOrigin::Begin);
        auto text = string(length, '\0');
        data.read(&text[0], length);
        _plainTextCtrl->SetEditable(true);
        _plainTextCtrl->Clear();
        _plainTextCtrl->AppendText(text);
        _plainTextCtrl->SetEditable(false);
        _notebook->AddPage(_textPanel, id.string(), true);
        _textPanel->Show();

    } else if (id.type == ResourceType::Ncs) {
        auto gameId = GameID::KotOR;
        auto routines = make_unique<Routines>(gameId, nullptr, nullptr);
        routines->init();

        auto pcodeBytes = make_unique<ByteArray>();
        auto pcode = ByteArrayOutputStream(*pcodeBytes);
        auto tool = NcsTool(gameId);
        tool.toPCODE(data, pcode, *routines);
        _pcodeTextCtrl->SetEditable(true);
        _pcodeTextCtrl->Clear();
        _pcodeTextCtrl->AppendText(*pcodeBytes);
        _pcodeTextCtrl->SetEditable(false);
        _notebook->AddPage(_pcodePanel, str(boost::format("%s.pcode") % id.resRef), true);
        _pcodePanel->Show();

        data.seek(0, SeekOrigin::Begin);
        auto nssBytes = make_unique<ByteArray>();
        auto nss = ByteArrayOutputStream(*nssBytes);
        tool.toNSS(data, nss, *routines);
        _nssTextCtrl->SetEditable(true);
        _nssTextCtrl->SetText(*nssBytes);
        _nssTextCtrl->SetEditable(false);
        _notebook->AddPage(_nssPanel, str(boost::format("%s.nss") % id.resRef), true);
        _nssPanel->Show();

    } else if (id.type == ResourceType::Nss) {
        data.seek(0, SeekOrigin::End);
        auto length = data.position();
        data.seek(0, SeekOrigin::Begin);
        auto text = string(length, '\0');
        data.read(&text[0], length);
        _nssTextCtrl->SetEditable(true);
        _nssTextCtrl->SetText(text);
        _nssTextCtrl->SetEditable(false);
        _notebook->AddPage(_nssPanel, id.string(), true);
        _nssPanel->Show();

    } else if (id.type == ResourceType::Lip) {
        _tableCtrl->ClearColumns();
        _tableCtrl->DeleteAllItems();
        auto reader = LipReader("");
        reader.load(data);
        auto animation = reader.animation();
        _tableCtrl->AppendTextColumn("Time");
        _tableCtrl->AppendTextColumn("Shape");
        for (auto &kf : animation->keyframes()) {
            auto values = wxVector<wxVariant>();
            values.push_back(wxVariant(to_string(kf.time)));
            values.push_back(wxVariant(to_string(kf.shape)));
            _tableCtrl->AppendItem(values);
        }
        _notebook->AddPage(_tablePanel, id.string(), true);
        _tablePanel->Show();

    } else if (id.type == ResourceType::Ssf) {
        _tableCtrl->ClearColumns();
        _tableCtrl->DeleteAllItems();
        auto reader = SsfReader();
        reader.load(data);
        auto &soundSet = reader.soundSet();
        _tableCtrl->AppendTextColumn("Index");
        _tableCtrl->AppendTextColumn("Sound");
        for (size_t i = 0; i < soundSet.size(); ++i) {
            auto values = wxVector<wxVariant>();
            values.push_back(wxVariant(to_string(i)));
            values.push_back(wxVariant(to_string(soundSet.at(i))));
            _tableCtrl->AppendItem(values);
        }
        _notebook->AddPage(_tablePanel, id.string(), true);
        _tablePanel->Show();

    } else if (id.type == ResourceType::Tpc || id.type == ResourceType::Tga) {
        auto tgaBytes = make_unique<ByteArray>();
        if (id.type == ResourceType::Tpc) {
            auto tga = ByteArrayOutputStream(*tgaBytes);
            auto txiBytes = make_unique<ByteArray>();
            auto txi = ByteArrayOutputStream(*txiBytes);
            TpcTool().toTGA(data, tga, txi);
            if (txiBytes->empty()) {
                _imageSplitter->Unsplit(_imageInfoCtrl);
            } else {
                _imageInfoCtrl->Clear();
                _imageInfoCtrl->AppendText(*txiBytes);
                _imageSplitter->SplitHorizontally(_imageCanvas, _imageInfoCtrl, std::numeric_limits<int>::max());
            }
        } else {
            data.seek(0, SeekOrigin::End);
            auto length = data.position();
            data.seek(0, SeekOrigin::Begin);
            tgaBytes->resize(length, '\0');
            data.read(&(*tgaBytes)[0], length);
            _imageSplitter->Unsplit(_imageInfoCtrl);
        }
        auto wxStream = wxMemoryInputStream(&(*tgaBytes)[0], tgaBytes->size());
        auto image = wxImage();
        image.LoadFile(wxStream, wxBITMAP_TYPE_TGA);
        _image = make_unique<wxBitmap>(image);
        _notebook->AddPage(_imageSplitter, id.string(), true);
        _imageSplitter->Show();

    } else if (id.type == ResourceType::Wav /* || id.type == ResourceType::Mp3 */) {
        auto mp3ReaderFactory = Mp3ReaderFactory();
        auto reader = WavReader(mp3ReaderFactory);
        reader.load(data);
        _audioSource = make_unique<AudioSource>(reader.stream(), false, 1.0f, false, glm::vec3());
        _audioSource->init();
        _audioSource->play();
        if (!_audioTimer.IsRunning()) {
            _audioTimer.Start(1000 / 60);
        }
        _audioResId = id;
        if (audioPageIdx != -1) {
            _notebook->SetPageText(audioPageIdx, id.string());
        } else {
            _notebook->AddPage(_audioPanel, id.string(), true);
        }
        _audioPanel->Show();

    } else {
        wxMessageBox("Resource type not supported", "Warning", wxICON_WARNING);
        return;
    }

    /*
    _notebook->AddPage(_renderPanel, "3D");
    _renderPanel->Show();
    */
}

void MainFrame::AppendGffStructToTree(wxDataViewItem parent, const string &text, const Gff &gff) {
    auto structItem = _gffTreeCtrl->AppendContainer(parent, str(boost::format("%s [%d]") % text % static_cast<int>(gff.type())));
    for (auto &field : gff.fields()) {
        switch (field.type) {
        case Gff::FieldType::CExoString:
        case Gff::FieldType::ResRef: {
            auto cleaned = boost::replace_all_copy(field.strValue, "\n", "\\n");
            _gffTreeCtrl->AppendItem(structItem, str(boost::format("%s = \"%s\" [%d]") % field.label % cleaned % static_cast<int>(field.type)));
        } break;
        case Gff::FieldType::CExoLocString: {
            auto locStringItem = _gffTreeCtrl->AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            _gffTreeCtrl->AppendItem(locStringItem, str(boost::format("StrRef = %d") % field.intValue));
            _gffTreeCtrl->AppendItem(locStringItem, str(boost::format("Substring = \"%s\"") % field.strValue));
        } break;
        case Gff::FieldType::Void:
            _gffTreeCtrl->AppendItem(structItem, str(boost::format("%s = \"%s\" [%d]") % field.label % hexify(field.data, "") % static_cast<int>(field.type)));
            break;
        case Gff::FieldType::Struct:
            AppendGffStructToTree(structItem, field.label, *field.children[0]);
            break;
        case Gff::FieldType::List: {
            auto listItem = _gffTreeCtrl->AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            for (auto it = field.children.begin(); it != field.children.end(); ++it) {
                auto childIdx = std::distance(field.children.begin(), it);
                AppendGffStructToTree(listItem, to_string(childIdx), **it);
            }
        } break;
        case Gff::FieldType::Orientation: {
            auto orientationItem = _gffTreeCtrl->AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            _gffTreeCtrl->AppendItem(orientationItem, str(boost::format("W = %f") % field.quatValue.w));
            _gffTreeCtrl->AppendItem(orientationItem, str(boost::format("X = %f") % field.quatValue.x));
            _gffTreeCtrl->AppendItem(orientationItem, str(boost::format("Y = %f") % field.quatValue.y));
            _gffTreeCtrl->AppendItem(orientationItem, str(boost::format("Z = %f") % field.quatValue.z));
        } break;
        case Gff::FieldType::Vector: {
            auto vectorItem = _gffTreeCtrl->AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            _gffTreeCtrl->AppendItem(vectorItem, str(boost::format("X = %f") % field.vecValue.x));
            _gffTreeCtrl->AppendItem(vectorItem, str(boost::format("Y = %f") % field.vecValue.y));
            _gffTreeCtrl->AppendItem(vectorItem, str(boost::format("Z = %f") % field.vecValue.z));
        } break;
        case Gff::FieldType::StrRef:
            _gffTreeCtrl->AppendItem(structItem, str(boost::format("%s = %d [%d]") % field.label % field.intValue % static_cast<int>(field.type)));
            break;
        default:
            _gffTreeCtrl->AppendItem(structItem, str(boost::format("%s = %s [%d]") % field.label % field.toString() % static_cast<int>(field.type)));
            break;
        }
    }
}

void MainFrame::OnFilesTreeCtrlItemEditingDone(wxDataViewEvent &event) {
    event.Veto();
}

void MainFrame::OnGffTreeCtrlItemEditingDone(wxDataViewEvent &event) {
    event.Veto();
}

void MainFrame::OnPopupCommandSelected(wxCommandEvent &event) {
    if (event.GetId() == CommandID::extract) {
        auto menu = static_cast<wxMenu *>(event.GetEventObject());
        auto data = menu->GetClientData();
        auto entry = reinterpret_cast<FilesEntry *>(data);
        auto extension = boost::to_lower_copy(entry->path.extension().string());
        if (extension == ".bif") {
            auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
            if (dialog->ShowModal() != wxID_OK) {
                return;
            }
            auto destPath = boost::filesystem::path(string(dialog->GetPath()));
            auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key", false);
            auto keyReader = KeyReader();
            auto key = FileInputStream(keyPath, OpenMode::Binary);
            keyReader.load(key);
            auto filename = boost::to_lower_copy(entry->path.filename().string());
            auto maybeBif = std::find_if(keyReader.files().begin(), keyReader.files().end(), [&filename](auto &file) {
                return boost::contains(boost::to_lower_copy(file.filename), filename);
            });
            if (maybeBif == keyReader.files().end()) {
                return;
            }
            auto bifIdx = std::distance(keyReader.files().begin(), maybeBif);
            KeyBifTool().extractBIF(keyReader, bifIdx, entry->path, destPath);
        } else if (extension == ".erf" || extension == ".sav" || extension == ".mod") {
            auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
            if (dialog->ShowModal() != wxID_OK) {
                return;
            }
            auto destPath = boost::filesystem::path(string(dialog->GetPath()));
            auto erf = FileInputStream(entry->path, OpenMode::Binary);
            auto erfReader = ErfReader();
            erfReader.load(erf);
            ErfTool().extract(erfReader, entry->path, destPath);
        } else if (extension == ".rim") {
            auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
            if (dialog->ShowModal() != wxID_OK) {
                return;
            }
            auto destPath = boost::filesystem::path(string(dialog->GetPath()));
            auto rim = FileInputStream(entry->path, OpenMode::Binary);
            auto rimReader = RimReader();
            rimReader.load(rim);
            RimTool().extract(rimReader, entry->path, destPath);
        }
    }
}

void MainFrame::OnImageCanvasPaint(wxPaintEvent &event) {
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

void MainFrame::OnGLCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(_glCanvas);

    glViewport(0, 0, _glCanvas->GetClientSize().x, _glCanvas->GetClientSize().y);

    _glCanvas->ClearBackground();
    _glCanvas->SwapBuffers();
}

void MainFrame::OnAudioTimer(wxTimerEvent &event) {
    if (_audioSource) {
        _audioSource->update();
    }
}

void MainFrame::OnStopAudioCommand(wxCommandEvent &event) {
    if (_audioSource) {
        _audioSource->stop();
        _audioSource.reset();
    }
    _audioTimer.Stop();
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)                                                     //
    EVT_MENU(EventHandlerID::openGameDir, MainFrame::OnOpenGameDirectoryCommand)      //
    EVT_MENU(EventHandlerID::extractAllBifs, MainFrame::OnExtractAllBifsCommand)      //
    EVT_MENU(EventHandlerID::batchTpcToTga, MainFrame::OnBatchConvertTpcToTgaCommand) //
    EVT_MENU(EventHandlerID::extractTool, MainFrame::OnExtractToolCommand)            //
    EVT_MENU(EventHandlerID::unwrapTool, MainFrame::OnUnwrapToolCommand)              //
    EVT_MENU(EventHandlerID::toRimTool, MainFrame::OnToRimToolCommand)                //
    EVT_MENU(EventHandlerID::toErfTool, MainFrame::OnToErfToolCommand)                //
    EVT_MENU(EventHandlerID::toModTool, MainFrame::OnToModToolCommand)                //
    EVT_MENU(EventHandlerID::toXmlTool, MainFrame::OnToXmlToolCommand)                //
    EVT_MENU(EventHandlerID::toTwoDaTool, MainFrame::OnToTwoDaToolCommand)            //
    EVT_MENU(EventHandlerID::toGffTool, MainFrame::OnToGffToolCommand)                //
    EVT_MENU(EventHandlerID::toTlkTool, MainFrame::OnToTlkToolCommand)                //
    EVT_MENU(EventHandlerID::toLipTool, MainFrame::OnToLipToolCommand)                //
    EVT_MENU(EventHandlerID::toSsfTool, MainFrame::OnToSsfToolCommand)                //
    EVT_MENU(EventHandlerID::toTgaTool, MainFrame::OnToTgaToolCommand)                //
    EVT_MENU(EventHandlerID::toPcodeTool, MainFrame::OnToPcodeToolCommand)            //
    EVT_MENU(EventHandlerID::toNcsTool, MainFrame::OnToNcsToolCommand)                //
    EVT_MENU(EventHandlerID::toNssTool, MainFrame::OnToNssToolCommand)                //
    EVT_TIMER(TimerID::audio, MainFrame::OnAudioTimer)                                //
    wxEND_EVENT_TABLE()

} // namespace reone
