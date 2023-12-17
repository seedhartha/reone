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

#include "explorerframe.h"

#include <wx/choicdlg.h>
#include <wx/dirdlg.h>
#include <wx/mstream.h>

#include "reone/audio/clip.h"
#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/game/script/routines.h"
#include "reone/graphics/format/lipreader.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/bifreader.h"
#include "reone/resource/format/erfreader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/rimreader.h"
#include "reone/resource/format/ssfreader.h"
#include "reone/resource/format/tlkreader.h"
#include "reone/resource/talktable.h"
#include "reone/resource/typeutil.h"
#include "reone/system/fileutil.h"
#include "reone/system/hexutil.h"
#include "reone/system/stream/fileinput.h"
#include "reone/system/stream/fileoutput.h"
#include "reone/system/stream/memoryinput.h"
#include "reone/system/stream/memoryoutput.h"
#include "reone/tools/legacy/2da.h"
#include "reone/tools/legacy/audio.h"
#include "reone/tools/legacy/erf.h"
#include "reone/tools/legacy/gff.h"
#include "reone/tools/legacy/keybif.h"
#include "reone/tools/legacy/lip.h"
#include "reone/tools/legacy/ncs.h"
#include "reone/tools/legacy/rim.h"
#include "reone/tools/legacy/ssf.h"
#include "reone/tools/legacy/tlk.h"
#include "reone/tools/legacy/tpc.h"

#include "../../viewmodel/resource/gff.h"
#include "../../viewmodel/resource/ncs.h"
#include "../../viewmodel/resource/nss.h"
#include "../../viewmodel/resource/table.h"
#include "../../viewmodel/resource/text.h"

#include "../tool/composelipdialog.h"

#include "audiopanel.h"
#include "gffpanel.h"
#include "imagepanel.h"
#include "modelpanel.h"
#include "ncspanel.h"
#include "nsspanel.h"
#include "tablepanel.h"
#include "textpanel.h"

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace reone {

static constexpr char kIconName[] = "toolkit";

static const std::set<std::string> kFilesArchiveExtensions {".bif", ".erf", ".sav", ".rim", ".mod"};

static const std::set<PageType> kStaticPageTypes {
    PageType::Image,
    PageType::Model,
    PageType::Audio};

struct EventHandlerID {
    static constexpr int openDir = wxID_HIGHEST + 1;
    static constexpr int extractAllBifs = wxID_HIGHEST + 2;
    static constexpr int batchTpcToTga = wxID_HIGHEST + 3;
    static constexpr int composeLip = wxID_HIGHEST + 4;
    static constexpr int extractTool = wxID_HIGHEST + 5;
    static constexpr int unwrapTool = wxID_HIGHEST + 6;
    static constexpr int toRimTool = wxID_HIGHEST + 7;
    static constexpr int toErfTool = wxID_HIGHEST + 8;
    static constexpr int toModTool = wxID_HIGHEST + 9;
    static constexpr int toXmlTool = wxID_HIGHEST + 10;
    static constexpr int toTwoDaTool = wxID_HIGHEST + 11;
    static constexpr int toGffTool = wxID_HIGHEST + 12;
    static constexpr int toTlkTool = wxID_HIGHEST + 13;
    static constexpr int toLipTool = wxID_HIGHEST + 14;
    static constexpr int toSsfTool = wxID_HIGHEST + 15;
    static constexpr int toTgaTool = wxID_HIGHEST + 16;
    static constexpr int toPcodeTool = wxID_HIGHEST + 17;
    static constexpr int toNcsTool = wxID_HIGHEST + 18;
    static constexpr int toNssTool = wxID_HIGHEST + 19;
    static constexpr int saveFile = wxID_HIGHEST + 20;
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

ResourceExplorerFrame::ResourceExplorerFrame(ResourceExplorerViewModel &viewModel) :
    wxFrame(nullptr, wxID_ANY, "reone toolkit", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE),
    m_viewModel(viewModel) {

#ifdef _WIN32
    SetIcon(wxIcon(kIconName));
#endif
    SetMinClientSize(wxSize(1024, 768));
    Maximize();

    InitControls();
    InitMenu();
    BindEvents();
    BindViewModel();

    m_viewModel.onViewCreated();
}

void ResourceExplorerFrame::InitControls() {
    m_splitter = new wxSplitterWindow(this, wxID_ANY);
    m_splitter->SetMinimumPaneSize(300);

    auto resourcesPanel = new wxPanel(m_splitter);

    m_resourcesTreeCtrl = new wxDataViewTreeCtrl(resourcesPanel, wxID_ANY);
    m_resourcesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_EXPANDING, &ResourceExplorerFrame::OnResourcesTreeCtrlItemExpanding, this);
    m_resourcesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &ResourceExplorerFrame::OnResourcesTreeCtrlItemContextMenu, this);
    m_resourcesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &ResourceExplorerFrame::OnResourcesTreeCtrlItemActivated, this);
    m_resourcesTreeCtrl->Bind(wxEVT_DATAVIEW_ITEM_START_EDITING, &ResourceExplorerFrame::OnResourcesTreeCtrlItemStartEditing, this);

    auto resourcesSizer = new wxStaticBoxSizer(wxVERTICAL, resourcesPanel, "Resources");
    resourcesSizer->Add(m_resourcesTreeCtrl, 1, wxEXPAND);
    resourcesPanel->SetSizer(resourcesSizer);

    m_notebook = new wxAuiNotebook(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE & ~(wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE));
    m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &ResourceExplorerFrame::OnNotebookPageClose, this);
    m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &ResourceExplorerFrame::OnNotebookPageChanged, this);

    m_imagePanel = new ImageResourcePanel(m_viewModel.imageResViewModel(), m_notebook);
    m_modelPanel = new ModelResourcePanel(m_notebook);
    m_audioPanel = new AudioResourcePanel(m_notebook);

    m_splitter->SplitVertically(resourcesPanel, m_notebook, 1);

    for (auto &page : kStaticPageTypes) {
        auto window = GetStaticPageWindow(page);
        window->Hide();
    }
}

