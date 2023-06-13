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

#include "mainviewmodel.h"

#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/game/format/ssfreader.h"
#include "reone/game/script/routines.h"
#include "reone/graphics/format/lipreader.h"
#include "reone/graphics/format/mdlreader.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/tlkreader.h"
#include "reone/resource/talktable.h"
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

#include "di/graphicsmodule.h"

using namespace std;

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

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

void MainViewModel::openFile(const GameDirectoryItem &item) {
    if (!item.resId) {
        return;
    }
    if (item.archived) {
        auto extension = boost::to_lower_copy(item.path.extension().string());
        if (extension == ".bif") {
            auto maybeKey = std::find_if(_keyKeys.begin(), _keyKeys.end(), [&item](auto &key) {
                return key.resId == *item.resId;
            });
            if (maybeKey == _keyKeys.end()) {
                return;
            }
            auto resIdx = maybeKey->resIdx;
            auto bif = FileInputStream(item.path, OpenMode::Binary);
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
            openResource(*item.resId, res);
        } else if (extension == ".erf" || extension == ".sav" || extension == ".mod") {
            auto erf = FileInputStream(item.path, OpenMode::Binary);
            auto erfReader = ErfReader();
            erfReader.load(erf);
            auto maybeKey = std::find_if(erfReader.keys().begin(), erfReader.keys().end(), [&item](auto &key) {
                return key.resId == *item.resId;
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
            openResource(*item.resId, res);
        } else if (extension == ".rim") {
            auto rim = FileInputStream(item.path, OpenMode::Binary);
            auto rimReader = RimReader();
            rimReader.load(rim);
            auto maybeRes = std::find_if(rimReader.resources().begin(), rimReader.resources().end(), [&item](auto &res) {
                return res.resId == *item.resId;
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
            openResource(*item.resId, res);
        }
    } else {
        auto res = FileInputStream(item.path, OpenMode::Binary);
        openResource(*item.resId, res);
    }
}

void MainViewModel::openResource(const ResourceId &id, IInputStream &data) {
    auto pages = _pages.data();
    pages.clear();

    if (id.type == ResourceType::TwoDa) {
        auto reader = TwoDaReader();
        reader.load(data);
        auto twoDa = reader.twoDa();

        auto columns = vector<string>();
        columns.push_back("Index");
        for (auto &column : twoDa->columns()) {
            columns.push_back(column);
        }
        auto rows = vector<vector<string>>();
        for (int i = 0; i < twoDa->getRowCount(); ++i) {
            auto &row = twoDa->rows()[i];
            auto values = vector<string>();
            values.push_back(to_string(i));
            for (auto &value : row.values) {
                values.push_back(value);
            }
            rows.push_back(std::move(values));
        }

        auto content = make_shared<TableContent>(std::move(columns), std::move(rows));
        _tableContent.reset(std::move(content));

        pages.push_back(Page(PageType::Table, id.string()));

    } else if (isGFFCompatibleResType(id.type)) {
        auto reader = GffReader();
        reader.load(data);
        _gffContent.reset(reader.root());
        pages.push_back(Page(PageType::GFF, id.string()));

    } else if (id.type == ResourceType::Tlk) {
        if (!_talkTableContent.data()) {
            auto reader = TlkReader();
            reader.load(data);
            auto tlk = reader.table();

            auto columns = vector<string>();
            columns.push_back("Index");
            columns.push_back("Text");
            columns.push_back("Sound");

            auto rows = vector<vector<string>>();
            for (int i = 0; i < tlk->getStringCount(); ++i) {
                auto &str = tlk->getString(i);
                auto cleanedText = boost::replace_all_copy(str.text, "\n", "\\n");
                auto values = vector<string>();
                values.push_back(to_string(i));
                values.push_back(cleanedText);
                values.push_back(str.soundResRef);
                rows.push_back(std::move(values));
            }

            auto content = make_shared<TableContent>(std::move(columns), std::move(rows));
            _talkTableContent.reset(std::move(content));
        }
        pages.push_back(Page(PageType::TalkTable, id.string()));

    } else if (kFilesPlaintextExtensions.count(id.type) > 0) {
        data.seek(0, SeekOrigin::End);
        auto length = data.position();
        data.seek(0, SeekOrigin::Begin);
        auto text = string(length, '\0');
        data.read(&text[0], length);
        _textContent.reset(text);
        pages.push_back(Page(PageType::Text, id.string()));

    } else if (id.type == ResourceType::Ncs) {
        auto routines = make_unique<Routines>(_gameId, nullptr, nullptr);
        routines->init();

        auto pcodeBytes = make_unique<ByteArray>();
        auto pcode = ByteArrayOutputStream(*pcodeBytes);
        auto tool = NcsTool(_gameId);
        tool.toPCODE(data, pcode, *routines);
        _pcodeContent.reset(*pcodeBytes);
        pages.push_back(Page(PageType::PCODE, str(boost::format("%s.pcode") % id.resRef)));

        data.seek(0, SeekOrigin::Begin);
        auto nssBytes = make_unique<ByteArray>();
        auto nss = ByteArrayOutputStream(*nssBytes);
        tool.toNSS(data, nss, *routines);
        _nssContent.reset(*nssBytes);
        pages.push_back(Page(PageType::NSS, str(boost::format("%s.nss") % id.resRef)));

    } else if (id.type == ResourceType::Nss) {
        data.seek(0, SeekOrigin::End);
        auto length = data.position();
        data.seek(0, SeekOrigin::Begin);
        auto text = string(length, '\0');
        data.read(&text[0], length);
        _nssContent.reset(text);
        pages.push_back(Page(PageType::NSS, id.string()));

    } else if (id.type == ResourceType::Lip) {
        auto reader = LipReader("");
        reader.load(data);
        auto animation = reader.animation();

        auto columns = vector<string>();
        columns.push_back("Time");
        columns.push_back("Shape");
        auto rows = vector<vector<string>>();
        for (auto &kf : animation->keyframes()) {
            auto values = vector<string>();
            values.push_back(to_string(kf.time));
            values.push_back(to_string(kf.shape));
            rows.push_back(std::move(values));
        }
        auto content = make_shared<TableContent>(std::move(columns), std::move(rows));
        _tableContent.reset(std::move(content));

        pages.push_back(Page(PageType::Table, id.string()));

    } else if (id.type == ResourceType::Ssf) {
        auto reader = SsfReader();
        reader.load(data);
        auto &soundSet = reader.soundSet();

        auto columns = vector<string>();
        columns.push_back("Index");
        columns.push_back("Sound");
        auto rows = vector<vector<string>>();
        for (size_t i = 0; i < soundSet.size(); ++i) {
            auto values = vector<string>();
            values.push_back(to_string(i));
            values.push_back(to_string(soundSet.at(i)));
            rows.push_back(std::move(values));
        }
        auto content = make_shared<TableContent>(std::move(columns), std::move(rows));
        _tableContent.reset(std::move(content));

        pages.push_back(Page(PageType::Table, id.string()));

    } else if (id.type == ResourceType::Tpc || id.type == ResourceType::Tga) {
        auto tgaBytes = make_shared<ByteArray>();
        if (id.type == ResourceType::Tpc) {
            auto tga = ByteArrayOutputStream(*tgaBytes);
            auto txiBytes = make_unique<ByteArray>();
            auto txi = ByteArrayOutputStream(*txiBytes);
            TpcTool().toTGA(data, tga, txi, false);
            _imageInfo.reset(*txiBytes);
        } else {
            data.seek(0, SeekOrigin::End);
            auto length = data.position();
            data.seek(0, SeekOrigin::Begin);
            tgaBytes->resize(length, '\0');
            data.read(&(*tgaBytes)[0], length);
            _imageInfo.reset("");
        }
        _imageData.reset(tgaBytes);
        pages.push_back(Page(PageType::Image, id.string()));

    } else if (id.type == ResourceType::Mdl || id.type == ResourceType::Mdx) {
        loadEngine();

        /*
        auto mdxBytes = _resourceModule->resources().get(id.resRef, ResourceType::Mdx, false);
        if (!mdxBytes) {
            throw runtime_error("Companion MDX file not found");
        }
        auto mdx = ByteArrayInputStream(*mdxBytes);
        auto reader = MdlReader(_graphicsModule->models(), _graphicsModule->textures());
        reader.load(data, mdx);
        */
        auto model = _graphicsModule->models().get(id.resRef);
        auto &scene = _sceneModule->graphs().get(kSceneMain);
        auto sceneNode = scene.newModel(*model, ModelUsage::Creature);
        scene.clear();
        scene.addRoot(sceneNode);

        updateCameraTransform();

        pages.push_back(Page(PageType::Model, id.string()));

    } else if (id.type == ResourceType::Wav) {
        loadEngine();
        auto mp3ReaderFactory = Mp3ReaderFactory();
        auto reader = WavReader(mp3ReaderFactory);
        reader.load(data);
        _audioStream.reset(reader.stream());
        pages.push_back(Page(PageType::Audio, id.string()));

    } else {
        return;
    }

    _pages.reset(pages);

    if (id.type != ResourceType::Wav) {
        _audioStream.reset(nullptr);
    }
}

void MainViewModel::loadGameDirectory() {
    auto tslExePath = getPathIgnoreCase(_gamePath, "swkotor2.exe", false);
    _gameId = !tslExePath.empty() ? GameID::TSL : GameID::KotOR;

    auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key", false);
    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = KeyReader();
    keyReader.load(key);
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    for (auto &file : boost::filesystem::directory_iterator(_gamePath)) {
        auto filename = boost::to_lower_copy(file.path().filename().string());
        auto extension = boost::to_lower_copy(file.path().extension().string());
        bool container;
        if ((file.status().type() == boost::filesystem::directory_file && kFilesSubdirectoryWhitelist.count(filename) > 0) ||
            (file.status().type() == boost::filesystem::regular_file && kFilesArchiveExtensions.count(extension) > 0)) {
            container = true;
        } else if (file.status().type() == boost::filesystem::regular_file && (kFilesExtensionBlacklist.count(extension) == 0 && extension != ".txt")) {
            container = false;
        } else {
            continue;
        }
        auto item = make_shared<GameDirectoryItem>();
        item->displayName = filename;
        item->path = file.path();
        item->container = container;
        if (extension == ".tlk") {
            auto resRef = filename.substr(0, filename.size() - 4);
            item->resId = make_unique<ResourceId>(resRef, ResourceType::Tlk);
        }
        _gameDirItems.push_back(std::move(item));
    }
}

void MainViewModel::loadTools() {
    _tools.clear();
    _tools.push_back(make_shared<KeyBifTool>());
    _tools.push_back(make_shared<ErfTool>());
    _tools.push_back(make_shared<RimTool>());
    _tools.push_back(make_shared<TwoDaTool>());
    _tools.push_back(make_shared<TlkTool>());
    _tools.push_back(make_shared<LipTool>());
    _tools.push_back(make_shared<SsfTool>());
    _tools.push_back(make_shared<GffTool>());
    _tools.push_back(make_shared<TpcTool>());
    _tools.push_back(make_shared<AudioTool>());
    _tools.push_back(make_shared<NcsTool>(_gameId));
}

void MainViewModel::loadEngine() {
    if (_engineLoaded || _gamePath.empty()) {
        return;
    }
    _engineLoadRequested.reset(true);

    _graphicsOpt.grass = false;
    _graphicsOpt.ssao = false;
    _graphicsOpt.ssr = false;
    _graphicsOpt.fxaa = false;
    _graphicsOpt.sharpen = false;

    _systemModule = make_unique<SystemModule>();
    _resourceModule = make_unique<ResourceModule>(_gamePath);
    _graphicsModule = make_unique<ToolkitGraphicsModule>(_graphicsOpt, *_resourceModule);
    _audioModule = make_unique<AudioModule>(_audioOpt, *_resourceModule);
    _sceneModule = make_unique<SceneModule>(_graphicsOpt, *_audioModule, *_graphicsModule);

    _systemModule->init();
    _resourceModule->init();
    _graphicsModule->init();
    _audioModule->init();
    _sceneModule->init();

    auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key", false);
    auto guiTexPackPath = getPathIgnoreCase(_gamePath, "texturepacks/swpc_tex_gui.erf", false);
    auto tpaTexPackPath = getPathIgnoreCase(_gamePath, "texturepacks/swpc_tex_tpa.erf", false);

    auto &resources = _resourceModule->resources();
    resources.indexKeyFile(keyPath);
    resources.indexErfFile(guiTexPackPath);
    resources.indexErfFile(tpaTexPackPath);

    auto &sceneGraphs = _sceneModule->graphs();
    sceneGraphs.reserve(kSceneMain);
    auto &scene = sceneGraphs.get(kSceneMain);

    _cameraNode = scene.newCamera();

    scene.setActiveCamera(_cameraNode.get());
    scene.setAmbientLightColor(glm::vec3(1.0f));

    _engineLoaded = true;
}

void MainViewModel::extractArchive(const boost::filesystem::path &srcPath, const boost::filesystem::path &destPath) {
    auto extension = boost::to_lower_copy(srcPath.extension().string());
    if (extension == ".bif") {
        auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key", false);
        auto keyReader = KeyReader();
        auto key = FileInputStream(keyPath, OpenMode::Binary);
        keyReader.load(key);
        auto filename = boost::to_lower_copy(srcPath.filename().string());
        auto maybeBif = std::find_if(keyReader.files().begin(), keyReader.files().end(), [&filename](auto &file) {
            return boost::contains(boost::to_lower_copy(file.filename), filename);
        });
        if (maybeBif == keyReader.files().end()) {
            return;
        }
        auto bifIdx = std::distance(keyReader.files().begin(), maybeBif);
        KeyBifTool().extractBIF(keyReader, bifIdx, srcPath, destPath);
    } else if (extension == ".erf" || extension == ".sav" || extension == ".mod") {
        auto erf = FileInputStream(srcPath, OpenMode::Binary);
        auto erfReader = ErfReader();
        erfReader.load(erf);
        ErfTool().extract(erfReader, srcPath, destPath);
    } else if (extension == ".rim") {
        auto rim = FileInputStream(srcPath, OpenMode::Binary);
        auto rimReader = RimReader();
        rimReader.load(rim);
        RimTool().extract(rimReader, srcPath, destPath);
    }
}

void MainViewModel::extractAllBifs(const boost::filesystem::path &destPath) {
    auto tool = KeyBifTool();

    auto keyPath = getPathIgnoreCase(_gamePath, "chitin.key");
    auto key = FileInputStream(keyPath, OpenMode::Binary);
    auto keyReader = KeyReader();
    keyReader.load(key);

    auto progress = Progress();
    progress.visible = true;
    progress.title = "Extract all BIF archives";
    _progress.reset(progress);

    int bifIdx = 0;
    for (auto &file : _keyFiles) {
        auto cleanedFilename = boost::replace_all_copy(file.filename, "\\", "/");
        auto bifPath = getPathIgnoreCase(_gamePath, cleanedFilename);
        if (bifPath.empty()) {
            continue;
        }
        progress.value = 100 * bifIdx / static_cast<int>(_keyFiles.size());
        tool.extractBIF(keyReader, bifIdx++, bifPath, destPath);
        _progress.reset(progress);
    }

    progress.visible = false;
    _progress.reset(progress);
}

void MainViewModel::batchConvertTpcToTga(const boost::filesystem::path &srcPath, const boost::filesystem::path &destPath) {
    vector<boost::filesystem::path> tpcFiles;
    for (auto &file : boost::filesystem::directory_iterator(srcPath)) {
        if (file.status().type() != boost::filesystem::regular_file) {
            continue;
        }
        auto extension = boost::to_lower_copy(file.path().extension().string());
        if (extension == ".tpc") {
            tpcFiles.push_back(file.path());
        }
    }

    auto progress = Progress();
    progress.visible = true;
    progress.title = "Batch convert TPC to TGA/TXI";
    _progress.reset(progress);

    auto tool = TpcTool();
    for (size_t i = 0; i < tpcFiles.size(); ++i) {
        progress.value = 100 * static_cast<int>(i / tpcFiles.size());
        _progress.reset(progress);

        auto &tpcPath = tpcFiles[i];
        tool.toTGA(tpcPath, destPath);
    }

    progress.visible = false;
    _progress.reset(progress);
}

bool MainViewModel::invokeTool(Operation operation,
                               const boost::filesystem::path &srcPath,
                               const boost::filesystem::path &destPath) {
    for (auto &tool : _tools) {
        if (!tool->supports(operation, srcPath)) {
            continue;
        }
        tool->invoke(operation, srcPath, destPath, _gamePath);
        return true;
    }
    return false;
}

void MainViewModel::render3D(int w, int h) {
    auto &scene = _sceneModule->graphs().get(kSceneMain);
    scene.update(0.0f);

    float aspect = w / static_cast<float>(h);
    _cameraNode->setPerspectiveProjection(glm::radians(46.8), aspect, kDefaultClipPlaneNear, kDefaultClipPlaneFar);

    auto output = _graphicsModule->pipeline().draw(scene, glm::ivec2(w, h));
    if (!output) {
        return;
    }
    _graphicsModule->uniforms().setGeneral([](auto &general) {
        general.resetGlobals();
        general.resetLocals();
    });
    _graphicsModule->graphicsContext().withViewport(glm::ivec4(0, 0, w, h), [this, &output]() {
        _graphicsModule->graphicsContext().clearColorDepth();
        _graphicsModule->shaders().use(_graphicsModule->shaders().simpleTexture());
        _graphicsModule->textures().bind(*output);
        _graphicsModule->meshes().quadNDC().draw();
    });
}

void MainViewModel::updateCameraTransform() {
    auto cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, glm::vec3(0.0f, _cameraDistance, 0.0f));
    cameraTransform *= glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraTransform *= glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    _cameraNode->setLocalTransform(cameraTransform);
}

void MainViewModel::onViewCreated() {
    loadTools();
}

void MainViewModel::onViewDestroyed() {
    _audioStream.reset(nullptr);
}

void MainViewModel::onGameDirectoryChanged(boost::filesystem::path path) {
    _gamePath = path;

    loadGameDirectory();
    loadTools();
}

void MainViewModel::onGameDirectoryItemIdentified(int index, GameDirectoryItemId id) {
    auto &item = _gameDirItems[index];
    item->id = id;
    _idToGameDirItem.insert(make_pair(id, item.get()));
}

void MainViewModel::onGameDirectoryItemExpanding(GameDirectoryItemId id) {
    if (_idToGameDirItem.count(id) == 0) {
        return;
    }
    auto &expandingItem = *_idToGameDirItem.at(id);
    if (boost::filesystem::is_directory(expandingItem.path)) {
        for (auto &file : boost::filesystem::directory_iterator(expandingItem.path)) {
            auto filename = boost::to_lower_copy(file.path().filename().string());
            auto extension = boost::to_lower_copy(file.path().extension().string());
            bool container;
            if (file.status().type() == boost::filesystem::directory_file || kFilesArchiveExtensions.count(extension) > 0) {
                container = true;
            } else if (file.status().type() == boost::filesystem::regular_file && kFilesExtensionBlacklist.count(extension) == 0) {
                container = false;
            } else {
                continue;
            }
            auto item = make_shared<GameDirectoryItem>();
            item->parentId = expandingItem.id;
            item->displayName = filename;
            item->path = file.path();
            if (!extension.empty()) {
                auto resType = getResTypeByExt(extension.substr(1), false);
                if (resType != ResourceType::Invalid) {
                    auto resRef = filename.substr(0, filename.size() - 4);
                    item->resId = make_shared<ResourceId>(resRef, resType);
                }
            }
            item->container = container;
            _gameDirItems.push_back(std::move(item));
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
                    auto item = make_shared<GameDirectoryItem>();
                    item->parentId = expandingItem.id;
                    item->displayName = str(boost::format("%s.%s") % key.resId.resRef % getExtByResType(key.resId.type));
                    item->path = expandingItem.path;
                    item->resId = make_shared<ResourceId>(key.resId);
                    item->archived = true;
                    _gameDirItems.push_back(std::move(item));
                }
            }
        } else if (boost::ends_with(extension, ".erf") || boost::ends_with(extension, ".sav") || boost::ends_with(extension, ".mod")) {
            auto erf = FileInputStream(expandingItem.path, OpenMode::Binary);
            auto erfReader = ErfReader();
            erfReader.load(erf);
            auto &keys = erfReader.keys();
            for (auto &key : keys) {
                auto item = make_shared<GameDirectoryItem>();
                item->parentId = expandingItem.id;
                item->displayName = str(boost::format("%s.%s") % key.resId.resRef % getExtByResType(key.resId.type));
                item->path = expandingItem.path;
                item->resId = make_shared<ResourceId>(key.resId);
                item->archived = true;
                _gameDirItems.push_back(std::move(item));
            }
        } else if (boost::ends_with(extension, ".rim")) {
            auto rim = FileInputStream(expandingItem.path, OpenMode::Binary);
            auto rimReader = RimReader();
            rimReader.load(rim);
            auto &resources = rimReader.resources();
            for (auto &resource : resources) {
                auto item = make_shared<GameDirectoryItem>();
                item->parentId = expandingItem.id;
                item->displayName = str(boost::format("%s.%s") % resource.resId.resRef % getExtByResType(resource.resId.type));
                item->path = expandingItem.path;
                item->resId = make_shared<ResourceId>(resource.resId);
                item->archived = true;
                _gameDirItems.push_back(std::move(item));
            }
        }
    }
    expandingItem.loaded = true;
}

void MainViewModel::onGameDirectoryItemActivated(GameDirectoryItemId id) {
    auto &item = *_idToGameDirItem.at(id);
    openFile(item);
}

void MainViewModel::onGLCanvasMouseMotion() {
}

void MainViewModel::onGLCanvasMouseWheel(int delta) {
    _cameraDistance = glm::clamp(_cameraDistance - glm::clamp(delta, -1, 1), 0.0f, 64.0f);
    updateCameraTransform();
    _renderRequested.reset(true);
}

} // namespace reone
