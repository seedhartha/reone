# Copyright (c) 2020-2021 The reone project contributors

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.16)
project(reone)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src/external/s3tc)

option(BUILD_TOOLS "build tools executable" OFF)
option(BUILD_LAUNCHER "build launcher executable" OFF)
option(BUILD_TESTS "build unit tests" OFF)

option(USE_EXTERNAL_GLM "use GLM library from external subdirectory" ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system OPTIONAL_COMPONENTS unit_test_framework)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)
find_package(wxWidgets COMPONENTS core base)

if(MSVC)
    find_package(OpenAL CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(FFMPEG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswresample libswscale)
    find_package(Threads REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/external/s3tc)

if(USE_EXTERNAL_GLM)
    include_directories(${CMAKE_SOURCE_DIR}/src/external/glm)
else()
    find_package(glm REQUIRED)
endif()

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

## libcommon static library

set(COMMON_HEADERS
    src/engine/common/cache.h
    src/engine/common/collectionutil.h
    src/engine/common/guardutil.h
    src/engine/common/log.h
    src/engine/common/mediastream.h
    src/engine/common/pathutil.h
    src/engine/common/random.h
    src/engine/common/streamreader.h
    src/engine/common/streamutil.h
    src/engine/common/streamwriter.h
    src/engine/common/timer.h
    src/engine/common/types.h
    src/engine/common/unicode.h)

set(COMMON_SOURCES
    src/engine/common/log.cpp
    src/engine/common/pathutil.cpp
    src/engine/common/random.cpp
    src/engine/common/streamreader.cpp
    src/engine/common/streamutil.cpp
    src/engine/common/streamwriter.cpp
    src/engine/common/unicode.cpp)

add_library(libcommon STATIC ${COMMON_HEADERS} ${COMMON_SOURCES})
set_target_properties(libcommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libcommon PRIVATE src/engine/pch.h)

## END libcommon static library

## libresource static library

set(RESOURCE_HEADERS
    src/engine/resource/2da.h
    src/engine/resource/gffstruct.h
    src/engine/resource/folder.h
    src/engine/resource/format/2dareader.h
    src/engine/resource/format/2dawriter.h
    src/engine/resource/format/bifreader.h
    src/engine/resource/format/binreader.h
    src/engine/resource/format/erfreader.h
    src/engine/resource/format/erfwriter.h
    src/engine/resource/format/gffreader.h
    src/engine/resource/format/gffwriter.h
    src/engine/resource/format/keyreader.h
    src/engine/resource/format/ltrreader.h
    src/engine/resource/format/lytreader.h
    src/engine/resource/format/pereader.h
    src/engine/resource/format/rimreader.h
    src/engine/resource/format/rimwriter.h
    src/engine/resource/format/ssfreader.h
    src/engine/resource/format/tlkreader.h
    src/engine/resource/format/tlkwriter.h
    src/engine/resource/format/visreader.h
    src/engine/resource/keybifprovider.h
    src/engine/resource/resourceprovider.h
    src/engine/resource/resources.h
    src/engine/resource/services.h
    src/engine/resource/strings.h
    src/engine/resource/talktable.h
    src/engine/resource/types.h
    src/engine/resource/typeutil.h)

set(RESOURCE_SOURCES
    src/engine/resource/2da.cpp
    src/engine/resource/gfffield.cpp
    src/engine/resource/gffstruct.cpp
    src/engine/resource/folder.cpp
    src/engine/resource/format/2dareader.cpp
    src/engine/resource/format/2dawriter.cpp
    src/engine/resource/format/bifreader.cpp
    src/engine/resource/format/binreader.cpp
    src/engine/resource/format/erfreader.cpp
    src/engine/resource/format/erfwriter.cpp
    src/engine/resource/format/gffreader.cpp
    src/engine/resource/format/gffwriter.cpp
    src/engine/resource/format/keyreader.cpp
    src/engine/resource/format/ltrreader.cpp
    src/engine/resource/format/lytreader.cpp
    src/engine/resource/format/pereader.cpp
    src/engine/resource/format/rimreader.cpp
    src/engine/resource/format/rimwriter.cpp
    src/engine/resource/format/ssfreader.cpp
    src/engine/resource/format/tlkreader.cpp
    src/engine/resource/format/tlkwriter.cpp
    src/engine/resource/format/visreader.cpp
    src/engine/resource/keybifprovider.cpp
    src/engine/resource/resources.cpp
    src/engine/resource/services.cpp
    src/engine/resource/strings.cpp
    src/engine/resource/talktable.cpp
    src/engine/resource/typeutil.cpp)

add_library(libresource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES})
set_target_properties(libresource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libresource PRIVATE src/engine/pch.h)

## END libresource static library

## libgraphics static library