void ResourceExplorerFrame::InitMenu() {
    auto fileMenu = new wxMenu();
    fileMenu->Append(EventHandlerID::openDir, "&Open directory...");
    fileMenu->AppendSeparator();

    m_saveFileMenuItem = fileMenu->Append(EventHandlerID::saveFile, "&Save copy as...");
    m_saveFileMenuItem->Enable(false);

    auto toolsMenu = new wxMenu();
    toolsMenu->Append(EventHandlerID::extractAllBifs, "Extract all BIF archives...");
    toolsMenu->Append(EventHandlerID::batchTpcToTga, "Batch convert TPC to TGA/TXI...");
    toolsMenu->Append(EventHandlerID::composeLip, "Compose LIP...");
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
}

void ResourceExplorerFrame::BindEvents() {
    Bind(wxEVT_CLOSE_WINDOW, &ResourceExplorerFrame::OnClose, this);
    Bind(wxEVT_IDLE, &ResourceExplorerFrame::OnIdle, this);
    Bind(wxEVT_MENU, &ResourceExplorerFrame::OnOpenDirectoryCommand, this, EventHandlerID::openDir);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::SaveFile, this), EventHandlerID::saveFile);
    Bind(wxEVT_MENU, &ResourceExplorerFrame::OnExtractAllBifsCommand, this, EventHandlerID::extractAllBifs);
    Bind(wxEVT_MENU, &ResourceExplorerFrame::OnBatchConvertTpcToTgaCommand, this, EventHandlerID::batchTpcToTga);
    Bind(wxEVT_MENU, &ResourceExplorerFrame::OnComposeLipCommand, this, EventHandlerID::composeLip);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::Extract), EventHandlerID::extractTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::Unwrap), EventHandlerID::unwrapTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToRIM), EventHandlerID::toRimTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToERF), EventHandlerID::toErfTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToMOD), EventHandlerID::toModTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToXML), EventHandlerID::toXmlTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::To2DA), EventHandlerID::toTwoDaTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToGFF), EventHandlerID::toGffTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToTLK), EventHandlerID::toTlkTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToLIP), EventHandlerID::toLipTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToSSF), EventHandlerID::toSsfTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToTGA), EventHandlerID::toTgaTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToPCODE), EventHandlerID::toPcodeTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToNCS), EventHandlerID::toNcsTool);
    Bind(wxEVT_MENU, std::bind(&ResourceExplorerFrame::InvokeTool, this, Operation::ToNSS), EventHandlerID::toNssTool);
}

