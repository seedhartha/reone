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

#include <wx/artprov.h>
#include <wx/choicdlg.h>
#include <wx/dirdlg.h>
#include <wx/mstream.h>

#include "reone/resource/typeutil.h"
#include "reone/system/fileutil.h"

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
    static constexpr int saveFile = wxID_HIGHEST + 5;
};

struct CommandID {
    static constexpr int extract = 1;
    static constexpr int decompile = 2;
    static constexpr int decompileNoOptimize = 3;
    static constexpr int exportRes = 4;
    static constexpr int exportTgaTxi = 5;
    static constexpr int exportWavMp3 = 6;
    static constexpr int createErf = 7;
    static constexpr int createRim = 8;
    static constexpr int createMod = 9;
};

struct TimerID {
    static constexpr int render = 1;
    static constexpr int audio = 2;
};

class ResourcesItemClientData : public wxClientData {
public:
    ResourcesItemClientData(ResourcesItemId id) :
        _id(std::move(id)) {
    }

    const ResourcesItemId &id() const {
        return _id;
    }

private:
    ResourcesItemId _id;
};

class PageClientData : public wxClientData {
public:
    PageClientData(ResourceId resId) :
        _resId(std::move(resId)) {
    }

    const ResourceId &resId() const {
        return _resId;
    }

private:
    ResourceId _resId;
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

    m_resourcesListBox = new wxListBox(resourcesPanel, wxID_ANY);
    m_resourcesListBox->Bind(wxEVT_CONTEXT_MENU, &ResourceExplorerFrame::OnResourcesListBoxContextMenu, this);
    m_resourcesListBox->Bind(wxEVT_LISTBOX, &ResourceExplorerFrame::OnResourcesListBox, this);
    m_resourcesListBox->Bind(wxEVT_LISTBOX_DCLICK, &ResourceExplorerFrame::OnResourcesListBoxDoubleClick, this);

    auto navigationPanel = new wxPanel(resourcesPanel);
    auto artProvider = new wxArtProvider();
#if wxCHECK_VERSION(3, 1, 6)
    m_goToParentButton = new wxBitmapButton(navigationPanel, wxID_ANY, artProvider->GetBitmapBundle(wxART_GO_TO_PARENT));
#else
    m_goToParentButton = new wxBitmapButton(navigationPanel, wxID_ANY, artProvider->GetBitmap(wxART_GO_TO_PARENT));
#endif
    m_goToParentButton->Disable();
    m_goToParentButton->Bind(wxEVT_BUTTON, &ResourceExplorerFrame::OnGoToParentButton, this);

    auto resourcesSizer = new wxStaticBoxSizer(wxVERTICAL, resourcesPanel, "Resources");
    resourcesSizer->Add(navigationPanel, 0, wxEXPAND);
    resourcesSizer->Add(m_resourcesListBox, 1, wxEXPAND);
    resourcesPanel->SetSizer(resourcesSizer);

    m_notebook = new wxAuiNotebook(m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE & ~(wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE));
    m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CLOSE, &ResourceExplorerFrame::OnNotebookPageClose, this);
    m_notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &ResourceExplorerFrame::OnNotebookPageChanged, this);

    m_imagePanel = new ImageResourcePanel(m_viewModel.imageResViewModel(), m_notebook);
    m_modelPanel = new ModelResourcePanel(m_viewModel.modelResViewModel(), m_notebook);
    m_audioPanel = new AudioResourcePanel(m_viewModel.audioResViewModel(), m_notebook);

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
}