set(GRAPHICS_HEADERS
    src/engine/graphics/aabb.h
    src/engine/graphics/baryutil.h
    src/engine/graphics/beziercurve.h
    src/engine/graphics/context.h
    src/engine/graphics/cursor.h
    src/engine/graphics/eventhandler.h
    src/engine/graphics/features.h
    src/engine/graphics/font.h
    src/engine/graphics/fonts.h
    src/engine/graphics/framebuffer.h
    src/engine/graphics/lip/animation.h
    src/engine/graphics/lip/lipreader.h
    src/engine/graphics/lip/lips.h
    src/engine/graphics/lip/lipwriter.h
    src/engine/graphics/material.h
    src/engine/graphics/materials.h
    src/engine/graphics/mesh/mesh.h
    src/engine/graphics/mesh/meshes.h
    src/engine/graphics/mesh/vertexattributes.h
    src/engine/graphics/model/animatedproperty.h
    src/engine/graphics/model/animation.h
    src/engine/graphics/model/mdlreader.h
    src/engine/graphics/model/model.h
    src/engine/graphics/model/modelnode.h
    src/engine/graphics/model/models.h
    src/engine/graphics/options.h
    src/engine/graphics/pbribl.h
    src/engine/graphics/pixelutil.h
    src/engine/graphics/renderbuffer.h
    src/engine/graphics/services.h
    src/engine/graphics/shader/shaders.h
    src/engine/graphics/texture/curreader.h
    src/engine/graphics/texture/texture.h
    src/engine/graphics/texture/textures.h
    src/engine/graphics/texture/textureutil.h
    src/engine/graphics/texture/tgareader.h
    src/engine/graphics/texture/tgawriter.h
    src/engine/graphics/texture/tpcreader.h
    src/engine/graphics/texture/txireader.h
    src/engine/graphics/textutil.h
    src/engine/graphics/types.h
    src/engine/graphics/walkmesh/bwmreader.h
    src/engine/graphics/walkmesh/walkmesh.h
    src/engine/graphics/walkmesh/walkmeshes.h
    src/engine/graphics/window.h)

set(GRAPHICS_SOURCES
    src/engine/graphics/aabb.cpp
    src/engine/graphics/context.cpp
    src/engine/graphics/cursor.cpp
    src/engine/graphics/features.cpp
    src/engine/graphics/font.cpp
    src/engine/graphics/fonts.cpp
    src/engine/graphics/framebuffer.cpp
    src/engine/graphics/lip/animation.cpp
    src/engine/graphics/lip/lipreader.cpp
    src/engine/graphics/lip/lips.cpp
    src/engine/graphics/lip/lipwriter.cpp
    src/engine/graphics/materials.cpp
    src/engine/graphics/mesh/mesh.cpp
    src/engine/graphics/mesh/meshes.cpp
    src/engine/graphics/model/animation.cpp
    src/engine/graphics/model/mdlreader.cpp
    src/engine/graphics/model/mdlreader_controllers.cpp
    src/engine/graphics/model/model.cpp
    src/engine/graphics/model/modelnode.cpp
    src/engine/graphics/model/models.cpp
    src/engine/graphics/pbribl.cpp
    src/engine/graphics/pixelutil.cpp
    src/engine/graphics/renderbuffer.cpp
    src/engine/graphics/services.cpp
    src/engine/graphics/shader/shaders.cpp
    src/engine/graphics/shader/shaders_common.cpp
    src/engine/graphics/shader/shaders_pbr.cpp
    src/engine/graphics/shader/shaders_phong.cpp
    src/engine/graphics/texture/curreader.cpp
    src/engine/graphics/texture/texture.cpp
    src/engine/graphics/texture/textures.cpp
    src/engine/graphics/texture/textureutil.cpp
    src/engine/graphics/texture/tgareader.cpp
    src/engine/graphics/texture/tgawriter.cpp
    src/engine/graphics/texture/tpcreader.cpp
    src/engine/graphics/texture/txireader.cpp
    src/engine/graphics/textutil.cpp
    src/engine/graphics/walkmesh/bwmreader.cpp
    src/engine/graphics/walkmesh/walkmesh.cpp
    src/engine/graphics/walkmesh/walkmeshes.cpp
    src/engine/graphics/window.cpp)

add_library(libgraphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES})
set_target_properties(libgraphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libgraphics PRIVATE src/engine/pch.h)

## END libgraphics static library

## libaudio static library

set(AUDIO_HEADERS
    src/engine/audio/files.h
    src/engine/audio/format/mp3reader.h
    src/engine/audio/format/wavreader.h
    src/engine/audio/options.h
    src/engine/audio/player.h
    src/engine/audio/services.h
    src/engine/audio/soundhandle.h
    src/engine/audio/soundinstance.h
    src/engine/audio/stream.h
    src/engine/audio/types.h)

set(AUDIO_SOURCES
    src/engine/audio/files.cpp
    src/engine/audio/format/mp3reader.cpp
    src/engine/audio/format/wavreader.cpp
    src/engine/audio/player.cpp
    src/engine/audio/services.cpp
    src/engine/audio/soundhandle.cpp
    src/engine/audio/soundinstance.cpp
    src/engine/audio/stream.cpp)

add_library(libaudio STATIC ${AUDIO_HEADERS} ${AUDIO_SOURCES})
set_target_properties(libaudio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libaudio PRIVATE src/engine/pch.h)

## END libaudio static library

## libvideo static library

set(VIDEO_HEADERS
    src/engine/video/bikreader.h
    src/engine/video/video.h)

set(VIDEO_SOURCES
    src/engine/video/bikreader.cpp
    src/engine/video/video.cpp)

add_library(libvideo STATIC ${VIDEO_HEADERS} ${VIDEO_SOURCES})
set_target_properties(libvideo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libvideo PRIVATE src/engine/pch.h)

## END libvideo static library

## libscene static library