void ResourceExplorerFrame::BindViewModel() {
    m_viewModel.pages().addChangedHandler([this](const auto &args) {
        switch (args.type) {
        case CollectionChangeType::Add: {
            auto &page = args.addedItem->get();
            wxWindow *window;
            if (kStaticPageTypes.count(page->type) > 0) {
                window = GetStaticPageWindow(page->type);
            } else {
                window = NewPageWindow(*page);
            }
            window->Show();
            m_notebook->AddPage(window, page->displayName, true);
        } break;
        case CollectionChangeType::Remove: {
            auto &page = args.removedItem->get();
            if (kStaticPageTypes.count(page->type) > 0) {
                auto window = GetStaticPageWindow(page->type);
                window->Hide();
                m_notebook->RemovePage(*args.removedItemIdx);
            } else {
                m_notebook->DeletePage(*args.removedItemIdx);
            }
        } break;
        default:
            throw std::logic_error("Unsupported collection state change type");
        }
    });
    m_viewModel.selectedPage().addChangedHandler([this](const auto &page) {
        m_notebook->SetSelection(page);
    });
    m_viewModel.progress().addChangedHandler([this](const auto &progress) {
        if (progress.visible) {
            if (!m_progressDialog) {
                m_progressDialog = new wxProgressDialog("", "", 100, this);
            }
            m_progressDialog->SetTitle(progress.title);
            m_progressDialog->Update(progress.value, progress.message);
        } else {
            if (m_progressDialog) {
                m_progressDialog->Destroy();
                m_progressDialog = nullptr;
            }
        }
    });
    m_viewModel.engineLoadRequested().addChangedHandler([this](const auto &requested) {
        if (!requested) {
            return;
        }
        m_modelPanel->SetViewModel(m_viewModel.modelResViewModel());
        m_modelPanel->OnEngineLoadRequested();
        m_audioPanel->SetViewModel(m_viewModel.audioResViewModel());
        m_audioPanel->OnEngineLoadRequested();
    });
    m_viewModel.renderEnabled().addChangedHandler([this](const auto &enabled) {
        if (enabled) {
            wxWakeUpIdle();
        }
    });
}

