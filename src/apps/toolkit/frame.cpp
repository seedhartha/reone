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

#include "frame.h"

#include <wx/dirdlg.h>
#include <wx/notebook.h>
#include <wx/stc/stc.h>

#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/rimreader.h"
#include "reone/resource/typeutil.h"
#include "reone/system/pathutil.h"
#include "reone/system/stream/bytearrayinput.h"
#include "reone/system/stream/bytearrayoutput.h"
#include "reone/system/stream/fileinput.h"
#include "reone/tools/2da.h"
#include "reone/tools/erf.h"
#include "reone/tools/gff.h"
#include "reone/tools/keybif.h"
#include "reone/tools/rim.h"
#include "reone/tools/tlk.h"

using namespace std;

using namespace reone::resource;

namespace reone {

static constexpr char kIconName[] = "reone";
static constexpr int kMinPanelWidth = 640;

static const set<string> kFilesSubdirectoryWhitelist {
    "data", "lips", "modules", "movies", "override", "rims", "saves", "texturepacks", //
    "streammusic", "streamsounds", "streamwaves", "streamvoice",                      //
};

static const set<string> kFilesArchiveWhitelist {
    ".bif", ".erf", ".sav", ".rim", ".mod" //
};

static const set<string> kFilesRegularBlacklist {
    ".lnk", ".bat", ".exe", ".dll", ".ini", ".ico",                    //
    ".zip", ".pdf",                                                    //
    ".hashdb", ".info", ".script", ".dat", ".msg", ".sdb", ".ds_store" //
};

struct EventHandlerID {
    static constexpr int openGameDir = wxID_HIGHEST + 1;
};

struct CommandID {
    static constexpr int extract = 1;
};

ToolkitFrame::ToolkitFrame() :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(1024, 768));
    Maximize();

    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openGameDir, "&Open game directory...");
    auto menuBar = new wxMenuBar();
    menuBar->Append(fileMenu, "&File");
    SetMenuBar(menuBar);

    _splitter = new wxSplitterWindow(this, wxID_ANY);
    _splitter->Bind(wxEVT_SIZE, &ToolkitFrame::OnSplitterSize, this);
    _splitter->Bind(wxEVT_SPLITTER_SASH_POS_CHANGING, &ToolkitFrame::OnSplitterSashPosChanging, this);
    _splitter->SetMinimumPaneSize(300);

    auto dataSplitter = new wxSplitterWindow(_splitter, wxID_ANY);
    dataSplitter->SetMinimumPaneSize(300);

    auto filesPanel = new wxPanel(dataSplitter);
    _filesTreeCtrl = new wxDataViewTreeCtrl(filesPanel, wxID_ANY);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &ToolkitFrame::OnFilesTreeCtrlItemExpanding, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &ToolkitFrame::OnFilesTreeCtrlItemContextMenu, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &ToolkitFrame::OnFilesTreeCtrlItemActivated, this);
    _filesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &ToolkitFrame::OnFilesTreeCtrlItemEditingDone, this);
    auto filesSizer = new wxStaticBoxSizer(wxVERTICAL, filesPanel, "Files");
    filesSizer->Add(_filesTreeCtrl, 1, wxEXPAND);
    filesPanel->SetSizer(filesSizer);

    auto modulesPanel = new wxPanel(dataSplitter);
    _modulesListBox = new wxListBox(modulesPanel, wxID_ANY);
    auto modulesSizer = new wxStaticBoxSizer(wxVERTICAL, modulesPanel, "Modules");
    modulesSizer->Add(_modulesListBox, 1, wxEXPAND);
    modulesPanel->SetSizer(modulesSizer);

    dataSplitter->SetSashGravity(0.5);
    dataSplitter->SplitHorizontally(filesPanel, modulesPanel);

    auto notebook = new wxNotebook(_splitter, wxID_ANY);

    auto xmlPanel = new wxPanel(notebook);
    auto xmlSizer = new wxBoxSizer(wxVERTICAL);
    _xmlTextCtrl = new wxStyledTextCtrl(xmlPanel);
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
    xmlPanel->SetSizer(xmlSizer);
    notebook->InsertPage(0, xmlPanel, "XML");

    auto renderPanel = new wxPanel(notebook);
    auto renderSizer = new wxBoxSizer(wxVERTICAL);
    auto glAttributes = wxGLAttributes().Defaults();
    glAttributes.EndList();
    //_glCanvas = new wxGLCanvas(renderPanel, glAttributes, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxFULL_REPAINT_ON_RESIZE);
    //_glCanvas->Bind(wxEVT_PAINT, &ToolkitFrame::OnGLCanvasPaint, this);
    // auto glContext = new wxGLContext(_glCanvas);
    // glContext->SetCurrent(*_glCanvas);
    // renderSizer->Add(_glCanvas, 1, wxEXPAND);
    renderPanel->SetSizer(renderSizer);
    notebook->InsertPage(1, renderPanel, "3D");

    _splitter->SplitVertically(dataSplitter, notebook);

    CreateStatusBar();
}