set(SCENE_HEADERS
    src/engine/scene/animeventlistener.h
    src/engine/scene/animproperties.h
    src/engine/scene/node/camera.h
    src/engine/scene/node/dummy.h
    src/engine/scene/node/emitter.h
    src/engine/scene/node/grass.h
    src/engine/scene/node/light.h
    src/engine/scene/node/mesh.h
    src/engine/scene/node/model.h
    src/engine/scene/node/modelnode.h
    src/engine/scene/node/scenenode.h
    src/engine/scene/nodeelement.h
    src/engine/scene/pipeline/control.h
    src/engine/scene/pipeline/world.h
    src/engine/scene/services.h
    src/engine/scene/scenegraph.h
    src/engine/scene/types.h)

set(SCENE_SOURCES
    src/engine/scene/node/camera.cpp
    src/engine/scene/node/emitter.cpp
    src/engine/scene/node/grass.cpp
    src/engine/scene/node/light.cpp
    src/engine/scene/node/mesh.cpp
    src/engine/scene/node/model.cpp
    src/engine/scene/node/model_animation.cpp
    src/engine/scene/node/modelnode.cpp
    src/engine/scene/node/scenenode.cpp
    src/engine/scene/pipeline/control.cpp
    src/engine/scene/pipeline/world.cpp
    src/engine/scene/services.cpp
    src/engine/scene/scenegraph.cpp)

add_library(libscene STATIC ${SCENE_HEADERS} ${SCENE_SOURCES})
set_target_properties(libscene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libscene PRIVATE src/engine/pch.h)

## END libscene static library

## libgui static library

set(GUI_HEADERS
    src/engine/gui/control/button.h
    src/engine/gui/control/control.h
    src/engine/gui/control/imagebutton.h
    src/engine/gui/control/label.h
    src/engine/gui/control/listbox.h
    src/engine/gui/control/panel.h
    src/engine/gui/control/progressbar.h
    src/engine/gui/control/scrollbar.h
    src/engine/gui/control/slider.h
    src/engine/gui/control/togglebutton.h
    src/engine/gui/gui.h
    src/engine/gui/scenebuilder.h
    src/engine/gui/textinput.h
    src/engine/gui/types.h)

set(GUI_SOURCES
    src/engine/gui/control/button.cpp
    src/engine/gui/control/control.cpp
    src/engine/gui/control/imagebutton.cpp
    src/engine/gui/control/label.cpp
    src/engine/gui/control/listbox.cpp
    src/engine/gui/control/panel.cpp
    src/engine/gui/control/progressbar.cpp
    src/engine/gui/control/slider.cpp
    src/engine/gui/control/scrollbar.cpp
    src/engine/gui/control/togglebutton.cpp
    src/engine/gui/gui.cpp
    src/engine/gui/scenebuilder.cpp
    src/engine/gui/textinput.cpp)

add_library(libgui STATIC ${GUI_HEADERS} ${GUI_SOURCES})
set_target_properties(libgui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libgui PRIVATE src/engine/pch.h)

## END libgui static library

## libscript static library

set(SCRIPT_HEADERS
    src/engine/script/enginetype.h
    src/engine/script/exception/argument.h
    src/engine/script/exception/notimpl.h
    src/engine/script/execution.h
    src/engine/script/executioncontext.h
    src/engine/script/executionstate.h
    src/engine/script/instrutil.h
    src/engine/script/ncsreader.h
    src/engine/script/program.h
    src/engine/script/routine.h
    src/engine/script/routineprovider.h
    src/engine/script/services.h
    src/engine/script/scripts.h
    src/engine/script/types.h
    src/engine/script/variable.h)

set(SCRIPT_SOURCES
    src/engine/script/execution.cpp
    src/engine/script/instrutil.cpp
    src/engine/script/ncsreader.cpp
    src/engine/script/program.cpp
    src/engine/script/routine.cpp
    src/engine/script/services.cpp
    src/engine/script/scripts.cpp
    src/engine/script/variable.cpp)

add_library(libscript STATIC ${SCRIPT_HEADERS} ${SCRIPT_SOURCES})
set_target_properties(libscript PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libscript PRIVATE src/engine/pch.h)

## END libscript static library

## libgame static library

