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

#include "main.h"

#include "reone/audio/format/mp3reader.h"
#include "reone/audio/format/wavreader.h"
#include "reone/game/format/ssfreader.h"
#include "reone/graphics/animation.h"
#include "reone/graphics/format/lipreader.h"
#include "reone/graphics/format/mdlmdxreader.h"
#include "reone/graphics/lipanimation.h"
#include "reone/resource/exception/notfound.h"
#include "reone/resource/format/2dareader.h"
#include "reone/resource/format/gffreader.h"
#include "reone/resource/format/keyreader.h"
#include "reone/resource/format/tlkreader.h"
#include "reone/resource/talktable.h"
#include "reone/system/fileutil.h"
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
#include "reone/tools/lip/shapeutil.h"

#include "../di/graphicsmodule.h"

using namespace reone::audio;
using namespace reone::game;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;

namespace reone {

static const std::set<std::string> kFilesSubdirectoryWhitelist {
    "data", "lips", "localvault", "modules", "movies", "override", "rims", "saves", "texturepacks", //
    "streammusic", "streamsounds", "streamwaves", "streamvoice"};

static const std::set<std::string> kFilesArchiveExtensions {".bif", ".erf", ".sav", ".rim", ".mod"};

static const std::set<std::string> kFilesExtensionBlacklist {
    ".key",                                         //
    ".lnk", ".bat", ".exe", ".dll", ".ini", ".ico", //
    ".zip", ".pdf",                                 //
    ".hashdb", ".info", ".script", ".dat", ".msg", ".sdb", ".ds_store"};

static const std::set<ResType> kFilesPlaintextResTypes {
    ResType::Txt,
    ResType::Txi,
    ResType::Lyt,
    ResType::Vis};

void MainViewModel::openFile(const GameDirectoryItem &item) {
    withResourceStream(item, [this, &item](auto &res) {
        try {
            openResource(*item.resId, res);
        } catch (const std::exception &e) {
            error(boost::format("Error opening resource: %s") % std::string(e.what()));
        }
    });
}

void MainViewModel::openResource(const ResourceId &id, IInputStream &data) {
    PageType pageType;
    try {
        pageType = getPageType(id.type);
    } catch (const std::invalid_argument &e) {
        return;
    }
    auto samePage = std::find_if(_pages.begin(), _pages.end(), [&id, &pageType](auto &page) {
        return page->resourceId == id && page->type == pageType;
    });
    if (samePage != _pages.end()) {
        _pageSelected.invoke(std::distance(_pages.begin(), samePage));
        return;
    }

    if (kFilesPlaintextResTypes.count(id.type) > 0) {
        data.seek(0, SeekOrigin::End);
        auto length = data.position();
        data.seek(0, SeekOrigin::Begin);
        auto text = std::string(length, '\0');
        data.read(&text[0], length);

        auto page = std::make_shared<Page>(PageType::Text, id.string(), id);
        page->textContent = text;
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::TwoDa) {
        auto reader = TwoDaReader(data);
        reader.load();
        auto twoDa = reader.twoDa();

        auto columns = std::vector<std::string>();
        columns.push_back("Index");
        for (auto &column : twoDa->columns()) {
            columns.push_back(column);
        }
        auto rows = std::vector<std::vector<std::string>>();
        for (int i = 0; i < twoDa->getRowCount(); ++i) {
            auto &row = twoDa->rows()[i];
            auto values = std::vector<std::string>();
            values.push_back(std::to_string(i));
            for (auto &value : row.values) {
                values.push_back(value);
            }
            rows.push_back(std::move(values));
        }

        auto page = std::make_shared<Page>(PageType::Table, id.string(), id);
        page->tableContent = std::make_shared<TableContent>(std::move(columns), std::move(rows));
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (isGFFCompatibleResType(id.type)) {
        auto reader = GffReader(data);
        reader.load();

        auto page = std::make_shared<Page>(PageType::GFF, id.string(), id);
        page->gffContent = reader.root();
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Tlk) {
        auto reader = TlkReader(data);
        reader.load();
        auto tlk = reader.table();

        auto columns = std::vector<std::string>();
        columns.push_back("Index");
        columns.push_back("Text");
        columns.push_back("Sound");

        auto rows = std::vector<std::vector<std::string>>();
        for (int i = 0; i < tlk->getStringCount(); ++i) {
            auto &str = tlk->getString(i);
            auto cleanedText = boost::replace_all_copy(str.text, "\n", "\\n");
            auto values = std::vector<std::string>();
            values.push_back(std::to_string(i));
            values.push_back(cleanedText);
            values.push_back(str.soundResRef);
            rows.push_back(std::move(values));
        }

        auto page = std::make_shared<Page>(PageType::Table, id.string(), id);
        page->tableContent = std::make_shared<TableContent>(std::move(columns), std::move(rows));
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Ncs) {
        auto pcodeBytes = ByteBuffer();
        auto pcode = MemoryOutputStream(pcodeBytes);
        NcsTool(_gameId).toPCODE(data, pcode, *_routines);

        auto page = std::make_shared<Page>(PageType::NCS, id.string(), id);
        page->pcodeContent = std::string(pcodeBytes.begin(), pcodeBytes.end());
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Nss) {
        data.seek(0, SeekOrigin::End);
        auto length = data.position();
        data.seek(0, SeekOrigin::Begin);
        auto text = std::string(length, '\0');
        data.read(&text[0], length);

        auto page = std::make_shared<Page>(PageType::NSS, id.string(), id);
        page->nssContent = text;
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Lip) {
        auto reader = LipReader(data, "");
        reader.load();
        auto animation = reader.animation();

        auto columns = std::vector<std::string>();
        columns.push_back("Time");
        columns.push_back("Shape");
        auto rows = std::vector<std::vector<std::string>>();
        for (auto &kf : animation->keyframes()) {
            auto values = std::vector<std::string>();
            values.push_back(std::to_string(kf.time));
            values.push_back(str(boost::format("%s [%d]") % describeLipShape(static_cast<LipShape>(kf.shape)) % static_cast<int>(kf.shape)));
            rows.push_back(std::move(values));
        }

        auto page = std::make_shared<Page>(PageType::Table, id.string(), id);
        page->tableContent = std::make_shared<TableContent>(std::move(columns), std::move(rows));
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Ssf) {
        auto reader = SsfReader(data);
        reader.load();
        auto &soundSet = reader.soundSet();

        auto columns = std::vector<std::string>();
        columns.push_back("Index");
        columns.push_back("StrRef");
        columns.push_back("TalkTableText");
        columns.push_back("TalkTableSound");
        auto rows = std::vector<std::vector<std::string>>();
        for (size_t i = 0; i < soundSet.size(); ++i) {
            auto values = std::vector<std::string>();
            values.push_back(std::to_string(i));
            auto strRef = soundSet.at(i);
            values.push_back(std::to_string(strRef));
            if (strRef != -1) {
                values.push_back(getTalkTableText(strRef));
                values.push_back(getTalkTableSound(strRef));
            } else {
                values.push_back("");
                values.push_back("");
            }
            rows.push_back(std::move(values));
        }

        auto page = std::make_shared<Page>(PageType::Table, id.string(), id);
        page->tableContent = std::make_shared<TableContent>(std::move(columns), std::move(rows));
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Tpc || id.type == ResType::Tga) {
        auto tgaBytes = std::make_shared<ByteBuffer>();
        auto txiBytes = std::make_shared<ByteBuffer>();
        if (id.type == ResType::Tpc) {
            auto tga = MemoryOutputStream(*tgaBytes);
            auto txi = MemoryOutputStream(*txiBytes);
            TpcTool().toTGA(data, tga, txi, false);
        } else {
            data.seek(0, SeekOrigin::End);
            auto length = data.position();
            data.seek(0, SeekOrigin::Begin);
            tgaBytes->resize(length, '\0');
            data.read(&(*tgaBytes)[0], length);
        }
        _imageChanged.invoke(ImageContent(tgaBytes, txiBytes));

        auto pageToErase = std::find_if(_pages.begin(), _pages.end(), [](auto &page) {
            return page->type == PageType::Image;
        });
        if (pageToErase != _pages.end()) {
            auto index = std::distance(_pages.begin(), pageToErase);
            _pageRemoving.invoke(PageRemovingEventData(index, pageToErase->get()));
            _pages.erase(pageToErase);
        }
        auto page = std::make_shared<Page>(PageType::Image, id.string(), id);
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Mdl) {
        loadEngine();

        _renderEnabled.invoke(false);

        auto mdxRes = _resourceModule->resources().find(ResourceId(id.resRef, ResType::Mdx));
        if (!mdxRes) {
            throw ResourceNotFoundException("Companion MDX resource not found: " + id.resRef.value());
        }
        auto mdx = MemoryInputStream(mdxRes->data);
        auto reader = MdlMdxReader(data, mdx, _graphicsModule->models(), _graphicsModule->textures());
        reader.load();

        auto &scene = _sceneModule->graphs().get(kSceneMain);
        scene.clear();
        scene.update(0.0f);
        _modelNode.reset();

        _model = reader.model();
        _model->init();
        _animations.invoke(_model->getAnimationNames());

        _modelNode = scene.newModel(*_model, ModelUsage::Creature);
        _modelHeading = 0.0f;
        _modelPitch = 0.0f;
        updateModelTransform();
        scene.addRoot(_modelNode);

        _cameraPosition = glm::vec3(0.0f, 8.0f, 0.0f);
        updateCameraTransform();

        auto pageToErase = std::find_if(_pages.begin(), _pages.end(), [](auto &page) {
            return page->type == PageType::Model;
        });
        if (pageToErase != _pages.end()) {
            auto index = std::distance(_pages.begin(), pageToErase);
            _pageRemoving.invoke(PageRemovingEventData(index, pageToErase->get()));
            _pages.erase(pageToErase);
        }
        auto page = std::make_shared<Page>(PageType::Model, id.string(), id);
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else if (id.type == ResType::Wav) {
        loadEngine();
        auto mp3ReaderFactory = Mp3ReaderFactory();
        auto reader = WavReader(data, mp3ReaderFactory);
        reader.load();
        _audioStream.invoke(reader.stream());

        auto pageToErase = std::find_if(_pages.begin(), _pages.end(), [](auto &page) {
            return page->type == PageType::Audio;
        });
        if (pageToErase != _pages.end()) {
            auto index = std::distance(_pages.begin(), pageToErase);
            _pageRemoving.invoke(PageRemovingEventData(index, pageToErase->get()));
            _pages.erase(pageToErase);
        }
        auto page = std::make_shared<Page>(PageType::Audio, id.string(), id);
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());

    } else {
        return;
    }

    if (id.type == ResType::Mdl) {
        _renderEnabled.invoke(true);
    }
}

PageType MainViewModel::getPageType(ResType type) const {
    if (kFilesPlaintextResTypes.count(type) > 0) {
        return PageType::Text;
    }
    if (isGFFCompatibleResType(type)) {
        return PageType::GFF;
    }
    switch (type) {
    case ResType::TwoDa:
    case ResType::Tlk:
    case ResType::Lip:
    case ResType::Ssf:
        return PageType::Table;
    case ResType::Ncs:
        return PageType::NCS;
    case ResType::Nss:
        return PageType::NSS;
    case ResType::Tga:
    case ResType::Tpc:
        return PageType::Image;
    case ResType::Mdl:
        return PageType::Model;
    case ResType::Wav:
        return PageType::Audio;
    default:
        throw std::invalid_argument("Invalid resource type: " + std::to_string(static_cast<int>(type)));
    }
}

void MainViewModel::loadGameDirectory() {
    auto tslExePath = findFileIgnoreCase(_gamePath, "swkotor2.exe");
    _gameId = tslExePath ? GameID::TSL : GameID::KotOR;

    auto keyPath = getFileIgnoreCase(_gamePath, "chitin.key");
    auto key = FileInputStream(keyPath);
    auto keyReader = KeyReader(key);
    keyReader.load();
    _keyKeys = keyReader.keys();
    _keyFiles = keyReader.files();

    auto tlkPath = getFileIgnoreCase(_gamePath, "dialog.tlk");
    auto tlk = FileInputStream(tlkPath);
    auto tlkReader = TlkReader(tlk);
    tlkReader.load();
    _talkTable = tlkReader.table();

    _routines = std::make_unique<Routines>(_gameId, nullptr, nullptr);
    _routines->init();

    for (auto &file : std::filesystem::directory_iterator(_gamePath)) {
        auto filename = boost::to_lower_copy(file.path().filename().string());
        auto extension = boost::to_lower_copy(file.path().extension().string());
        bool container;
        if ((file.is_directory() && kFilesSubdirectoryWhitelist.count(filename) > 0) ||
            (file.is_regular_file() && kFilesArchiveExtensions.count(extension) > 0)) {
            container = true;
        } else if (file.is_regular_file() && (kFilesExtensionBlacklist.count(extension) == 0 && extension != ".txt")) {
            container = false;
        } else {
            continue;
        }
        auto item = std::make_shared<GameDirectoryItem>();
        item->displayName = filename;
        item->path = file.path();
        item->container = container;
        if (extension == ".tlk") {
            auto resRef = filename.substr(0, filename.size() - 4);
            item->resId = std::make_unique<ResourceId>(resRef, ResType::Tlk);
        }
        _gameDirItems.push_back(std::move(item));
    }
}

void MainViewModel::loadTools() {
    _tools.clear();
    _tools.push_back(std::make_shared<KeyBifTool>());
    _tools.push_back(std::make_shared<ErfTool>());
    _tools.push_back(std::make_shared<RimTool>());
    _tools.push_back(std::make_shared<TwoDaTool>());
    _tools.push_back(std::make_shared<TlkTool>());
    _tools.push_back(std::make_shared<LipTool>());
    _tools.push_back(std::make_shared<SsfTool>());
    _tools.push_back(std::make_shared<GffTool>());
    _tools.push_back(std::make_shared<TpcTool>());
    _tools.push_back(std::make_shared<AudioTool>());
    _tools.push_back(std::make_shared<NcsTool>(_gameId));
}

void MainViewModel::loadEngine() {
    if (_engineLoaded || _gamePath.empty()) {
        return;
    }
    _engineLoadRequested.invoke(true);

    _graphicsOpt.grass = false;
    _graphicsOpt.ssao = false;
    _graphicsOpt.ssr = false;
    _graphicsOpt.fxaa = false;
    _graphicsOpt.sharpen = false;

    _systemModule = std::make_unique<SystemModule>();
    _resourceModule = std::make_unique<ResourceModule>(_gamePath);
    _graphicsModule = std::make_unique<ToolkitGraphicsModule>(_graphicsOpt, *_resourceModule);
    _audioModule = std::make_unique<AudioModule>(_audioOpt, *_resourceModule);
    _sceneModule = std::make_unique<SceneModule>(_graphicsOpt, *_audioModule, *_graphicsModule);

    _systemModule->init();
    _resourceModule->init();
    _graphicsModule->init();
    _audioModule->init();
    _sceneModule->init();

    auto keyPath = getFileIgnoreCase(_gamePath, "chitin.key");
    auto guiTexPackPath = getFileIgnoreCase(_gamePath, "texturepacks/swpc_tex_gui.erf");
    auto tpaTexPackPath = getFileIgnoreCase(_gamePath, "texturepacks/swpc_tex_tpa.erf");
    auto overridePath = getFileIgnoreCase(_gamePath, "override");

    auto &resources = _resourceModule->resources();
    resources.addKEY(keyPath);
    resources.addERF(guiTexPackPath);
    resources.addERF(tpaTexPackPath);
    resources.addFolder(overridePath);

    auto &sceneGraphs = _sceneModule->graphs();
    sceneGraphs.reserve(kSceneMain);
    auto &scene = sceneGraphs.get(kSceneMain);

    _cameraNode = scene.newCamera();

    scene.setActiveCamera(_cameraNode.get());
    scene.setAmbientLightColor(glm::vec3(1.0f));

    _engineLoaded = true;
}

void MainViewModel::decompile(GameDirectoryItemId itemId, bool optimize) {
    auto &item = *_idToGameDirItem.at(itemId);

    withResourceStream(item, [this, &item, &optimize](auto &res) {
        auto nssBytes = ByteBuffer();
        auto nss = MemoryOutputStream(nssBytes);
        NcsTool(_gameId).toNSS(res, nss, *_routines, optimize);

        auto page = std::make_shared<Page>(PageType::NSS, str(boost::format("%s.nss") % item.resId->resRef.value()), *item.resId);
        page->nssContent = std::string(nssBytes.begin(), nssBytes.end());
        _pages.push_back(std::move(page));
        _pageAdded.invoke(_pages.back().get());
    });
}

void MainViewModel::extractArchive(const std::filesystem::path &srcPath, const std::filesystem::path &destPath) {
    auto extension = boost::to_lower_copy(srcPath.extension().string());
    if (extension == ".bif") {
        auto keyPath = getFileIgnoreCase(_gamePath, "chitin.key");
        auto key = FileInputStream(keyPath);
        auto keyReader = KeyReader(key);
        keyReader.load();
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
        auto erf = FileInputStream(srcPath);
        auto erfReader = ErfReader(erf);
        erfReader.load();
        ErfTool().extract(erfReader, srcPath, destPath);
    } else if (extension == ".rim") {
        auto rim = FileInputStream(srcPath);
        auto rimReader = RimReader(rim);
        rimReader.load();
        RimTool().extract(rimReader, srcPath, destPath);
    }
}

void MainViewModel::exportFile(GameDirectoryItemId itemId, const std::filesystem::path &destPath) {
    auto &item = *_idToGameDirItem.at(itemId);
    withResourceStream(item, [&destPath, &item](auto &res) {
        auto exportedPath = destPath;
        exportedPath.append(item.resId->string());
        auto exported = FileOutputStream(exportedPath);
        auto buffer = ByteBuffer();
        buffer.resize(8192);
        bool eof = false;
        while (!eof) {
            int bytesRead = res.read(&buffer[0], buffer.size());
            if (bytesRead < buffer.size()) {
                eof = true;
            }
            exported.write(&buffer[0], bytesRead);
        }
    });
}

void MainViewModel::extractAllBifs(const std::filesystem::path &destPath) {
    auto tool = KeyBifTool();

    auto keyPath = getFileIgnoreCase(_gamePath, "chitin.key");
    auto key = FileInputStream(keyPath);
    auto keyReader = KeyReader(key);
    keyReader.load();

    auto progress = Progress();
    progress.visible = true;
    progress.title = "Extract all BIF archives";
    _progress.invoke(progress);

    int bifIdx = 0;
    for (auto &file : _keyFiles) {
        auto cleanedFilename = boost::replace_all_copy(file.filename, "\\", "/");
        auto bifPath = findFileIgnoreCase(_gamePath, cleanedFilename);
        if (!bifPath) {
            continue;
        }
        progress.value = 100 * bifIdx / static_cast<int>(_keyFiles.size());
        tool.extractBIF(keyReader, bifIdx++, *bifPath, destPath);
        _progress.invoke(progress);
    }

    progress.visible = false;
    _progress.invoke(progress);
}

void MainViewModel::batchConvertTpcToTga(const std::filesystem::path &srcPath, const std::filesystem::path &destPath) {
    std::vector<std::filesystem::path> tpcFiles;
    for (auto &file : std::filesystem::directory_iterator(srcPath)) {
        if (!file.is_regular_file()) {
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
    _progress.invoke(progress);

    auto tool = TpcTool();
    for (size_t i = 0; i < tpcFiles.size(); ++i) {
        progress.value = 100 * static_cast<int>(i / tpcFiles.size());
        _progress.invoke(progress);

        auto &tpcPath = tpcFiles[i];
        tool.toTGA(tpcPath, destPath);
    }

    progress.visible = false;
    _progress.invoke(progress);
}

bool MainViewModel::invokeTool(Operation operation,
                               const std::filesystem::path &srcPath,
                               const std::filesystem::path &destPath) {
    for (auto &tool : _tools) {
        if (!tool->supports(operation, srcPath)) {
            continue;
        }
        tool->invoke(operation, srcPath, destPath, _gamePath);
        return true;
    }
    return false;
}

void MainViewModel::playAnimation(std::string anim, graphics::LipAnimation *lipAnim) {
    if (!_modelNode) {
        return;
    }
    _modelNode->playAnimation(anim, lipAnim, AnimationProperties::fromFlags(AnimationFlags::loop));
    _animationPlaying = true;
}

void MainViewModel::pauseAnimation() {
    if (!_modelNode) {
        return;
    }
    _modelNode->pauseAnimation();
    _animationPlaying = false;
}

void MainViewModel::resumeAnimation() {
    if (!_modelNode) {
        return;
    }
    _modelNode->resumeAnimation();
    _animationPlaying = true;
}

void MainViewModel::setAnimationTime(float time) {
    if (!_modelNode) {
        return;
    }
    _modelNode->setAnimationTime(time);
}

void MainViewModel::update3D() {
    auto ticks = _systemModule->services().clock.ticks();
    if (_lastTicks == 0) {
        _lastTicks = ticks;
    }
    float delta = (ticks - _lastTicks) / 1000.0f;
    _lastTicks = ticks;

    auto &scene = _sceneModule->graphs().get(kSceneMain);
    scene.update(delta);

    if (_modelNode && !_modelNode->animationChannels().empty()) {
        const auto &animChannel = _modelNode->animationChannels().front();
        if (animChannel.anim) {
            float time = animChannel.time;
            float duration = animChannel.lipAnim ? animChannel.lipAnim->length() : animChannel.anim->length();
            _animationProgress.invoke(AnimationProgress {time, duration});
        }
    }
}

void MainViewModel::render3D(int w, int h) {
    auto &scene = _sceneModule->graphs().get(kSceneMain);

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
    _graphicsModule->context().withViewport(glm::ivec4(0, 0, w, h), [this, &output]() {
        _graphicsModule->context().clearColorDepth();
        _graphicsModule->shaders().use(ShaderProgramId::SimpleTexture);
        _graphicsModule->textures().bind(*output);
        _graphicsModule->meshes().quadNDC().draw();
    });
}

void MainViewModel::updateModelTransform() {
    auto transform = glm::mat4(1.0f);
    transform *= glm::rotate(_modelHeading, glm::vec3(0.0f, 0.0f, 1.0f));
    transform *= glm::rotate(_modelPitch, glm::vec3(-1.0f, 0.0f, 0.0f));
    _modelNode->setLocalTransform(transform);
}

void MainViewModel::updateCameraTransform() {
    auto cameraTransform = glm::mat4(1.0f);
    cameraTransform = glm::translate(cameraTransform, _cameraPosition);
    cameraTransform *= glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    cameraTransform *= glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    _cameraNode->setLocalTransform(cameraTransform);
}

void MainViewModel::withResourceStream(const GameDirectoryItem &item, std::function<void(IInputStream &)> block) {
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
            auto bif = FileInputStream(item.path);
            auto bifReader = BifReader(bif);
            bifReader.load();
            if (bifReader.resources().size() <= resIdx) {
                return;
            }
            auto &bifEntry = bifReader.resources().at(resIdx);
            auto resBytes = ByteBuffer();
            resBytes.resize(bifEntry.fileSize);
            bif.seek(bifEntry.offset, SeekOrigin::Begin);
            bif.read(&resBytes[0], bifEntry.fileSize);
            auto res = MemoryInputStream(resBytes);
            block(res);
        } else if (extension == ".erf" || extension == ".sav" || extension == ".mod") {
            auto erf = FileInputStream(item.path);
            auto erfReader = ErfReader(erf);
            erfReader.load();
            auto maybeKey = std::find_if(erfReader.keys().begin(), erfReader.keys().end(), [&item](auto &key) {
                return key.resId == *item.resId;
            });
            if (maybeKey == erfReader.keys().end()) {
                return;
            }
            auto resIdx = std::distance(erfReader.keys().begin(), maybeKey);
            auto &erfEntry = erfReader.resources().at(resIdx);
            auto resBytes = ByteBuffer();
            resBytes.resize(erfEntry.size);
            erf.seek(erfEntry.offset, SeekOrigin::Begin);
            erf.read(&resBytes[0], erfEntry.size);
            auto res = MemoryInputStream(resBytes);
            block(res);
        } else if (extension == ".rim") {
            auto rim = FileInputStream(item.path);
            auto rimReader = RimReader(rim);
            rimReader.load();
            auto maybeRes = std::find_if(rimReader.resources().begin(), rimReader.resources().end(), [&item](auto &res) {
                return res.resId == *item.resId;
            });
            if (maybeRes == rimReader.resources().end()) {
                return;
            }
            auto &rimRes = *maybeRes;
            auto resBytes = ByteBuffer();
            resBytes.resize(rimRes.size);
            rim.seek(rimRes.offset, SeekOrigin::Begin);
            rim.read(&resBytes[0], rimRes.size);
            auto res = MemoryInputStream(resBytes);
            block(res);
        }
    } else {
        auto res = FileInputStream(item.path);
        block(res);
    }
}

void MainViewModel::onViewCreated() {
    loadTools();
}

void MainViewModel::onViewDestroyed() {
    _audioStream.invoke(nullptr);
}

void MainViewModel::onNotebookPageClose(int page) {
    auto pageIterator = _pages.begin();
    std::advance(pageIterator, page);
    auto pageResId = (*pageIterator)->resourceId;
    _pages.erase(pageIterator);

    if (pageResId.type == ResType::Mdl) {
        _renderEnabled.invoke(false);
    }
    if (pageResId.type == ResType::Wav) {
        _audioStream.invoke(nullptr);
    }
}

void MainViewModel::onGameDirectoryChanged(std::filesystem::path path) {
    _gamePath = path;
    _gameDirItems.clear();
    _idToGameDirItem.clear();

    loadGameDirectory();
    loadTools();
}

void MainViewModel::onGameDirectoryItemIdentified(int index, GameDirectoryItemId id) {
    auto &item = _gameDirItems[index];
    item->id = id;
    _idToGameDirItem.insert(std::make_pair(id, item.get()));
}

void MainViewModel::onGameDirectoryItemExpanding(GameDirectoryItemId id) {
    if (_idToGameDirItem.count(id) == 0) {
        return;
    }
    auto &expandingItem = *_idToGameDirItem.at(id);
    if (std::filesystem::is_directory(expandingItem.path)) {
        for (auto &file : std::filesystem::directory_iterator(expandingItem.path)) {
            auto filename = boost::to_lower_copy(file.path().filename().string());
            auto extension = boost::to_lower_copy(file.path().extension().string());
            bool container;
            if (file.is_directory() || kFilesArchiveExtensions.count(extension) > 0) {
                container = true;
            } else if (file.is_regular_file() && kFilesExtensionBlacklist.count(extension) == 0) {
                container = false;
            } else {
                continue;
            }
            auto item = std::make_shared<GameDirectoryItem>();
            item->parentId = expandingItem.id;
            item->displayName = filename;
            item->path = file.path();
            if (!extension.empty()) {
                auto resType = getResTypeByExt(extension.substr(1), false);
                if (resType != ResType::Invalid) {
                    auto resRef = filename.substr(0, filename.size() - 4);
                    item->resId = std::make_shared<ResourceId>(resRef, resType);
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
                    auto item = std::make_shared<GameDirectoryItem>();
                    item->parentId = expandingItem.id;
                    item->displayName = str(boost::format("%s.%s") % key.resId.resRef.value() % getExtByResType(key.resId.type));
                    item->path = expandingItem.path;
                    item->resId = std::make_shared<ResourceId>(key.resId);
                    item->archived = true;
                    _gameDirItems.push_back(std::move(item));
                }
            }
        } else if (boost::ends_with(extension, ".erf") || boost::ends_with(extension, ".sav") || boost::ends_with(extension, ".mod")) {
            auto erf = FileInputStream(expandingItem.path);
            auto erfReader = ErfReader(erf);
            erfReader.load();
            auto &keys = erfReader.keys();
            for (auto &key : keys) {
                auto item = std::make_shared<GameDirectoryItem>();
                item->parentId = expandingItem.id;
                item->displayName = str(boost::format("%s.%s") % key.resId.resRef.value() % getExtByResType(key.resId.type));
                item->path = expandingItem.path;
                item->resId = std::make_shared<ResourceId>(key.resId);
                item->archived = true;
                _gameDirItems.push_back(std::move(item));
            }
        } else if (boost::ends_with(extension, ".rim")) {
            auto rim = FileInputStream(expandingItem.path);
            auto rimReader = RimReader(rim);
            rimReader.load();
            auto &resources = rimReader.resources();
            for (auto &resource : resources) {
                auto item = std::make_shared<GameDirectoryItem>();
                item->parentId = expandingItem.id;
                item->displayName = str(boost::format("%s.%s") % resource.resId.resRef.value() % getExtByResType(resource.resId.type));
                item->path = expandingItem.path;
                item->resId = std::make_shared<ResourceId>(resource.resId);
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

void MainViewModel::onGLCanvasMouseMotion(int x, int y, bool leftDown, bool rightDown) {
    int dx = x - _lastMouseX;
    int dy = y - _lastMouseY;

    if (leftDown) {
        _modelHeading += dx / glm::pi<float>() / 64.0f;
        //_modelPitch += dy / glm::pi<float>() / 64.0f;
        updateModelTransform();
    } else if (rightDown) {
        _cameraPosition.x += dx / static_cast<float>(256.0f);
        _cameraPosition.z += dy / static_cast<float>(256.0f);
        updateCameraTransform();
    }

    _lastMouseX = x;
    _lastMouseY = y;
}

void MainViewModel::onGLCanvasMouseWheel(int delta) {
    _cameraPosition.y = glm::max(0.0f, _cameraPosition.y - glm::clamp(delta, -1, 1));
    updateCameraTransform();
}

} // namespace reone