void ToolkitFrame::OnOpenGameDirectoryMenu(wxCommandEvent &event) {
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
        if ((file.status().type() == boost::filesystem::directory_file && kFilesSubdirectoryWhitelist.count(filename) > 0) || kFilesArchiveWhitelist.count(filename) > 0) {
            auto item = _filesTreeCtrl->AppendContainer(wxDataViewItem(), filename);
            itemId = item.GetID();
        } else if (file.status().type() == boost::filesystem::regular_file && (kFilesRegularBlacklist.count(extension) == 0 && extension != ".txt")) {
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
}

void ToolkitFrame::OnSplitterSize(wxSizeEvent &event) {
    int requestedPanelSize = event.GetSize().x - _splitter->GetSashSize() - _splitter->GetSashPosition();
    if (requestedPanelSize < kMinPanelWidth) {
        _splitter->SetSashPosition(event.GetSize().x - _splitter->GetSashSize() - kMinPanelWidth);
    }
    event.Skip();
}

void ToolkitFrame::OnSplitterSashPosChanging(wxSplitterEvent &event) {
    int requestedPanelSize = _splitter->GetSize().x - _splitter->GetSashSize() - event.GetSashPosition();
    if (requestedPanelSize < kMinPanelWidth) {
        event.SetSashPosition(_splitter->GetSize().x - _splitter->GetSashSize() - kMinPanelWidth);
    }
}

void ToolkitFrame::OnFilesTreeCtrlItemExpanding(wxDataViewEvent &event) {
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
            if (file.status().type() == boost::filesystem::directory_file || kFilesArchiveWhitelist.count(extension) > 0) {
                auto item = _filesTreeCtrl->AppendContainer(event.GetItem(), filename);
                itemId = item.GetID();
            } else if (file.status().type() == boost::filesystem::regular_file && kFilesRegularBlacklist.count(extension) == 0) {
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

void ToolkitFrame::OnFilesTreeCtrlItemActivated(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    if (_files.count(itemId) == 0) {
        return;
    }
    auto &item = _files.at(itemId);
    OpenFile(item);
}

void ToolkitFrame::OnFilesTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    if (_files.count(itemId) == 0) {
        return;
    }
    auto &item = _files.at(itemId);
    auto extension = boost::to_lower_copy(item.path.extension().string());
    if (!boost::filesystem::is_regular_file(item.path) || kFilesArchiveWhitelist.count(extension) == 0) {
        return;
    }
    auto menu = wxMenu();
    menu.Append(CommandID::extract, "Extract...");
    menu.SetClientData(&item);
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ToolkitFrame::OnPopupCommandSelected), nullptr, this);
    PopupMenu(&menu, event.GetPosition());
}

void ToolkitFrame::OpenFile(FilesEntry &entry) {
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

void ToolkitFrame::OpenResource(ResourceId &id, IInputStream &data) {
    if (id.type == ResourceType::TwoDa) {
        auto xmlBytes = ByteArray();
        auto xml = ByteArrayOutputStream(xmlBytes);
        TwoDaTool().toXML(data, xml);
        _xmlTextCtrl->SetEditable(true);
        _xmlTextCtrl->SetText(xmlBytes);
        _xmlTextCtrl->SetEditable(false);
    } else if (isGFFCompatibleResType(id.type)) {
        auto xmlBytes = ByteArray();
        auto xml = ByteArrayOutputStream(xmlBytes);
        GffTool().toXML(data, xml, _strings);
        _xmlTextCtrl->SetEditable(true);
        _xmlTextCtrl->SetText(xmlBytes);
        _xmlTextCtrl->SetEditable(false);
    } else if (id.type == ResourceType::Tlk) {
        auto xmlBytes = ByteArray();
        auto xml = ByteArrayOutputStream(xmlBytes);
        TlkTool().toXML(data, xml);
        _xmlTextCtrl->SetEditable(true);
        _xmlTextCtrl->SetText(xmlBytes);
        _xmlTextCtrl->SetEditable(false);
    }
}

void ToolkitFrame::OnFilesTreeCtrlItemEditingDone(wxDataViewEvent &event) {
    event.Veto();
}

void ToolkitFrame::OnPopupCommandSelected(wxCommandEvent &event) {
    if (event.GetId() == CommandID::extract) {
        auto menu = static_cast<wxMenu *>(event.GetEventObject());
        auto data = menu->GetClientData();
        auto entry = reinterpret_cast<FilesEntry *>(data);
        if (entry->archived) {
            // TODO: extract individual files
        } else {
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
}

void ToolkitFrame::OnGLCanvasPaint(wxPaintEvent &event) {
    wxPaintDC dc(_glCanvas);

    glViewport(0, 0, _glCanvas->GetClientSize().x, _glCanvas->GetClientSize().y);

    _glCanvas->ClearBackground();
    _glCanvas->SwapBuffers();
}

wxBEGIN_EVENT_TABLE(ToolkitFrame, wxFrame)                                       //
    EVT_MENU(EventHandlerID::openGameDir, ToolkitFrame::OnOpenGameDirectoryMenu) //
    wxEND_EVENT_TABLE()

} // namespace reone
