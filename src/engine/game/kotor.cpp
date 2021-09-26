/*
 * Copyright (c) 2020-2021 The reone project contributors
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

#include "kotor.h"

#include "../common/pathutil.h"
#include "../resource/resources.h"

using namespace std;

using namespace reone::audio;
using namespace reone::graphics;
using namespace reone::resource;
using namespace reone::scene;
using namespace reone::script;

namespace fs = boost::filesystem;

namespace reone {

namespace game {

static constexpr char kPatchFilename[] = "patch.erf";
static constexpr char kWavesDirectoryName[] = "streamwaves";
static constexpr char kExeFilename[] = "swkotor.exe";

static vector<string> g_nonTransientLipFiles {"global.mod", "localization.mod"};

KotOR::KotOR(
    fs::path path,
    Options options,
    ActionFactory &actionFactory,
    Classes &classes,
    Combat &combat,
    Cursors &cursors,
    EffectFactory &effectFactory,
    Feats &feats,
    FootstepSounds &footstepSounds,
    GUISounds &guiSounds,
    ObjectFactory &objectFactory,
    Party &party,
    Portraits &portraits,
    Reputes &reputes,
    ScriptRunner &scriptRunner,
    Skills &skills,
    SoundSets &soundSets,
    Surfaces &surfaces,
    AudioFiles &audioFiles,
    AudioPlayer &audioPlayer,
    Context &context,
    Features &features,
    Fonts &fonts,
    Lips &lips,
    Materials &materials,
    Meshes &meshes,
    Models &models,
    PBRIBL &pbrIbl,
    Shaders &shaders,
    Textures &textures,
    Walkmeshes &walkmeshes,
    Window &window,
    SceneGraph &sceneGraph,
    WorldRenderPipeline &worldRenderPipeline,
    Scripts &scripts,
    Resources &resources,
    Strings &strings) :
    Game(
        GameID::KotOR,
        move(path),
        move(options),
        actionFactory,
        classes,
        combat,
        cursors,
        effectFactory,
        feats,
        footstepSounds,
        guiSounds,
        objectFactory,
        party,
        portraits,
        reputes,
        scriptRunner,
        skills,
        soundSets,
        surfaces,
        audioFiles,
        audioPlayer,
        context,
        features,
        fonts,
        lips,
        materials,
        meshes,
        models,
        pbrIbl,
        shaders,
        textures,
        walkmeshes,
        window,
        sceneGraph,
        worldRenderPipeline,
        scripts,
        resources,
        strings) {
    _mainMenuMusicResRef = "mus_theme_cult";
    _charGenMusicResRef = "mus_theme_rep";
    _charGenLoadScreenResRef = "load_chargen";

    _guiColorBase = glm::vec3(0.0f, 0.639216f, 0.952941f);
    _guiColorHilight = glm::vec3(0.980392f, 1.0f, 0.0f);
    _guiColorDisabled = glm::vec3(0.0f, 0.349020f, 0.549020f);
}

void KotOR::initResourceProviders() {
    _resources.indexKeyFile(getPathIgnoreCase(_path, kKeyFilename));
    _resources.indexErfFile(getPathIgnoreCase(_path, kPatchFilename));

    fs::path texPacksPath(getPathIgnoreCase(_path, kTexturePackDirectoryName));
    _resources.indexErfFile(getPathIgnoreCase(texPacksPath, kGUITexturePackFilename));
    _resources.indexErfFile(getPathIgnoreCase(texPacksPath, kTexturePackFilename));

    _resources.indexDirectory(getPathIgnoreCase(_path, kMusicDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(_path, kSoundsDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(_path, kWavesDirectoryName));

    fs::path lipsPath(getPathIgnoreCase(_path, kLipsDirectoryName));
    for (auto &filename : g_nonTransientLipFiles) {
        _resources.indexErfFile(getPathIgnoreCase(lipsPath, filename));
    }

    _resources.indexExeFile(getPathIgnoreCase(_path, kExeFilename));
    _resources.indexDirectory(getPathIgnoreCase(_path, kOverrideDirectoryName));
    _resources.indexDirectory(getPathIgnoreCase(fs::current_path(), kDataDirectoryName));
}

} // namespace game

} // namespace reone