void ResourceExplorerFrame::SaveFile() {
    auto pageIdx = m_notebook->GetSelection();
    checkGreaterOrEqual("pageIdx", pageIdx, 0);
    checkLess("pageIdx", static_cast<size_t>(pageIdx), m_viewModel.pages()->size());

    auto &page = m_viewModel.pages().at(pageIdx);
    checkThat(page->dirty, "Page must have dirty flag set");

    auto filename = page->resourceId.string();
    auto &ext = getExtByResType(page->resourceId.type);
    auto destFileDialog = wxFileDialog(
        this,
        "Choose destination file",
        wxEmptyString,
        filename,
        str(boost::format("*.%1%") % ext),
        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    if (destFileDialog.ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path(destFileDialog.GetPath().ToStdString());
    m_viewModel.saveFileCommand().execute(*page, destPath);
}

wxWindow *ResourceExplorerFrame::NewPageWindow(Page &page) {
    switch (page.type) {
    case PageType::Text: {
        auto &viewModel = *std::static_pointer_cast<TextResourceViewModel>(page.viewModel);
        viewModel.modified().addChangedHandler([this, &page](const auto &modified) {
            for (size_t i = 0; i < m_viewModel.pages()->size(); ++i) {
                const auto &p = m_viewModel.pages().at(i);
                if (p->resourceId != page.resourceId) {
                    continue;
                }
                m_notebook->SetPageText(i, str(boost::format("*%1%") % page.displayName));
                m_saveFileMenuItem->Enable(m_notebook->GetSelection() == i);
                break;
            }
            page.dirty = true;
        });
        return new TextResourcePanel {viewModel, m_notebook};
    }
    case PageType::Table: {
        auto &viewModel = *std::static_pointer_cast<TableResourceViewModel>(page.viewModel);
        viewModel.modified().addChangedHandler([this, &page](const auto &modified) {
            for (size_t i = 0; i < m_viewModel.pages()->size(); ++i) {
                const auto &p = m_viewModel.pages().at(i);
                if (p->resourceId != page.resourceId) {
                    continue;
                }
                m_notebook->SetPageText(i, str(boost::format("*%1%") % page.displayName));
                m_saveFileMenuItem->Enable(m_notebook->GetSelection() == i);
                break;
            }
            page.dirty = true;
        });
        return new TableResourcePanel {viewModel, m_notebook};
    }
    case PageType::GFF: {
        auto &viewModel = *std::static_pointer_cast<GFFResourceViewModel>(page.viewModel);
        viewModel.modified().addChangedHandler([this, &page](const auto &modified) {
            for (size_t i = 0; i < m_viewModel.pages()->size(); ++i) {
                const auto &p = m_viewModel.pages().at(i);
                if (p->resourceId != page.resourceId) {
                    continue;
                }
                m_notebook->SetPageText(i, str(boost::format("*%1%") % page.displayName));
                m_saveFileMenuItem->Enable(m_notebook->GetSelection() == i);
                break;
            }
            page.dirty = true;
        });
        return new GFFResourcePanel {viewModel, m_viewModel.talkTable(), m_notebook};
    }
    case PageType::NCS: {
        auto &viewModel = *std::static_pointer_cast<NCSResourceViewModel>(page.viewModel);
        viewModel.modified().addChangedHandler([this, &page](const auto &modified) {
            for (size_t i = 0; i < m_viewModel.pages()->size(); ++i) {
                const auto &p = m_viewModel.pages().at(i);
                if (p->resourceId != page.resourceId) {
                    continue;
                }
                m_notebook->SetPageText(i, str(boost::format("*%1%") % page.displayName));
                m_saveFileMenuItem->Enable(m_notebook->GetSelection() == i);
                break;
            }
            page.dirty = true;
        });
        return new NCSResourcePanel {m_viewModel.gameId(), viewModel, m_notebook};
    }
    case PageType::NSS:
        return new NSSResourcePanel {*std::static_pointer_cast<NSSResourceViewModel>(page.viewModel), m_notebook};
    default:
        throw std::invalid_argument {"Invalid page type: " + std::to_string(static_cast<int>(page.type))};
    }
}

wxWindow *ResourceExplorerFrame::GetStaticPageWindow(PageType type) const {
    switch (type) {
    case PageType::Image:
        return m_imagePanel;
    case PageType::Model:
        return m_modelPanel;
    case PageType::Audio:
        return m_audioPanel;
    default:
        return nullptr;
    }
}

void ResourceExplorerFrame::OnClose(wxCloseEvent &event) {
    Destroy();
    m_viewModel.onViewDestroyed();
}

void ResourceExplorerFrame::OnIdle(wxIdleEvent &event) {
    bool renderEnabled = *m_viewModel.renderEnabled();
    if (renderEnabled) {
        m_viewModel.modelResViewModel().update3D();
        m_modelPanel->RefreshGL();
    }
    bool hasAudio = m_audioPanel->HasAudioSource();
    if (hasAudio) {
        m_audioPanel->UpdateAudioSource();
    }
    if (renderEnabled || hasAudio) {
        event.RequestMore();
    }
}

void ResourceExplorerFrame::OnOpenDirectoryCommand(wxCommandEvent &event) {
    auto dialog = new wxDirDialog(nullptr, "Choose directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (dialog->ShowModal() != wxID_OK) {
        return;
    }
    GameID gameId {GameID::KotOR};
    auto resourcesPath = std::filesystem::path {dialog->GetPath().ToStdString()};
    auto keyPath = findFileIgnoreCase(resourcesPath, "chitin.key");
    if (keyPath) {
        auto tslExePath = findFileIgnoreCase(resourcesPath, "swkotor2.exe");
        gameId = tslExePath ? GameID::TSL : GameID::KotOR;
    } else {
        const char *choices[] = {
            "KotOR", //
            "TSL"    //
        };
        wxSingleChoiceDialog dialog {
            nullptr,
            "Not a game directory. Choose game manually:",
            "Choose game",
            wxArrayString {2, choices}};
        if (dialog.ShowModal() == wxID_OK) {
            gameId = static_cast<GameID>(dialog.GetSelection());
        }
    }
    m_viewModel.onResourcesDirectoryChanged(gameId, resourcesPath);

    m_resourcesTreeCtrl->Freeze();
    m_resourcesTreeCtrl->DeleteAllItems();
    int numGameDirItems = m_viewModel.getNumResourcesItems();
    for (int i = 0; i < numGameDirItems; ++i) {
        auto &item = m_viewModel.getResourcesItem(i);
        void *itemId;
        if (item.container) {
            auto treeItem = m_resourcesTreeCtrl->AppendContainer(wxDataViewItem(), item.displayName);
            itemId = treeItem.GetID();
        } else {
            auto treeItem = m_resourcesTreeCtrl->AppendItem(wxDataViewItem(), item.displayName);
            itemId = treeItem.GetID();
        }
        m_viewModel.onResourcesItemIdentified(i, itemId);
    }
    m_resourcesTreeCtrl->Thaw();
}

void ResourceExplorerFrame::OnResourcesTreeCtrlItemExpanding(wxDataViewEvent &event) {
    auto expandingItemId = event.GetItem().GetID();
    auto &expandingItem = m_viewModel.getResourcesItemById(expandingItemId);
    if (expandingItem.loaded) {
        return;
    }
    m_viewModel.onResourcesItemExpanding(expandingItemId);
    m_resourcesTreeCtrl->Freeze();
    int numGameDirItems = m_viewModel.getNumResourcesItems();
    for (int i = 0; i < numGameDirItems; ++i) {
        auto &item = m_viewModel.getResourcesItem(i);
        if (item.id || item.parentId != expandingItemId) {
            continue;
        }
        void *itemId;
        if (item.container) {
            auto treeItem = m_resourcesTreeCtrl->AppendContainer(wxDataViewItem(expandingItemId), item.displayName);
            itemId = treeItem.GetID();
        } else {
            auto treeItem = m_resourcesTreeCtrl->AppendItem(wxDataViewItem(expandingItemId), item.displayName);
            itemId = treeItem.GetID();
        }
        m_viewModel.onResourcesItemIdentified(i, itemId);
    }
    m_resourcesTreeCtrl->Thaw();
    expandingItem.loaded = true;
}

void ResourceExplorerFrame::OnResourcesTreeCtrlItemActivated(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    m_viewModel.onResourcesItemActivated(itemId);
}

void ResourceExplorerFrame::OnResourcesTreeCtrlItemContextMenu(wxDataViewEvent &event) {
    auto itemId = event.GetItem().GetID();
    auto &item = m_viewModel.getResourcesItemById(itemId);
    if (item.resId) {
        auto menu = wxMenu();
        menu.Append(CommandID::exportFile, "Export...");
        if (item.resId->type == ResType::Ncs) {
            menu.Append(CommandID::decompile, "Decompile");
            menu.Append(CommandID::decompileNoOptimize, "Decompile without optimization");
        }
        menu.SetClientData(itemId);
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ResourceExplorerFrame::OnPopupCommandSelected), nullptr, this);
        PopupMenu(&menu, event.GetPosition());
    } else {
        if (item.archived || !std::filesystem::is_regular_file(item.path)) {
            return;
        }
        auto extension = item.path.extension().string();
        if (kFilesArchiveExtensions.count(extension) == 0) {
            return;
        }
        auto menu = wxMenu();
        menu.Append(CommandID::extract, "Extract...");
        menu.SetClientData(itemId);
        menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ResourceExplorerFrame::OnPopupCommandSelected), nullptr, this);
        PopupMenu(&menu, event.GetPosition());
    }
}