set(GAME_HEADERS
    src/engine/game/action/action.h
    src/engine/game/action/actionfactory.h
    src/engine/game/action/attack.h
    src/engine/game/action/barkstring.h
    src/engine/game/action/castfakespellatlocation.h
    src/engine/game/action/castfakespellatobject.h
    src/engine/game/action/castspellatlocation.h
    src/engine/game/action/castspellatobject.h
    src/engine/game/action/closedoor.h
    src/engine/game/action/docommand.h
    src/engine/game/action/equipitem.h
    src/engine/game/action/equipmostdamagingmelee.h
    src/engine/game/action/equipmostdamagingranged.h
    src/engine/game/action/equipmosteffectivearmor.h
    src/engine/game/action/follow.h
    src/engine/game/action/followleader.h
    src/engine/game/action/followowner.h
    src/engine/game/action/forcefollowobject.h
    src/engine/game/action/giveitem.h
    src/engine/game/action/interactobject.h
    src/engine/game/action/jumptolocation.h
    src/engine/game/action/jumptoobject.h
    src/engine/game/action/locationaction.h
    src/engine/game/action/lockobject.h
    src/engine/game/action/moveawayfromlocation.h
    src/engine/game/action/moveawayfromobject.h
    src/engine/game/action/movetolocation.h
    src/engine/game/action/movetoobject.h
    src/engine/game/action/movetopoint.h
    src/engine/game/action/objectaction.h
    src/engine/game/action/opencontainer.h
    src/engine/game/action/opendoor.h
    src/engine/game/action/openlock.h
    src/engine/game/action/pauseconversation.h
    src/engine/game/action/pickupitem.h
    src/engine/game/action/playanimation.h
    src/engine/game/action/putdownitem.h
    src/engine/game/action/randomwalk.h
    src/engine/game/action/resumeconversation.h
    src/engine/game/action/speakstring.h
    src/engine/game/action/speakstringbystrref.h
    src/engine/game/action/startconversation.h
    src/engine/game/action/surrendertoenemies.h
    src/engine/game/action/switchweapons.h
    src/engine/game/action/takeitem.h
    src/engine/game/action/unequipitem.h
    src/engine/game/action/unlockobject.h
    src/engine/game/action/usefeat.h
    src/engine/game/action/useskill.h
    src/engine/game/action/usetalentatlocation.h
    src/engine/game/action/usetalentonobject.h
    src/engine/game/action/wait.h
    src/engine/game/animationutil.h
    src/engine/game/camera/animated.h
    src/engine/game/camera/camera.h
    src/engine/game/camera/camerastyle.h
    src/engine/game/camera/dialog.h
    src/engine/game/camera/firstperson.h
    src/engine/game/camera/static.h
    src/engine/game/camera/thirdperson.h
    src/engine/game/character.h
    src/engine/game/combat/combat.h
    src/engine/game/console.h
    src/engine/game/contextaction.h
    src/engine/game/cursors.h
    src/engine/game/d20/attributes.h
    src/engine/game/d20/class.h
    src/engine/game/d20/classes.h
    src/engine/game/d20/feat.h
    src/engine/game/d20/feats.h
    src/engine/game/d20/savingthrows.h
    src/engine/game/d20/skill.h
    src/engine/game/d20/skills.h
    src/engine/game/d20/spell.h
    src/engine/game/d20/spells.h
    src/engine/game/dialog.h
    src/engine/game/effect/abilitydecrease.h
    src/engine/game/effect/abilityincrease.h
    src/engine/game/effect/acdecrease.h
    src/engine/game/effect/acincrease.h
    src/engine/game/effect/areaofeffect.h
    src/engine/game/effect/assureddeflection.h
    src/engine/game/effect/assuredhit.h
    src/engine/game/effect/attackdecrease.h
    src/engine/game/effect/attackincrease.h
    src/engine/game/effect/beam.h
    src/engine/game/effect/blasterdeflectiondecrease.h
    src/engine/game/effect/blasterdeflectionincrease.h
    src/engine/game/effect/blind.h
    src/engine/game/effect/bodyfuel.h
    src/engine/game/effect/choke.h
    src/engine/game/effect/concealment.h
    src/engine/game/effect/confused.h
    src/engine/game/effect/crush.h
    src/engine/game/effect/cutscenehorrified.h
    src/engine/game/effect/cutsceneparalyze.h
    src/engine/game/effect/cutscenestunned.h
    src/engine/game/effect/damage.h
    src/engine/game/effect/damagedecrease.h
    src/engine/game/effect/damageforcepoints.h
    src/engine/game/effect/damageimmunitydecrease.h
    src/engine/game/effect/damageimmunityincrease.h
    src/engine/game/effect/damageincrease.h
    src/engine/game/effect/damagereduction.h
    src/engine/game/effect/damageresistance.h
    src/engine/game/effect/damageshield.h
    src/engine/game/effect/death.h
    src/engine/game/effect/disguise.h
    src/engine/game/effect/dispelmagicall.h
    src/engine/game/effect/dispelmagicbest.h
    src/engine/game/effect/droidconfused.h
    src/engine/game/effect/droidscramble.h
    src/engine/game/effect/droidstun.h
    src/engine/game/effect/effect.h
    src/engine/game/effect/effectfactory.h
    src/engine/game/effect/entangle.h
    src/engine/game/effect/factionmodifier.h
    src/engine/game/effect/forcebody.h
    src/engine/game/effect/forcedrain.h
    src/engine/game/effect/forcefizzle.h
    src/engine/game/effect/forcejump.h
    src/engine/game/effect/forcepushed.h
    src/engine/game/effect/forcepushtargeted.h
    src/engine/game/effect/forceresistancedecrease.h
    src/engine/game/effect/forceresistanceincrease.h
    src/engine/game/effect/forceresisted.h
    src/engine/game/effect/forceshield.h
    src/engine/game/effect/forcesight.h
    src/engine/game/effect/fpregenmodifier.h
    src/engine/game/effect/frightened.h
    src/engine/game/effect/fury.h
    src/engine/game/effect/haste.h
    src/engine/game/effect/heal.h
    src/engine/game/effect/healforcepoints.h
    src/engine/game/effect/hitpointschangewhendying.h
    src/engine/game/effect/horrified.h
    src/engine/game/effect/immunity.h
    src/engine/game/effect/invisibility.h
    src/engine/game/effect/knockdown.h
    src/engine/game/effect/lightsaberthrow.h
    src/engine/game/effect/linkeffects.h
    src/engine/game/effect/mindtrick.h
    src/engine/game/effect/misschance.h
    src/engine/game/effect/modifyattacks.h
    src/engine/game/effect/movementspeeddecrease.h
    src/engine/game/effect/movementspeedincrease.h
    src/engine/game/effect/paralyze.h
    src/engine/game/effect/poison.h
    src/engine/game/effect/psychicstatic.h
    src/engine/game/effect/regenerate.h
    src/engine/game/effect/resurrection.h
    src/engine/game/effect/savingthrowdecrease.h
    src/engine/game/effect/savingthrowincrease.h
    src/engine/game/effect/seeinvisible.h
    src/engine/game/effect/skilldecrease.h
    src/engine/game/effect/skillincrease.h
    src/engine/game/effect/sleep.h
    src/engine/game/effect/spellimmunity.h
    src/engine/game/effect/spelllevelabsorption.h
    src/engine/game/effect/stunned.h
    src/engine/game/effect/temporaryforcepoints.h
    src/engine/game/effect/temporaryhitpoints.h
    src/engine/game/effect/timestop.h
    src/engine/game/effect/trueseeing.h
    src/engine/game/effect/visual.h
    src/engine/game/effect/vpregenmodifier.h
    src/engine/game/effect/whirlwind.h
    src/engine/game/event.h
    src/engine/game/footstepsounds.h
    src/engine/game/game.h
    src/engine/game/gui/barkbubble.h
    src/engine/game/gui/chargen/abilities.h
    src/engine/game/gui/chargen/chargen.h
    src/engine/game/gui/chargen/classselect.h
    src/engine/game/gui/chargen/custom.h
    src/engine/game/gui/chargen/feats.h
    src/engine/game/gui/chargen/levelup.h
    src/engine/game/gui/chargen/nameentry.h
    src/engine/game/gui/chargen/portraitselect.h
    src/engine/game/gui/chargen/quick.h
    src/engine/game/gui/chargen/quickorcustom.h
    src/engine/game/gui/chargen/skills.h
    src/engine/game/gui/computer.h
    src/engine/game/gui/container.h
    src/engine/game/gui/conversation.h
    src/engine/game/gui/dialog.h
    src/engine/game/gui/gui.h
    src/engine/game/gui/hud.h
    src/engine/game/gui/ingame/abilities.h
    src/engine/game/gui/ingame/character.h
    src/engine/game/gui/ingame/equip.h
    src/engine/game/gui/ingame/ingame.h
    src/engine/game/gui/ingame/inventory.h
    src/engine/game/gui/ingame/journal.h
    src/engine/game/gui/ingame/map.h
    src/engine/game/gui/ingame/messages.h
    src/engine/game/gui/ingame/options.h
    src/engine/game/gui/loadscreen.h
    src/engine/game/gui/mainmenu.h
    src/engine/game/gui/partyselect.h
    src/engine/game/gui/profileoverlay.h
    src/engine/game/gui/saveload.h
    src/engine/game/gui/selectoverlay.h
    src/engine/game/gui/sounds.h
    src/engine/game/kotor.h
    src/engine/game/location.h
    src/engine/game/map.h
    src/engine/game/object/area.h
    src/engine/game/object/creature.h
    src/engine/game/object/door.h
    src/engine/game/object/encounter.h
    src/engine/game/object/item.h
    src/engine/game/object/module.h
    src/engine/game/object/object.h
    src/engine/game/object/objectfactory.h
    src/engine/game/object/placeable.h
    src/engine/game/object/placeablecamera.h
    src/engine/game/object/sound.h
    src/engine/game/object/spatial.h
    src/engine/game/object/trigger.h
    src/engine/game/object/waypoint.h
    src/engine/game/options.h
    src/engine/game/party.h
    src/engine/game/path.h
    src/engine/game/pathfinder.h
    src/engine/game/player.h
    src/engine/game/portrait.h
    src/engine/game/portraits.h
    src/engine/game/reputes.h
    src/engine/game/room.h
    src/engine/game/savedgame.h
    src/engine/game/script/routine/impl/argutil.h
    src/engine/game/script/routine/impl/declarations.h
    src/engine/game/script/routine/impl/objectutil.h
    src/engine/game/script/routines.h
    src/engine/game/script/runner.h
    src/engine/game/services.h
    src/engine/game/soundsets.h
    src/engine/game/surface.h
    src/engine/game/surfaces.h
    src/engine/game/talent.h
    src/engine/game/tsl.h
    src/engine/game/types.h)