void ResourceExplorerFrame::BindViewModel() {
    m_viewModel.goToParentEnabled().addChangedHandler([this](const auto &enabled) {
        m_goToParentButton->Enable(enabled);
    });
    m_viewModel.resourcesItems().addChangedHandler([this](const auto &args) {
        switch (args.type) {
        case CollectionChangeType::Reset: {
            m_resourcesListBox->Freeze();
            m_resourcesListBox->Clear();
            for (const auto &resItem : *m_viewModel.resourcesItems()) {
                m_resourcesListBox->Append(resItem->displayName, new ResourcesItemClientData {resItem->id});
            }
            m_resourcesListBox->Thaw();
            break;
        }
        default:
            throw std::logic_error("Unsupported collection change type: " + std::to_string(static_cast<int>(args.type)));
        }
    });
    m_viewModel.pages().addChangedHandler([this](const auto &args) {
        switch (args.type) {
        case CollectionChangeType::Add: {
            auto &page = args.addedItem->get();
            wxWindow *window;
            if (kStaticPageTypes.count(page->type) > 0) {
                window = GetStaticPageWindow(page->type);
            } else {
                window = NewPageWindow(*page);
                page->viewModel->modified().addChangedHandler([this, &page](const auto &modified) {
                    for (size_t i = 0; i < m_notebook->GetPageCount(); ++i) {
                        auto notebookPage = m_notebook->GetPage(i);
                        auto &pageData = *static_cast<PageClientData *>(notebookPage->GetClientData());
                        if (pageData.resId() != page->resourceId) {
                            continue;
                        }
                        m_notebook->SetPageText(i, str(boost::format("*%1%") % page->displayName));
                        m_saveFileMenuItem->Enable(m_notebook->GetSelection() == i);
                        break;
                    }
                    page->dirty = true;
                });
            }
            if (page->type == PageType::Model || page->type == PageType::Audio) {
                m_modelPanel->Show();
                m_modelPanel->InitGL();
                m_modelPanel->Hide();
            }
            window->SetClientData(new PageClientData {page->resourceId});
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
    m_viewModel.saveFile(*page, destPath);
}

wxWindow *ResourceExplorerFrame::NewPageWindow(Page &page) {
    switch (page.type) {
    case PageType::Text: {
        auto &viewModel = *std::static_pointer_cast<TextResourceViewModel>(page.viewModel);
        return new TextResourcePanel {viewModel, m_notebook};
    }
    case PageType::Table: {
        auto &viewModel = *std::static_pointer_cast<TableResourceViewModel>(page.viewModel);
        return new TableResourcePanel {viewModel, m_notebook};
    }
    case PageType::GFF: {
        auto &viewModel = *std::static_pointer_cast<GFFResourceViewModel>(page.viewModel);
        return new GFFResourcePanel {viewModel, m_viewModel.talkTable(), m_notebook};
    }
    case PageType::NCS: {
        auto &viewModel = *std::static_pointer_cast<NCSResourceViewModel>(page.viewModel);
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
}

void ResourceExplorerFrame::OnGoToParentButton(wxCommandEvent &event) {
    m_viewModel.onGoToParentButton();
}

void ResourceExplorerFrame::OnResourcesListBox(wxCommandEvent &event) {
    _resListBoxSelection = event.GetSelection();
}

void ResourceExplorerFrame::OnResourcesListBoxDoubleClick(wxCommandEvent &event) {
    int itemIdx = event.GetInt();
    if (itemIdx == -1) {
        return;
    }
    auto *itemData = m_resourcesListBox->GetClientObject(itemIdx);
    if (!itemData) {
        return;
    }
    auto &resItemId = static_cast<ResourcesItemClientData *>(itemData)->id();
    m_viewModel.onResourcesListBoxDoubleClick(resItemId);
}

void ResourceExplorerFrame::OnResourcesListBoxContextMenu(wxContextMenuEvent &event) {
    auto *itemData = m_resourcesListBox->GetClientObject(_resListBoxSelection);
    if (!itemData) {
        return;
    }
    auto &resItemId = static_cast<ResourcesItemClientData *>(itemData)->id();
    auto &resItem = m_viewModel.getResourcesItemById(resItemId);
    auto menu = wxMenu();
    if (resItem.id.resId) {
        menu.Append(CommandID::exportRes, "Export...");
        if (resItem.id.resId->type == ResType::Tpc) {
            menu.Append(CommandID::exportTgaTxi, "Export as TGA/TXI...");
        } else if (resItem.id.resId->type == ResType::Wav) {
            menu.Append(CommandID::exportWavMp3, "Export as regular WAV/MP3...");
        } else if (resItem.id.resId->type == ResType::Ncs) {
            menu.Append(CommandID::decompile, "Decompile");
            menu.Append(CommandID::decompileNoOptimize, "Decompile without optimization");
        }
    } else if (!resItem.archived && std::filesystem::is_regular_file(resItem.id.path)) {
        auto extension = resItem.id.path.extension().string();
        if (kFilesArchiveExtensions.count(extension) > 0) {
            menu.Append(CommandID::extract, "Extract...");
        }
    } else if (std::filesystem::is_directory(resItem.id.path)) {
        menu.Append(CommandID::createRim, "Create RIM archive...");
        menu.Append(CommandID::createErf, "Create ERF archive...");
        menu.Append(CommandID::createMod, "Create MOD archive...");
    }
    menu.SetClientData(new ResourcesItemClientData {resItemId});
    menu.Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(ResourceExplorerFrame::OnPopupCommandSelected), nullptr, this);
    PopupMenu(&menu);
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
    auto &page = *m_viewModel.pages().at(pageIdx);
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
        auto &resItemId = static_cast<ResourcesItemClientData *>(menu->GetClientData())->id();
        auto &resItem = m_viewModel.getResourcesItemById(resItemId);

        auto dialog = new wxDirDialog(nullptr, "Choose extraction directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));

        m_viewModel.extractArchive(resItem.id.path, destPath);

    } else if (event.GetId() == CommandID::decompile) {
        auto &resItemId = static_cast<ResourcesItemClientData *>(menu->GetClientData())->id();
        m_viewModel.decompile(resItemId, true);

    } else if (event.GetId() == CommandID::decompileNoOptimize) {
        auto &resItemId = static_cast<ResourcesItemClientData *>(menu->GetClientData())->id();
        m_viewModel.decompile(resItemId, false);

    } else if (event.GetId() == CommandID::exportRes ||
               event.GetId() == CommandID::exportTgaTxi ||
               event.GetId() == CommandID::exportWavMp3) {
        auto &resItemId = static_cast<ResourcesItemClientData *>(menu->GetClientData())->id();
        auto dialog = new wxDirDialog(nullptr, "Choose destination directory", "", wxDD_DEFAULT_STYLE | wxDD_DIR_MUST_EXIST);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));
        if (event.GetId() == CommandID::exportTgaTxi) {
            m_viewModel.exportTgaTxi(resItemId, destPath);
        } else if (event.GetId() == CommandID::exportWavMp3) {
            m_viewModel.exportWavMp3(resItemId, destPath);
        } else {
            m_viewModel.exportResource(resItemId, destPath);
        }

    } else if (event.GetId() == CommandID::createRim ||
               event.GetId() == CommandID::createErf ||
               event.GetId() == CommandID::createMod) {
        std::unordered_map<int, std::string> cmdToExt {
            {CommandID::createRim, "rim"}, //
            {CommandID::createErf, "erf"}, //
            {CommandID::createMod, "mod"}  //
        };
        const auto &resItemId = static_cast<ResourcesItemClientData *>(menu->GetClientData())->id();
        auto defDir = resItemId.path.parent_path().string();
        auto defFilename = resItemId.path.filename().string();
        const auto &ext = cmdToExt.at(event.GetId());
        auto wildcard = str(boost::format("%1% archive (*.%2%)|*.%2%") % boost::to_upper_copy(ext) % ext);
        auto dialog = new wxFileDialog(
            nullptr,
            "Choose destination file",
            defDir,
            defFilename,
            wildcard,
            wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
        if (dialog->ShowModal() != wxID_OK) {
            return;
        }
        auto destPath = std::filesystem::path(std::string(dialog->GetPath()));
        if (event.GetId() == CommandID::createRim) {
            m_viewModel.createRim(resItemId, destPath);
        } else if (event.GetId() == CommandID::createErf) {
            m_viewModel.createErf(resItemId, destPath);
        } else {
            m_viewModel.createMod(resItemId, destPath);
        }
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
}

void ResourceExplorerFrame::OnComposeLipCommand(wxCommandEvent &event) {
    auto dialog = ComposeLipDialog(this, -1, "LIP Composer");
    if (dialog.ShowModal() != wxID_OK) {
    }
}

} // namespace reone