void ResourceExplorerFrame::OnResourcesTreeCtrlItemStartEditing(wxDataViewEvent &event) {
    event.Veto();
}

void ResourceExplorerFrame::OnNotebookPageClose(wxAuiNotebookEvent &event) {
    int pageIdx = event.GetSelection();
    m_viewModel.onNotebookPageClose(pageIdx);
    event.Veto();
}

void ResourceExplorerFrame::OnNotebookPageChanged(wxAuiNotebookEvent &event) {
    int pageIdx = event.GetSelection();
    if (pageIdx == -1) {
        return;
    }
    auto &page = m_viewModel.getPage(pageIdx);
    if (page.dirty) {
        m_saveFileMenuItem->Enable(true);
    } else {
        m_saveFileMenuItem->Enable(false);
    }
    event.Skip();
}

void ResourceExplorerFrame::OnPopupCommandSelected(wxCommandEvent &event) {
    auto menu = static_cast<wxMenu *>(event.GetEventObject());

    if (event.GetId() == CommandID::extract) {
        auto itemId = menu->GetClientData();
        auto &item = m_viewModel.getResourcesItemById(itemId);

        auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));

        m_viewModel.extractArchive(item.path, destPath);
        wxMessageBox("Operation completed successfully", "Success");

    } else if (event.GetId() == CommandID::decompile) {
        auto itemId = menu->GetClientData();
        m_viewModel.decompile(itemId, true);

    } else if (event.GetId() == CommandID::decompileNoOptimize) {
        auto itemId = menu->GetClientData();
        m_viewModel.decompile(itemId, false);

    } else if (event.GetId() == CommandID::exportFile) {
        auto itemId = menu->GetClientData();
        auto dialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));
        m_viewModel.exportFile(itemId, destPath);
        wxMessageBox("Operation completed successfully", "Success");
    }
}