set(GAME_SOURCES
    src/engine/game/action/action.cpp
    src/engine/game/action/attack.cpp
    src/engine/game/action/barkstring.cpp
    src/engine/game/action/castfakespellatlocation.cpp
    src/engine/game/action/castfakespellatobject.cpp
    src/engine/game/action/castspellatlocation.cpp
    src/engine/game/action/castspellatobject.cpp
    src/engine/game/action/closedoor.cpp
    src/engine/game/action/docommand.cpp
    src/engine/game/action/equipitem.cpp
    src/engine/game/action/equipmostdamagingmelee.cpp
    src/engine/game/action/equipmostdamagingranged.cpp
    src/engine/game/action/equipmosteffectivearmor.cpp
    src/engine/game/action/follow.cpp
    src/engine/game/action/followleader.cpp
    src/engine/game/action/followowner.cpp
    src/engine/game/action/forcefollowobject.cpp
    src/engine/game/action/giveitem.cpp
    src/engine/game/action/interactobject.cpp
    src/engine/game/action/jumptolocation.cpp
    src/engine/game/action/jumptoobject.cpp
    src/engine/game/action/lockobject.cpp
    src/engine/game/action/moveawayfromlocation.cpp
    src/engine/game/action/moveawayfromobject.cpp
    src/engine/game/action/movetolocation.cpp
    src/engine/game/action/movetoobject.cpp
    src/engine/game/action/movetopoint.cpp
    src/engine/game/action/opencontainer.cpp
    src/engine/game/action/opendoor.cpp
    src/engine/game/action/openlock.cpp
    src/engine/game/action/pauseconversation.cpp
    src/engine/game/action/pickupitem.cpp
    src/engine/game/action/playanimation.cpp
    src/engine/game/action/putdownitem.cpp
    src/engine/game/action/randomwalk.cpp
    src/engine/game/action/resumeconversation.cpp
    src/engine/game/action/speakstring.cpp
    src/engine/game/action/speakstringbystrref.cpp
    src/engine/game/action/startconversation.cpp
    src/engine/game/action/surrendertoenemies.cpp
    src/engine/game/action/switchweapons.cpp
    src/engine/game/action/takeitem.cpp
    src/engine/game/action/unequipitem.cpp
    src/engine/game/action/unlockobject.cpp
    src/engine/game/action/usefeat.cpp
    src/engine/game/action/useskill.cpp
    src/engine/game/action/usetalentatlocation.cpp
    src/engine/game/action/usetalentonobject.cpp
    src/engine/game/action/wait.cpp
    src/engine/game/animationutil.cpp
    src/engine/game/camera/animated.cpp
    src/engine/game/camera/camera.cpp
    src/engine/game/camera/camerastyle.cpp
    src/engine/game/camera/dialog.cpp
    src/engine/game/camera/firstperson.cpp
    src/engine/game/camera/static.cpp
    src/engine/game/camera/thirdperson.cpp
    src/engine/game/combat/combat.cpp
    src/engine/game/console.cpp
    src/engine/game/cursors.cpp
    src/engine/game/d20/attributes.cpp
    src/engine/game/d20/class.cpp
    src/engine/game/d20/classes.cpp
    src/engine/game/d20/feats.cpp
    src/engine/game/d20/skills.cpp
    src/engine/game/d20/spells.cpp
    src/engine/game/dialog.cpp
    src/engine/game/effect/abilitydecrease.cpp
    src/engine/game/effect/abilityincrease.cpp
    src/engine/game/effect/acdecrease.cpp
    src/engine/game/effect/acincrease.cpp
    src/engine/game/effect/areaofeffect.cpp
    src/engine/game/effect/assureddeflection.cpp
    src/engine/game/effect/assuredhit.cpp
    src/engine/game/effect/attackdecrease.cpp
    src/engine/game/effect/attackincrease.cpp
    src/engine/game/effect/beam.cpp
    src/engine/game/effect/blasterdeflectiondecrease.cpp
    src/engine/game/effect/blasterdeflectionincrease.cpp
    src/engine/game/effect/blind.cpp
    src/engine/game/effect/bodyfuel.cpp
    src/engine/game/effect/choke.cpp
    src/engine/game/effect/concealment.cpp
    src/engine/game/effect/confused.cpp
    src/engine/game/effect/crush.cpp
    src/engine/game/effect/cutscenehorrified.cpp
    src/engine/game/effect/cutsceneparalyze.cpp
    src/engine/game/effect/cutscenestunned.cpp
    src/engine/game/effect/damage.cpp
    src/engine/game/effect/damagedecrease.cpp
    src/engine/game/effect/damageforcepoints.cpp
    src/engine/game/effect/damageimmunitydecrease.cpp
    src/engine/game/effect/damageimmunityincrease.cpp
    src/engine/game/effect/damageincrease.cpp
    src/engine/game/effect/damagereduction.cpp
    src/engine/game/effect/damageresistance.cpp
    src/engine/game/effect/damageshield.cpp
    src/engine/game/effect/death.cpp
    src/engine/game/effect/disguise.cpp
    src/engine/game/effect/dispelmagicall.cpp
    src/engine/game/effect/dispelmagicbest.cpp
    src/engine/game/effect/droidconfused.cpp
    src/engine/game/effect/droidscramble.cpp
    src/engine/game/effect/droidstun.cpp
    src/engine/game/effect/effect.cpp
    src/engine/game/effect/entangle.cpp
    src/engine/game/effect/factionmodifier.cpp
    src/engine/game/effect/forcebody.cpp
    src/engine/game/effect/forcedrain.cpp
    src/engine/game/effect/forcefizzle.cpp
    src/engine/game/effect/forcejump.cpp
    src/engine/game/effect/forcepushed.cpp
    src/engine/game/effect/forcepushtargeted.cpp
    src/engine/game/effect/forceresistancedecrease.cpp
    src/engine/game/effect/forceresistanceincrease.cpp
    src/engine/game/effect/forceresisted.cpp
    src/engine/game/effect/forceshield.cpp
    src/engine/game/effect/forcesight.cpp
    src/engine/game/effect/fpregenmodifier.cpp
    src/engine/game/effect/frightened.cpp
    src/engine/game/effect/fury.cpp
    src/engine/game/effect/haste.cpp
    src/engine/game/effect/heal.cpp
    src/engine/game/effect/healforcepoints.cpp
    src/engine/game/effect/hitpointschangewhendying.cpp
    src/engine/game/effect/horrified.cpp
    src/engine/game/effect/immunity.cpp
    src/engine/game/effect/invisibility.cpp
    src/engine/game/effect/knockdown.cpp
    src/engine/game/effect/lightsaberthrow.cpp
    src/engine/game/effect/linkeffects.cpp
    src/engine/game/effect/mindtrick.cpp
    src/engine/game/effect/misschance.cpp
    src/engine/game/effect/modifyattacks.cpp
    src/engine/game/effect/movementspeeddecrease.cpp
    src/engine/game/effect/movementspeedincrease.cpp
    src/engine/game/effect/paralyze.cpp
    src/engine/game/effect/poison.cpp
    src/engine/game/effect/psychicstatic.cpp
    src/engine/game/effect/regenerate.cpp
    src/engine/game/effect/resurrection.cpp
    src/engine/game/effect/savingthrowdecrease.cpp
    src/engine/game/effect/savingthrowincrease.cpp
    src/engine/game/effect/seeinvisible.cpp
    src/engine/game/effect/skilldecrease.cpp
    src/engine/game/effect/skillincrease.cpp
    src/engine/game/effect/sleep.cpp
    src/engine/game/effect/spellimmunity.cpp
    src/engine/game/effect/spelllevelabsorption.cpp
    src/engine/game/effect/stunned.cpp
    src/engine/game/effect/temporaryforcepoints.cpp
    src/engine/game/effect/temporaryhitpoints.cpp
    src/engine/game/effect/timestop.cpp
    src/engine/game/effect/trueseeing.cpp
    src/engine/game/effect/visual.cpp
    src/engine/game/effect/vpregenmodifier.cpp
    src/engine/game/effect/whirlwind.cpp
    src/engine/game/footstepsounds.cpp
    src/engine/game/game.cpp
    src/engine/game/gui/barkbubble.cpp
    src/engine/game/gui/chargen/abilities.cpp
    src/engine/game/gui/chargen/chargen.cpp
    src/engine/game/gui/chargen/classselect.cpp
    src/engine/game/gui/chargen/custom.cpp
    src/engine/game/gui/chargen/feats.cpp
    src/engine/game/gui/chargen/levelup.cpp
    src/engine/game/gui/chargen/nameentry.cpp
    src/engine/game/gui/chargen/portraitselect.cpp
    src/engine/game/gui/chargen/quick.cpp
    src/engine/game/gui/chargen/quickorcustom.cpp
    src/engine/game/gui/chargen/skills.cpp
    src/engine/game/gui/computer.cpp
    src/engine/game/gui/container.cpp
    src/engine/game/gui/conversation.cpp
    src/engine/game/gui/dialog.cpp
    src/engine/game/gui/gui.cpp
    src/engine/game/gui/hud.cpp
    src/engine/game/gui/ingame/abilities.cpp
    src/engine/game/gui/ingame/character.cpp
    src/engine/game/gui/ingame/equip.cpp
    src/engine/game/gui/ingame/ingame.cpp
    src/engine/game/gui/ingame/inventory.cpp
    src/engine/game/gui/ingame/journal.cpp
    src/engine/game/gui/ingame/map.cpp
    src/engine/game/gui/ingame/messages.cpp
    src/engine/game/gui/ingame/options.cpp
    src/engine/game/gui/loadscreen.cpp
    src/engine/game/gui/mainmenu.cpp
    src/engine/game/gui/partyselect.cpp
    src/engine/game/gui/profileoverlay.cpp
    src/engine/game/gui/saveload.cpp
    src/engine/game/gui/selectoverlay.cpp
    src/engine/game/gui/sounds.cpp
    src/engine/game/kotor.cpp
    src/engine/game/map.cpp
    src/engine/game/object/area.cpp
    src/engine/game/object/creature.cpp
    src/engine/game/object/door.cpp
    src/engine/game/object/encounter.cpp
    src/engine/game/object/item.cpp
    src/engine/game/object/module.cpp
    src/engine/game/object/object.cpp
    src/engine/game/object/objectfactory.cpp
    src/engine/game/object/placeable.cpp
    src/engine/game/object/placeablecamera.cpp
    src/engine/game/object/sound.cpp
    src/engine/game/object/spatial.cpp
    src/engine/game/object/trigger.cpp
    src/engine/game/object/waypoint.cpp
    src/engine/game/party.cpp
    src/engine/game/path.cpp
    src/engine/game/pathfinder.cpp
    src/engine/game/player.cpp
    src/engine/game/portraits.cpp
    src/engine/game/reputes.cpp
    src/engine/game/room.cpp
    src/engine/game/script/routine/impl/action.cpp
    src/engine/game/script/routine/impl/ambientsound.cpp
    src/engine/game/script/routine/impl/argutil.cpp
    src/engine/game/script/routine/impl/convert.cpp
    src/engine/game/script/routine/impl/effect.cpp
    src/engine/game/script/routine/impl/encounter.cpp
    src/engine/game/script/routine/impl/event.cpp
    src/engine/game/script/routine/impl/log.cpp
    src/engine/game/script/routine/impl/main.cpp
    src/engine/game/script/routine/impl/math.cpp
    src/engine/game/script/routine/impl/minigame.cpp
    src/engine/game/script/routine/impl/movie.cpp
    src/engine/game/script/routine/impl/music.cpp
    src/engine/game/script/routine/impl/party.cpp
    src/engine/game/script/routine/impl/perception.cpp
    src/engine/game/script/routine/impl/planet.cpp
    src/engine/game/script/routine/impl/random.cpp
    src/engine/game/script/routine/impl/soundobject.cpp
    src/engine/game/script/routine/impl/stealth.cpp
    src/engine/game/script/routine/impl/string.cpp
    src/engine/game/script/routine/impl/talent.cpp
    src/engine/game/script/routine/impl/time.cpp
    src/engine/game/script/routine/impl/vector.cpp
    src/engine/game/script/routine/registrar/kotor.h
    src/engine/game/script/routine/registrar/kotor.cpp
    src/engine/game/script/routine/registrar/registrar.h
    src/engine/game/script/routine/registrar/tsl.h
    src/engine/game/script/routine/registrar/tsl.cpp
    src/engine/game/script/runner.cpp
    src/engine/game/services.cpp
    src/engine/game/soundsets.cpp
    src/engine/game/surfaces.cpp
    src/engine/game/tsl.cpp)

