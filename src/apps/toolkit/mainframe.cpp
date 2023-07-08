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
#include "reone/system/fileutil.h"
#include "reone/system/hexutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stream/memoryoutput.h"
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

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

static constexpr char kIconName[] = "toolkit";

static const std::set<std::string> kFilesArchiveExtensions {".bif", ".erf", ".sav", ".rim", ".mod"};

static const std::set<PageType> kStaticPageTypes {
    PageType::Image,
    PageType::Model,
    PageType::Audio};

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
    static constexpr int saveFile = wxID_HIGHEST + 19;
};

struct CommandID {
    static constexpr int extract = 1;
    static constexpr int decompile = 2;
    static constexpr int decompileNoOptimize = 3;
    static constexpr int exportFile = 4;
};

struct TimerID {
    static constexpr int render = 1;
    static constexpr int audio = 2;
};

MainFrame::MainFrame() :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(1024, 768));
    Maximize();

    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openGameDir, "&Open game directory...");
    fileMenu->AppendSeparator();
    _saveFileMenuItem = fileMenu->Append(EventHandlerID::saveFile, "&Save");
    _saveFileMenuItem->Enable(false);
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
    _splitter->SetMinimumPaneSize(300);

    auto filesPanel = new wxPanel(_splitter);
    _filesTreeCtrl = new wxDataViewTreeCtrl(filesPanel, wxID_ANY);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &MainFrame::OnFilesTreeCtrlItemExpanding, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &MainFrame::OnFilesTreeCtrlItemContextMenu, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &MainFrame::OnFilesTreeCtrlItemActivated, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &MainFrame::OnFilesTreeCtrlItemEditingDone, this);
    auto filesSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, "Game Directory");
    filesSizer->Add(_filesTreeCtrl, 1, wxEXPAND);
    filesPanel->SetSizer(filesSizer);

    _notebook = new wxAuiNotebook(_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE & ~(wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE));
    _notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &MainFrame::OnNotebookPageClose, this);
    _notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &MainFrame::OnNotebookPageChanged, this);

    _imageSplitter = new wxSplitterWindow(_notebook, wxID_ANY);
    _imageCanvas = new wxPanel(_imageSplitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    _imageCanvas->Bind(wxEVT_PAINT, &MainFrame::OnImageCanvasPaint, this);
    _imageInfoCtrl = new wxTextCtrl(_imageSplitter, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
    _imageInfoCtrl->SetEditable(false);
    _imageSplitter->SetMinimumPaneSize(100);
    _imageSplitter->SplitHorizontally(_imageCanvas, _imageInfoCtrl, std::numeric_limits<int>::max());

    _renderSplitter = new wxSplitterWindow(_notebook);
    _renderSplitter->SetMinimumPaneSize(100);

    _audioPanel = new wxPanel(_notebook);
    auto stopAudioButton = new wxButton(_audioPanel, wxID_ANY, "Stop");
    stopAudioButton->Bind(wxEVT_BUTTON, &MainFrame::OnStopAudioCommand, this);
    auto audioSizer = new wxBoxSizer(wxVERTICAL);
    audioSizer->Add(stopAudioButton);
    _audioPanel->SetSizer(audioSizer);

    _splitter->SplitVertically(filesPanel, _notebook, 1);

    for (auto &page : kStaticPageTypes) {
        auto window = GetStaticPageWindow(page);
        window->Hide();
    }

    _viewModel = std::make_unique<MainViewModel>();
    _viewModel->pageAdded().subscribe([this](auto &page) {
        wxWindow *window;
        if (kStaticPageTypes.count(page->type) > 0) {
            window = GetStaticPageWindow(page->type);
        } else {
            window = NewPageWindow(*page);
        }
        window->Show();
        _notebook->AddPage(window, page->displayName, true);
    });
    _viewModel->pageRemoving().subscribe([this](auto &data) {
        if (kStaticPageTypes.count(data.page->type) > 0) {
            auto window = GetStaticPageWindow(data.page->type);
            window->Hide();
            _notebook->RemovePage(data.index);
        } else {
            _notebook->DeletePage(data.index);
        }
    });
    _viewModel->pageSelected().subscribe([this](int page) {
        _notebook->SetSelection(page);
    });
    _viewModel->imageChanged().subscribe([this](auto &data) {
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
    _viewModel->animations().subscribe([this](auto &animations) {
        if (!animations.empty()) {
            _animationsListBox->Freeze();
            _animationsListBox->Clear();
            for (auto &animation : animations) {
                _animationsListBox->Append(animation);
            }
            _animationsListBox->Thaw();
            _renderSplitter->SplitHorizontally(_glCanvas, _animationsListBox, std::numeric_limits<int>::max());
        } else {
            _renderSplitter->Unsplit();
        }
    });
    _viewModel->audioStream().subscribe([this](auto &stream) {
        if (stream) {
            _audioSource = std::make_unique<AudioSource>(stream, false, 1.0f, false, glm::vec3());
            _audioSource->init();
            _audioSource->play();
        } else {
            _audioSource.reset();
        }
    });
    _viewModel->progress().subscribe([this](auto &progress) {
        if (progress.visible) {
            if (!_progressDialog) {
                _progressDialog = new wxProgressDialog("", "", 100, this);
            }
            _progressDialog->SetTitle(progress.title);
            _progressDialog->Update(progress.value, progress.message);
        } else {
            if (_progressDialog) {
                _progressDialog->Destroy();
                _progressDialog = nullptr;
            }
        }
    });
    _viewModel->engineLoadRequested().subscribe([this](auto &requested) {
        if (!requested) {
            return;
        }
        _glCanvas = new wxGLCanvas(_renderSplitter, wxID_ANY, nullptr, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
        _glCanvas->Bind(wxEVT_PAINT, &MainFrame::OnGLCanvasPaint, this);
        _glCanvas->Bind(wxEVT_MOTION, &MainFrame::OnGLCanvasMouseMotion, this);
        _glCanvas->Bind(wxEVT_MOUSEWHEEL, &MainFrame::OnGLCanvasMouseWheel, this);

#if wxCHECK_VERSION(3, 1, 0)
        wxGLContextAttrs glCtxAttrs;
        glCtxAttrs.CoreProfile().OGLVersion(3, 3).EndList();
        auto glContext = new wxGLContext(_glCanvas, nullptr, &glCtxAttrs);
#else
        auto glContext = new wxGLContext(_glCanvas);
#endif
        glContext->SetCurrent(*_glCanvas);

        _animationsListBox = new wxListBox(_renderSplitter, wxID_ANY);
        _animationsListBox->Bind(wxEVT_LISTBOX_DCLICK, &MainFrame::OnAnimationsListBoxDoubleClick, this);
        _renderSplitter->SplitHorizontally(_glCanvas, _animationsListBox, std::numeric_limits<int>::max());
    });
    _viewModel->onViewCreated();

    // CreateStatusBar();
}

void MainFrame::SaveFile() {
}

wxWindow *MainFrame::NewPageWindow(Page &page) {
    switch (page.type) {
    case PageType::Text: {
        auto textPanel = new wxPanel(_notebook);
        auto textSizer = new wxBoxSizer(wxVERTICAL);
        auto plainTextCtrl = new wxTextCtrl(textPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        plainTextCtrl->AppendText(page.textContent);
        plainTextCtrl->SetEditable(false);
        textSizer->Add(plainTextCtrl, 1, wxEXPAND);
        textPanel->SetSizer(textSizer);
        return textPanel;
    }
    case PageType::Table: {
        auto tablePanel = new wxPanel(_notebook);
        auto tableSizer = new wxBoxSizer(wxVERTICAL);
        auto tableCtrl = new wxDataViewListCtrl(tablePanel, wxID_ANY);
        tableCtrl->Freeze();
        for (auto &column : page.tableContent->columns) {
            tableCtrl->AppendTextColumn(column);
        }
        for (auto &row : page.tableContent->rows) {
            auto values = wxVector<wxVariant>();
            for (auto &value : row) {
                values.push_back(wxVariant(value));
            }
            tableCtrl->AppendItem(values);
        }
        tableCtrl->Thaw();
        tableSizer->Add(tableCtrl, 1, wxEXPAND);
        tablePanel->SetSizer(tableSizer);
        return tablePanel;
    }
    case PageType::GFF: {
        auto gffPanel = new wxPanel(_notebook);
        auto gffSizer = new wxBoxSizer(wxVERTICAL);
        auto gffTreeCtrl = new wxDataViewTreeCtrl(gffPanel, wxID_ANY);
        gffTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &MainFrame::OnGffTreeCtrlItemEditingDone, this);
        gffTreeCtrl->Freeze();
        AppendGffStructToTree(*gffTreeCtrl, wxDataViewItem(), "/", *page.gffContent);
        gffTreeCtrl->Thaw();
        gffSizer->Add(gffTreeCtrl, 1, wxEXPAND);
        gffPanel->SetSizer(gffSizer);
        return gffPanel;
    }
    case PageType::NCS: {
        auto pcodePanel = new wxPanel(_notebook);
        auto pcodeSizer = new wxBoxSizer(wxVERTICAL);
        auto pcodeTextCtrl = new wxTextCtrl(pcodePanel, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
        pcodeTextCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        pcodeTextCtrl->AppendText(page.pcodeContent);
        pcodeTextCtrl->SetEditable(false);
        pcodeSizer->Add(pcodeTextCtrl, 1, wxEXPAND);
        pcodePanel->SetSizer(pcodeSizer);
        return pcodePanel;
    }
    case PageType::NSS: {
        auto nssPanel = new wxPanel(_notebook);
        auto nssSizer = new wxBoxSizer(wxVERTICAL);
        auto nssTextCtrl = new wxStyledTextCtrl(nssPanel);
        nssTextCtrl->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        nssTextCtrl->SetLexer(wxSTC_LEX_CPP);
        nssTextCtrl->SetKeyWords(0, "break case continue default do else for if return switch while");
        nssTextCtrl->SetKeyWords(1, "action command const effect event float int itemproperty location object std::string struct talent vector void");
        nssTextCtrl->StyleSetForeground(wxSTC_C_PREPROCESSOR, wxColour(128, 64, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_DEFAULT, wxColour(0, 0, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_WORD, wxColour(0, 0, 255));
        nssTextCtrl->StyleSetForeground(wxSTC_C_WORD2, wxColour(128, 0, 255));
        nssTextCtrl->StyleSetForeground(wxSTC_C_NUMBER, wxColour(255, 128, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_STRING, wxColour(128, 128, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_CHARACTER, wxColour(128, 128, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_OPERATOR, wxColour(0, 0, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_VERBATIM, wxColour(0, 0, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_REGEX, wxColour(0, 0, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENT, wxColour(0, 128, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTLINE, wxColour(0, 128, 0));
        nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTDOC, wxColour(0, 128, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTLINEDOC, wxColour(0, 128, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORD, wxColour(0, 128, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_COMMENTDOCKEYWORDERROR, wxColour(0, 128, 128));
        nssTextCtrl->StyleSetForeground(wxSTC_C_PREPROCESSORCOMMENT, wxColour(0, 128, 0));
        nssTextCtrl->SetText(page.nssContent);
        nssTextCtrl->SetEditable(false);
        nssSizer->Add(nssTextCtrl, 1, wxEXPAND);
        nssPanel->SetSizer(nssSizer);
        return nssPanel;
    }
    default:
        throw std::invalid_argument("Invalid page type: " + std::to_string(static_cast<int>(page.type)));
    }
}

wxWindow *MainFrame::GetStaticPageWindow(PageType type) const {
    switch (type) {
    case PageType::Image:
        return _imageSplitter;
    case PageType::Model:
        return _renderSplitter;
    case PageType::Audio:
        return _audioPanel;
    default:
        return nullptr;
    }
}

void MainFrame::OnClose(wxCloseEvent &event) {
    Destroy();
    _viewModel->onViewDestroyed();
}

void MainFrame::OnIdle(wxIdleEvent &event) {
    if (_viewModel->isRenderEnabled()) {
        _viewModel->update3D();
        _glCanvas->Refresh();
    }
    if (_audioSource) {
        _audioSource->update();
    }
    event.RequestMore();
}

void MainFrame::OnOpenGameDirectoryCommand(wxCommandEvent &event) {
    auto dialog = new wxDirDialog(nullptr, "Choose game directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog->ShowModal() != wxID_OK) {
        return;
    }
    auto gamePath = boost::filesystem::path((std::string)dialog->GetPath());
    auto keyPath = findFileIgnoreCase(gamePath, "chitin.key");
    auto modulesPath = findFileIgnoreCase(gamePath, "modules");
    if (keyPath.empty() || modulesPath.empty()) {
        wxMessageBox("Not a valid game directory", "Error", wxICON_ERROR);
        return;
    }
    _viewModel->onGameDirectoryChanged(gamePath);

    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = KeyReader();
    keyReader.load(key);
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    _filesTreeCtrl->Freeze();
    _filesTreeCtrl->DeleteAllItems();
    int numGameDirItems = _viewModel->getGameDirItemCount();
    for (int i = 0; i < numGameDirItems; ++i) {
        auto &item = _viewModel->getGameDirItem(i);
        void *itemId;
        if (item.container) {
            auto treeItem = _filesTreeCtrl->AppendContainer(wxDataViewItem(), item.displayName);
            itemId = treeItem.GetID();
        } else {
            auto treeItem = _filesTreeCtrl->AppendItem(wxDataViewItem(), item.displayName);
            itemId = treeItem.GetID();
        }
        _viewModel->onGameDirectoryItemIdentified(i, itemId);
    }
    _filesTreeCtrl->Thaw();
}

void MainFrame::OnSaveFileCommand(wxCommandEvent &event) {
    SaveFile();
}

void MainFrame::OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event) {
    auto expandingItemId = event.GetItem().GetID();
    auto &expandingItem = _viewModel->getGameDirItemById(expandingItemId);
    if (expandingItem.loaded) {
        return;
    }
    _viewModel->onGameDirectoryItemExpanding(expandingItemId);
    _filesTreeCtrl->Freeze();
    int numGameDirItems = _viewModel->getGameDirItemCount();
    for (int i = 0; i < numGameDirItems; ++i) {
        auto &item = _viewModel->getGameDirItem(i);
        if (item.id || item.parentId != expandingItemId) {
            continue;
        }
        void *itemId;
        if (item.container) {
            auto treeItem = _filesTreeCtrl->AppendContainer(wxDataViewItem(expandingItemId), item.displayName);
            itemId = treeItem.GetID();
        } else {
            auto treeItem = _filesTreeCtrl->AppendItem(wxDataViewItem(expandingItemId), item.displayName);
            itemId = treeItem.GetID();
        }
        _viewModel->onGameDirectoryItemIdentified(i, itemId);
    }
    _filesTreeCtrl->Thaw();
    expandingItem.loaded = true;
}

void MainFrame::OnFilesTreeCtrlItemActivated(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    _viewModel->onGameDirectoryItemActivated(itemId);
}

void MainFrame::AppendGffStructToTree(wxDataViewTreeCtrl &ctrl, wxDataViewItem parent, const std::string &text, const Gff &gff) {
    auto structItem = ctrl.AppendContainer(parent, str(boost::format("%s [%d]") % text % static_cast<int>(gff.type())));
    for (auto &field : gff.fields()) {
        switch (field.type) {
        case Gff::FieldType::CExoString:
        case Gff::FieldType::ResRef: {
            auto cleaned = boost::replace_all_copy(field.strValue, "\n", "\\n");
            ctrl.AppendItem(structItem, str(boost::format("%s = \"%s\" [%d]") % field.label % cleaned % static_cast<int>(field.type)));
        } break;
        case Gff::FieldType::CExoLocString: {
            auto locStringItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            ctrl.AppendItem(locStringItem, str(boost::format("StrRef = %d") % field.intValue));
            ctrl.AppendItem(locStringItem, str(boost::format("Substring = \"%s\"") % field.strValue));
            if (field.intValue != -1) {
                auto tlkText = _viewModel->getTalkTableText(field.intValue);
                auto cleanedTlkText = boost::replace_all_copy(tlkText, "\n", "\\n");
                ctrl.AppendItem(locStringItem, str(boost::format("TalkTableText = \"%s\"") % cleanedTlkText));
            }
        } break;
        case Gff::FieldType::Void:
            ctrl.AppendItem(structItem, str(boost::format("%s = \"%s\" [%d]") % field.label % hexify(field.data, "") % static_cast<int>(field.type)));
            break;
        case Gff::FieldType::Struct:
            AppendGffStructToTree(ctrl, structItem, field.label, *field.children[0]);
            break;
        case Gff::FieldType::List: {
            auto listItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            for (auto it = field.children.begin(); it != field.children.end(); ++it) {
                auto childIdx = std::distance(field.children.begin(), it);
                AppendGffStructToTree(ctrl, listItem, std::to_string(childIdx), **it);
            }
        } break;
        case Gff::FieldType::Orientation: {
            auto orientationItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            ctrl.AppendItem(orientationItem, str(boost::format("W = %f") % field.quatValue.w));
            ctrl.AppendItem(orientationItem, str(boost::format("X = %f") % field.quatValue.x));
            ctrl.AppendItem(orientationItem, str(boost::format("Y = %f") % field.quatValue.y));
            ctrl.AppendItem(orientationItem, str(boost::format("Z = %f") % field.quatValue.z));
        } break;
        case Gff::FieldType::Vector: {
            auto vectorItem = ctrl.AppendContainer(structItem, str(boost::format("%s [%d]") % field.label % static_cast<int>(field.type)));
            ctrl.AppendItem(vectorItem, str(boost::format("X = %f") % field.vecValue.x));
            ctrl.AppendItem(vectorItem, str(boost::format("Y = %f") % field.vecValue.y));
            ctrl.AppendItem(vectorItem, str(boost::format("Z = %f") % field.vecValue.z));
        } break;
        case Gff::FieldType::StrRef:
            ctrl.AppendItem(structItem, str(boost::format("%s = %d [%d]") % field.label % field.intValue % static_cast<int>(field.type)));
            break;
        default:
            ctrl.AppendItem(structItem, str(boost::format("%s = %s [%d]") % field.label % field.toString() % static_cast<int>(field.type)));
            break;
        }
    }
}

void MainFrame::OnFilesTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    auto &item = _viewModel->getGameDirItemById(itemId);
    if (item.resId) {
        auto menu = wxMenu();
        menu.Append(CommandID::exportFile, "Export...");
        if (item.resId->type == ResourceType::Ncs) {
            menu.Append(CommandID::decompile, "Decompile");
            menu.Append(CommandID::decompileNoOptimize, "Decompile without optimization");
        }
        menu.SetClientData(itemId);
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPopupCommandSelected), nullptr, this);
        PopupMenu(&menu, event.GetPosition());
    } else {
        if (item.archived || !boost::filesystem::is_regular_file(item.path)) {
            return;
        }
        auto extension = item.path.extension().string();
        if (kFilesArchiveExtensions.count(extension) == 0) {
            return;
        }
        auto menu = wxMenu();
        menu.Append(CommandID::extract, "Extract...");
        menu.SetClientData(itemId);
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(MainFrame::OnPopupCommandSelected), nullptr, this);
        PopupMenu(&menu, event.GetPosition());
    }
}

void MainFrame::OnFilesTreeCtrlItemEditingDone(wxDataViewEvent &event) {
    event.Veto();
}

void MainFrame::OnNotebookPageClose(wxAuiNotebookEvent &event) {
    int pageIdx = event.GetSelection();
    auto &page = _viewModel->getPage(pageIdx);
    if (kStaticPageTypes.count(page.type) > 0) {
        _notebook->RemovePage(pageIdx);
    } else {
        _notebook->DeletePage(pageIdx);
    }
    _viewModel->onNotebookPageClose(pageIdx);

    event.Veto();
}

void MainFrame::OnNotebookPageChanged(wxAuiNotebookEvent &event) {
    int pageIdx = event.GetSelection();
    if (pageIdx == -1) {
        return;
    }
    auto &page = _viewModel->getPage(pageIdx);
    if (page.dirty) {
        _saveFileMenuItem->Enable(true);
    } else {
        _saveFileMenuItem->Enable(false);
    }
    event.Skip();
}

void MainFrame::OnGffTreeCtrlItemEditingDone(wxDataViewEvent &event) {
    event.Veto();
}

void MainFrame::OnPopupCommandSelected(wxCommandEvent &event) {
    auto menu = static_cast<wxMenu *>(event.GetEventObject());

    if (event.GetId() == CommandID::extract) {
        auto itemId = menu->GetClientData();
        auto &item = _viewModel->getGameDirItemById(itemId);

        auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = boost::filesystem::path(std::string(dialog->GetPath()));

        _viewModel->extractArchive(item.path, destPath);
        wxMessageBox("Operation completed successfully", "Success");

    } else if (event.GetId() == CommandID::decompile) {
        auto itemId = menu->GetClientData();
        _viewModel->decompile(itemId, true);

    } else if (event.GetId() == CommandID::decompileNoOptimize) {
        auto itemId = menu->GetClientData();
        _viewModel->decompile(itemId, false);

    } else if (event.GetId() == CommandID::exportFile) {
        auto itemId = menu->GetClientData();
        auto dialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = boost::filesystem::path(std::string(dialog->GetPath()));
        _viewModel->exportFile(itemId, destPath);
        wxMessageBox("Operation completed successfully", "Success");
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

    auto clientSize = _glCanvas->GetClientSize();
    _viewModel->render3D(clientSize.x, clientSize.y);

    _glCanvas->SwapBuffers();
}

void MainFrame::OnGLCanvasMouseWheel(wxMouseEvent &event) {
    auto delta = event.GetWheelDelta() * event.GetWheelRotation();
    _viewModel->onGLCanvasMouseWheel(delta);
}

void MainFrame::OnGLCanvasMouseMotion(wxMouseEvent &event) {
    wxClientDC dc(_glCanvas);
    auto position = event.GetLogicalPosition(dc);
    _viewModel->onGLCanvasMouseMotion(position.x, position.y, event.LeftIsDown(), event.RightIsDown());
}

void MainFrame::OnAnimationsListBoxDoubleClick(wxCommandEvent &event) {
    int selection = event.GetSelection();
    if (selection == -1) {
        return;
    }
    auto animation = _animationsListBox->GetString(selection);
    _viewModel->playAnimation((std::string)animation);
}

void MainFrame::OnStopAudioCommand(wxCommandEvent &event) {
    if (_audioSource) {
        _audioSource->stop();
        _audioSource.reset();
    }
}

void MainFrame::OnExtractAllBifsCommand(wxCommandEvent &event) {
    if (_viewModel->gamePath().empty()) {
        wxMessageBox("Game directory must be open", "Error", wxICON_ERROR);
        return;
    }
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = boost::filesystem::path((std::string)destDirDialog->GetPath());
    _viewModel->extractAllBifs(destPath);
    wxMessageBox("Operation completed successfully", "Success");
}

void MainFrame::OnBatchConvertTpcToTgaCommand(wxCommandEvent &event) {
    auto srcDirDialog = new wxDirDialog(nullptr, "Choose source directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (srcDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto srcPath = boost::filesystem::path((std::string)srcDirDialog->GetPath());
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = boost::filesystem::path((std::string)destDirDialog->GetPath());
    _viewModel->batchConvertTpcToTga(srcPath, destPath);
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
    boost::filesystem::path srcPath;
    switch (operation) {
    case Operation::ToERF:
    case Operation::ToRIM:
    case Operation::ToMOD: {
        auto srcDirDialog = new wxDirDialog(
            nullptr,
            "Choose source directory",
            "",
            wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (srcDirDialog->ShowModal() != wxID_OK) {
            return;
        }
        srcPath = (std::string)srcDirDialog->GetPath();
    } break;
    default: {
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
        srcPath = (std::string)srcFileDialog->GetPath();
    } break;
    }
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = boost::filesystem::path((std::string)destDirDialog->GetPath());
    if (_viewModel->invokeTool(operation, srcPath, destPath)) {
        wxMessageBox("Operation completed successfully", "Success");
    } else {
        wxMessageBox("Tool not found", "Error", wxICON_ERROR);
    }
}

wxBEGIN_EVENT_TABLE(MainFrame, wxFrame)
    EVT_CLOSE(MainFrame::OnClose)                                                     //
    EVT_IDLE(MainFrame::OnIdle)                                                       //
    EVT_MENU(EventHandlerID::openGameDir, MainFrame::OnOpenGameDirectoryCommand)      //
    EVT_MENU(EventHandlerID::saveFile, MainFrame::OnSaveFileCommand)                  //
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
    wxEND_EVENT_TABLE()

} // namespace reone