void ResourceExplorerFrame::OnExtractAllBifsCommand(wxCommandEvent &event) {
    if (m_viewModel.gamePath().empty()) {
        wxMessageBox("Game directory must be open", "Error", wxICON_ERROR);
        return;
    }
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path((std::string)destDirDialog->GetPath());
    m_viewModel.extractAllBifs(destPath);
    wxMessageBox("Operation completed successfully", "Success");
}

void ResourceExplorerFrame::OnBatchConvertTpcToTgaCommand(wxCommandEvent &event) {
    auto srcDirDialog = new wxDirDialog(nullptr, "Choose source directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (srcDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto srcPath = std::filesystem::path((std::string)srcDirDialog->GetPath());
    auto destDirDialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
    if (destDirDialog->ShowModal() != wxID_OK) {
        return;
    }
    auto destPath = std::filesystem::path((std::string)destDirDialog->GetPath());
    m_viewModel.batchConvertTpcToTga(srcPath, destPath);
    wxMessageBox("Operation completed successfully", "Success");
}

void ResourceExplorerFrame::OnComposeLipCommand(wxCommandEvent &event) {
    auto dialog = ComposeLipDialog(this, -1, "LIP Composer");
    if (dialog.ShowModal() != wxID_OK) {
    }
}

void ResourceExplorerFrame::InvokeTool(Operation operation) {
    std::filesystem::path srcPath;
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
    auto destPath = std::filesystem::path((std::string)destDirDialog->GetPath());
    if (m_viewModel.invokeTool(operation, srcPath, destPath)) {
        wxMessageBox("Operation completed successfully", "Success");
    } else {
        wxMessageBox("Tool not found", "Error", wxICON_ERROR);
    }
}

} // namespace reone