add_library(libgame STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(libgame PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libgame PRIVATE src/engine/pch.h)

## END libgame static library

## reone executable

set(REONE_HEADERS
    src/engine/engine.h
    src/engine/pch.h)

set(REONE_SOURCES
    src/engine/engine.cpp
    src/engine/main.cpp)

if(WIN32)
    list(APPEND REONE_SOURCES res/reone.rc)
endif()

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_precompile_headers(reone PRIVATE src/engine/pch.h)

target_link_libraries(reone PRIVATE
    libgame libscript libgui libscene libvideo libaudio libgraphics libresource libcommon
    libs3tc
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    GLEW::GLEW
    ${OPENGL_LIBRARIES}
    ${MAD_LIBRARY}
    ${FFMPEG_LIBRARIES})

if(MSVC)
    target_link_libraries(reone PRIVATE SDL2::SDL2 OpenAL::OpenAL)
else()
    target_link_libraries(reone PRIVATE ${SDL2_LIBRARIES} ${OpenAL_LIBRARIES} Threads::Threads -latomic)
endif()

list(APPEND InstallTargets reone)

## END reone executable

## reone-tools executable

if(BUILD_TOOLS)
    set(TOOLS_HEADERS
        src/tools/program.h
        src/tools/tools.h
        src/tools/types.h)

    set(TOOLS_SOURCES
        src/tools/2datool.cpp
        src/tools/audiotool.cpp
        src/tools/erftool.cpp
        src/tools/gfftool.cpp
        src/tools/keybiftool.cpp
        src/tools/liptool.cpp
        src/tools/main.cpp
        src/tools/program.cpp
        src/tools/pthtool.cpp
        src/tools/rimtool.cpp
        src/tools/tlktool.cpp
        src/tools/tpctool.cpp)

    add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})
    set_target_properties(reone-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    target_precompile_headers(reone-tools PRIVATE src/engine/pch.h)

    target_link_libraries(reone-tools PRIVATE
        libgraphics libresource libcommon
        libs3tc
        GLEW::GLEW
        ${OPENGL_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

    if(MSVC)
        target_link_libraries(reone PRIVATE SDL2::SDL2)
    else()
        target_link_libraries(reone PRIVATE ${SDL2_LIBRARIES})
    endif()
    list(APPEND InstallTargets reone-tools)
endif()

## END reone-tools executable

## reone-launcher executable

if(BUILD_LAUNCHER)
    if(wxWidgets_FOUND)
        set(LAUNCHER_HEADERS
            src/launcher/app.h
            src/launcher/frame.h)

        set(LAUNCHER_SOURCES
            src/launcher/app.cpp
            src/launcher/frame.cpp)

        if(WIN32)
            list(APPEND LAUNCHER_SOURCES res/reone.rc)
            add_executable(reone-launcher WIN32 ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES})
        else()
            add_executable(reone-launcher ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES})
        endif()

        include(${wxWidgets_USE_FILE})
        target_link_libraries(reone-launcher ${wxWidgets_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

        set_target_properties(reone-launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
        target_precompile_headers(reone-launcher PRIVATE src/engine/pch.h)
        list(APPEND InstallTargets reone-launcher)
    else()
        message(WARNING "wxWidgets not found - launcher cannot be built")
    endif()

endif()

## END reone-launcher executable

## Unit tests

if(BUILD_TESTS)
    set(TEST_SOURCES
        tests/common/cache.cpp
        tests/common/mediastream.cpp
        tests/common/streamreader.cpp
        tests/common/timer.cpp
        tests/common/unicode.cpp
        tests/game/pathfinder.cpp
        tests/main.cpp
        tests/resource/gffstruct.cpp
        tests/script/execution.cpp)

    add_executable(reone-tests ${TEST_SOURCES})
    target_link_libraries(reone-tests PRIVATE libgame libscript libresource libcommon ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})
    if(MSVC)
        target_link_libraries(reone-tests PRIVATE SDL2::SDL2)
    else()
        target_link_libraries(reone-tests PRIVATE ${SDL2_LIBRARIES})
    endif()
    target_precompile_headers(reone-tests PRIVATE src/engine/pch.h)

    enable_testing()
    add_test(reone-tests reone-tests)
endif()

## END Unit tests

## Installation

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
endif()

install(TARGETS ${InstallTargets} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(TARGETS libcommon libresource libgraphics libaudio libvideo libscript libgame DESTINATION ${CMAKE_INSTALL_LIBDIR})

## END Installation
