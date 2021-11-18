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

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/external/s3tc)

option(BUILD_TOOLS "build tools executable" OFF)
option(BUILD_LAUNCHER "build launcher executable" OFF)
option(BUILD_TESTS "build unit tests" OFF)

option(ENABLE_VIDEO "enable video playback" ON)

option(USE_EXTERNAL_GLM "use GLM library from external subdirectory" ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system OPTIONAL_COMPONENTS regex unit_test_framework)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)
find_package(wxWidgets COMPONENTS core base)

if(MSVC)
    find_package(OpenAL CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    find_package(Threads REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/src/external/s3tc)

if(ENABLE_VIDEO)
    if(MSVC)
        find_package(FFMPEG REQUIRED)
    else()
        pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswresample libswscale)
    endif()
    include_directories(${FFMPEG_INCLUDE_DIRS})
endif()

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
    src/engine/common/async/executor.h
    src/engine/common/async/impl/task.h
    src/engine/common/async/task.h
    src/engine/common/cache.h
    src/engine/common/collectionutil.h
    src/engine/common/exception/validation.h
    src/engine/common/guardutil.h
    src/engine/common/logutil.h
    src/engine/common/mediastream.h
    src/engine/common/pathutil.h
    src/engine/common/randomutil.h
    src/engine/common/streamreader.h
    src/engine/common/streamutil.h
    src/engine/common/streamwriter.h
    src/engine/common/threadutil.h
    src/engine/common/timer.h
    src/engine/common/types.h
    src/engine/common/unicodeutil.h)

set(COMMON_SOURCES
    src/engine/common/async/executor.cpp
    src/engine/common/async/impl/task.cpp
    src/engine/common/logutil.cpp
    src/engine/common/pathutil.cpp
    src/engine/common/randomutil.cpp
    src/engine/common/streamreader.cpp
    src/engine/common/streamutil.cpp
    src/engine/common/streamwriter.cpp
    src/engine/common/threadutil.cpp
    src/engine/common/unicodeutil.cpp)

add_library(libcommon STATIC ${COMMON_HEADERS} ${COMMON_SOURCES})
set_target_properties(libcommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libcommon PRIVATE src/engine/pch.h)
target_link_libraries(libcommon PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

if(NOT MSVC)
    target_link_libraries(libcommon PRIVATE Threads::Threads -latomic)
endif()

## END libcommon static library

## libresource static library

set(RESOURCE_HEADERS
    src/engine/resource/2da.h
    src/engine/resource/2das.h
    src/engine/resource/gffs.h
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
    src/engine/resource/format/ssfwriter.h
    src/engine/resource/format/tlkreader.h
    src/engine/resource/format/tlkwriter.h
    src/engine/resource/format/visreader.h
    src/engine/resource/id.h
    src/engine/resource/keybifprovider.h
    src/engine/resource/resourceprovider.h
    src/engine/resource/resources.h
    src/engine/resource/strings.h
    src/engine/resource/talktable.h
    src/engine/resource/types.h
    src/engine/resource/typeutil.h)

set(RESOURCE_SOURCES
    src/engine/resource/2da.cpp
    src/engine/resource/2das.cpp
    src/engine/resource/gfffield.cpp
    src/engine/resource/gffs.cpp
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
    src/engine/resource/format/ssfwriter.cpp
    src/engine/resource/format/tlkreader.cpp
    src/engine/resource/format/tlkwriter.cpp
    src/engine/resource/format/visreader.cpp
    src/engine/resource/keybifprovider.cpp
    src/engine/resource/resources.cpp
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
    src/engine/graphics/lip/format/lipreader.h
    src/engine/graphics/lip/format/lipwriter.h
    src/engine/graphics/lip/lips.h
    src/engine/graphics/material.h
    src/engine/graphics/materials.h
    src/engine/graphics/mesh/mesh.h
    src/engine/graphics/mesh/meshes.h
    src/engine/graphics/mesh/vertexattributes.h
    src/engine/graphics/model/animatedproperty.h
    src/engine/graphics/model/animation.h
    src/engine/graphics/model/format/mdlreader.h
    src/engine/graphics/model/model.h
    src/engine/graphics/model/modelnode.h
    src/engine/graphics/model/models.h
    src/engine/graphics/options.h
    src/engine/graphics/pbribl.h
    src/engine/graphics/pixelutil.h
    src/engine/graphics/renderbuffer.h
    src/engine/graphics/shader/shaders.h
    src/engine/graphics/texture/format/curreader.h
    src/engine/graphics/texture/format/tgareader.h
    src/engine/graphics/texture/format/tgawriter.h
    src/engine/graphics/texture/format/tpcreader.h
    src/engine/graphics/texture/format/txireader.h
    src/engine/graphics/texture/texture.h
    src/engine/graphics/texture/textures.h
    src/engine/graphics/texture/textureutil.h
    src/engine/graphics/textutil.h
    src/engine/graphics/types.h
    src/engine/graphics/walkmesh/format/bwmreader.h
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
    src/engine/graphics/lip/format/lipreader.cpp
    src/engine/graphics/lip/format/lipwriter.cpp
    src/engine/graphics/lip/lips.cpp
    src/engine/graphics/materials.cpp
    src/engine/graphics/mesh/mesh.cpp
    src/engine/graphics/mesh/meshes.cpp
    src/engine/graphics/model/animation.cpp
    src/engine/graphics/model/format/mdlreader.cpp
    src/engine/graphics/model/model.cpp
    src/engine/graphics/model/modelnode.cpp
    src/engine/graphics/model/models.cpp
    src/engine/graphics/pbribl.cpp
    src/engine/graphics/pixelutil.cpp
    src/engine/graphics/renderbuffer.cpp
    src/engine/graphics/shader/shaders.cpp
    src/engine/graphics/shader/shaders_common.cpp
    src/engine/graphics/shader/shaders_pbr.cpp
    src/engine/graphics/shader/shaders_phong.cpp
    src/engine/graphics/texture/format/curreader.cpp
    src/engine/graphics/texture/format/tgareader.cpp
    src/engine/graphics/texture/format/tgawriter.cpp
    src/engine/graphics/texture/format/tpcreader.cpp
    src/engine/graphics/texture/format/txireader.cpp
    src/engine/graphics/texture/texture.cpp
    src/engine/graphics/texture/textures.cpp
    src/engine/graphics/texture/textureutil.cpp
    src/engine/graphics/textutil.cpp
    src/engine/graphics/walkmesh/format/bwmreader.cpp
    src/engine/graphics/walkmesh/walkmesh.cpp
    src/engine/graphics/walkmesh/walkmeshes.cpp
    src/engine/graphics/window.cpp)

add_library(libgraphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES})
set_target_properties(libgraphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libgraphics PRIVATE src/engine/pch.h)
target_link_libraries(libgraphics PUBLIC libs3tc GLEW::GLEW ${OPENGL_LIBRARIES})

if(MSVC)
    target_link_libraries(libgraphics PUBLIC SDL2::SDL2)
else()
    target_link_libraries(libgraphics PUBLIC ${SDL2_LIBRARIES})
endif()

## END libgraphics static library

## libaudio static library

set(AUDIO_HEADERS
    src/engine/audio/files.h
    src/engine/audio/format/mp3reader.h
    src/engine/audio/format/wavreader.h
    src/engine/audio/options.h
    src/engine/audio/player.h
    src/engine/audio/soundhandle.h
    src/engine/audio/soundinstance.h
    src/engine/audio/stream.h
    src/engine/audio/types.h)

set(AUDIO_SOURCES
    src/engine/audio/files.cpp
    src/engine/audio/format/mp3reader.cpp
    src/engine/audio/format/wavreader.cpp
    src/engine/audio/player.cpp
    src/engine/audio/soundhandle.cpp
    src/engine/audio/soundinstance.cpp
    src/engine/audio/stream.cpp)

add_library(libaudio STATIC ${AUDIO_HEADERS} ${AUDIO_SOURCES})
set_target_properties(libaudio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libaudio PRIVATE src/engine/pch.h)
target_link_libraries(libaudio PUBLIC ${MAD_LIBRARY})
if(MSVC)
    target_link_libraries(libaudio PUBLIC OpenAL::OpenAL)
else()
    target_link_libraries(libaudio PUBLIC ${OpenAL_LIBRARIES})
endif()

## END libaudio static library

## libvideo static library

set(VIDEO_HEADERS
    src/engine/video/format/bikreader.h
    src/engine/video/video.h)

set(VIDEO_SOURCES
    src/engine/video/format/bikreader.cpp
    src/engine/video/video.cpp)

add_library(libvideo STATIC ${VIDEO_HEADERS} ${VIDEO_SOURCES})
set_target_properties(libvideo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libvideo PRIVATE src/engine/pch.h)

if(ENABLE_VIDEO)
    target_compile_definitions(libvideo PRIVATE R_ENABLE_VIDEO)
    target_link_libraries(libvideo PUBLIC ${FFMPEG_LIBRARIES})
endif()

## END libvideo static library

## libscene static library

set(SCENE_HEADERS
    src/engine/scene/animeventlistener.h
    src/engine/scene/animproperties.h
    src/engine/scene/graph.h
    src/engine/scene/graphs.h
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
    src/engine/scene/types.h)

set(SCENE_SOURCES
    src/engine/scene/graph.cpp
    src/engine/scene/graphs.cpp
    src/engine/scene/node/camera.cpp
    src/engine/scene/node/emitter.cpp
    src/engine/scene/node/grass.cpp
    src/engine/scene/node/light.cpp
    src/engine/scene/node/mesh.cpp
    src/engine/scene/node/model.cpp
    src/engine/scene/node/modelnode.cpp
    src/engine/scene/node/scenenode.cpp
    src/engine/scene/pipeline/control.cpp
    src/engine/scene/pipeline/world.cpp)

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
    src/engine/gui/sceneinitializer.h
    src/engine/gui/textinput.h
    src/engine/gui/types.h)

set(GUI_SOURCES
    src/engine/gui/control/control.cpp
    src/engine/gui/control/imagebutton.cpp
    src/engine/gui/control/listbox.cpp
    src/engine/gui/control/progressbar.cpp
    src/engine/gui/control/scrollbar.cpp
    src/engine/gui/control/togglebutton.cpp
    src/engine/gui/gui.cpp
    src/engine/gui/sceneinitializer.cpp
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
    src/engine/script/format/ncsreader.h
    src/engine/script/format/ncswriter.h
    src/engine/script/instrutil.h
    src/engine/script/program.h
    src/engine/script/routine.h
    src/engine/script/routineprovider.h
    src/engine/script/scripts.h
    src/engine/script/types.h
    src/engine/script/variable.h)

set(SCRIPT_SOURCES
    src/engine/script/execution.cpp
    src/engine/script/format/ncsreader.cpp
    src/engine/script/format/ncswriter.cpp
    src/engine/script/instrutil.cpp
    src/engine/script/program.cpp
    src/engine/script/routine.cpp
    src/engine/script/scripts.cpp
    src/engine/script/variable.cpp)

add_library(libscript STATIC ${SCRIPT_HEADERS} ${SCRIPT_SOURCES})
set_target_properties(libscript PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libscript PRIVATE src/engine/pch.h)

## END libscript static library

## libgamecore static library

set(GAMECORE_HEADERS
    src/engine/game/core/action/action.h
    src/engine/game/core/action/attack.h
    src/engine/game/core/action/barkstring.h
    src/engine/game/core/action/castfakespellatlocation.h
    src/engine/game/core/action/castfakespellatobject.h
    src/engine/game/core/action/castspellatlocation.h
    src/engine/game/core/action/castspellatobject.h
    src/engine/game/core/action/closedoor.h
    src/engine/game/core/action/context.h
    src/engine/game/core/action/docommand.h
    src/engine/game/core/action/equipitem.h
    src/engine/game/core/action/equipmostdamagingmelee.h
    src/engine/game/core/action/equipmostdamagingranged.h
    src/engine/game/core/action/equipmosteffectivearmor.h
    src/engine/game/core/action/factory.h
    src/engine/game/core/action/follow.h
    src/engine/game/core/action/followleader.h
    src/engine/game/core/action/followowner.h
    src/engine/game/core/action/forcefollowobject.h
    src/engine/game/core/action/giveitem.h
    src/engine/game/core/action/interactobject.h
    src/engine/game/core/action/jumptolocation.h
    src/engine/game/core/action/jumptoobject.h
    src/engine/game/core/action/locationaction.h
    src/engine/game/core/action/lockobject.h
    src/engine/game/core/action/moveawayfromlocation.h
    src/engine/game/core/action/moveawayfromobject.h
    src/engine/game/core/action/movetolocation.h
    src/engine/game/core/action/movetoobject.h
    src/engine/game/core/action/movetopoint.h
    src/engine/game/core/action/objectaction.h
    src/engine/game/core/action/opencontainer.h
    src/engine/game/core/action/opendoor.h
    src/engine/game/core/action/openlock.h
    src/engine/game/core/action/pauseconversation.h
    src/engine/game/core/action/pickupitem.h
    src/engine/game/core/action/playanimation.h
    src/engine/game/core/action/putdownitem.h
    src/engine/game/core/action/randomwalk.h
    src/engine/game/core/action/resumeconversation.h
    src/engine/game/core/action/speakstring.h
    src/engine/game/core/action/speakstringbystrref.h
    src/engine/game/core/action/startconversation.h
    src/engine/game/core/action/surrendertoenemies.h
    src/engine/game/core/action/switchweapons.h
    src/engine/game/core/action/takeitem.h
    src/engine/game/core/action/unequipitem.h
    src/engine/game/core/action/unlockobject.h
    src/engine/game/core/action/usefeat.h
    src/engine/game/core/action/useskill.h
    src/engine/game/core/action/usetalentatlocation.h
    src/engine/game/core/action/usetalentonobject.h
    src/engine/game/core/action/wait.h
    src/engine/game/core/animationutil.h
    src/engine/game/core/camera/animated.h
    src/engine/game/core/camera/camera.h
    src/engine/game/core/camera/camerastyle.h
    src/engine/game/core/camera/dialog.h
    src/engine/game/core/camera/firstperson.h
    src/engine/game/core/camera/static.h
    src/engine/game/core/camera/thirdperson.h
    src/engine/game/core/character.h
    src/engine/game/core/combat.h
    src/engine/game/core/console.h
    src/engine/game/core/contextaction.h
    src/engine/game/core/cursors.h
    src/engine/game/core/d20/attributes.h
    src/engine/game/core/d20/class.h
    src/engine/game/core/d20/classes.h
    src/engine/game/core/d20/feat.h
    src/engine/game/core/d20/feats.h
    src/engine/game/core/d20/savingthrows.h
    src/engine/game/core/d20/skill.h
    src/engine/game/core/d20/skills.h
    src/engine/game/core/d20/spell.h
    src/engine/game/core/d20/spells.h
    src/engine/game/core/dialog.h
    src/engine/game/core/effect/abilitydecrease.h
    src/engine/game/core/effect/abilityincrease.h
    src/engine/game/core/effect/acdecrease.h
    src/engine/game/core/effect/acincrease.h
    src/engine/game/core/effect/areaofeffect.h
    src/engine/game/core/effect/assureddeflection.h
    src/engine/game/core/effect/assuredhit.h
    src/engine/game/core/effect/attackdecrease.h
    src/engine/game/core/effect/attackincrease.h
    src/engine/game/core/effect/beam.h
    src/engine/game/core/effect/blasterdeflectiondecrease.h
    src/engine/game/core/effect/blasterdeflectionincrease.h
    src/engine/game/core/effect/blind.h
    src/engine/game/core/effect/bodyfuel.h
    src/engine/game/core/effect/choke.h
    src/engine/game/core/effect/concealment.h
    src/engine/game/core/effect/confused.h
    src/engine/game/core/effect/crush.h
    src/engine/game/core/effect/cutscenehorrified.h
    src/engine/game/core/effect/cutsceneparalyze.h
    src/engine/game/core/effect/cutscenestunned.h
    src/engine/game/core/effect/damage.h
    src/engine/game/core/effect/damagedecrease.h
    src/engine/game/core/effect/damageforcepoints.h
    src/engine/game/core/effect/damageimmunitydecrease.h
    src/engine/game/core/effect/damageimmunityincrease.h
    src/engine/game/core/effect/damageincrease.h
    src/engine/game/core/effect/damagereduction.h
    src/engine/game/core/effect/damageresistance.h
    src/engine/game/core/effect/damageshield.h
    src/engine/game/core/effect/death.h
    src/engine/game/core/effect/disguise.h
    src/engine/game/core/effect/dispelmagicall.h
    src/engine/game/core/effect/dispelmagicbest.h
    src/engine/game/core/effect/droidconfused.h
    src/engine/game/core/effect/droidscramble.h
    src/engine/game/core/effect/droidstun.h
    src/engine/game/core/effect/effect.h
    src/engine/game/core/effect/entangle.h
    src/engine/game/core/effect/factionmodifier.h
    src/engine/game/core/effect/factory.h
    src/engine/game/core/effect/forcebody.h
    src/engine/game/core/effect/forcedrain.h
    src/engine/game/core/effect/forcefizzle.h
    src/engine/game/core/effect/forcejump.h
    src/engine/game/core/effect/forcepushed.h
    src/engine/game/core/effect/forcepushtargeted.h
    src/engine/game/core/effect/forceresistancedecrease.h
    src/engine/game/core/effect/forceresistanceincrease.h
    src/engine/game/core/effect/forceresisted.h
    src/engine/game/core/effect/forceshield.h
    src/engine/game/core/effect/forcesight.h
    src/engine/game/core/effect/fpregenmodifier.h
    src/engine/game/core/effect/frightened.h
    src/engine/game/core/effect/fury.h
    src/engine/game/core/effect/haste.h
    src/engine/game/core/effect/heal.h
    src/engine/game/core/effect/healforcepoints.h
    src/engine/game/core/effect/hitpointschangewhendying.h
    src/engine/game/core/effect/horrified.h
    src/engine/game/core/effect/immunity.h
    src/engine/game/core/effect/invisibility.h
    src/engine/game/core/effect/knockdown.h
    src/engine/game/core/effect/lightsaberthrow.h
    src/engine/game/core/effect/linkeffects.h
    src/engine/game/core/effect/mindtrick.h
    src/engine/game/core/effect/misschance.h
    src/engine/game/core/effect/modifyattacks.h
    src/engine/game/core/effect/movementspeeddecrease.h
    src/engine/game/core/effect/movementspeedincrease.h
    src/engine/game/core/effect/paralyze.h
    src/engine/game/core/effect/poison.h
    src/engine/game/core/effect/psychicstatic.h
    src/engine/game/core/effect/regenerate.h
    src/engine/game/core/effect/resurrection.h
    src/engine/game/core/effect/savingthrowdecrease.h
    src/engine/game/core/effect/savingthrowincrease.h
    src/engine/game/core/effect/seeinvisible.h
    src/engine/game/core/effect/skilldecrease.h
    src/engine/game/core/effect/skillincrease.h
    src/engine/game/core/effect/sleep.h
    src/engine/game/core/effect/spellimmunity.h
    src/engine/game/core/effect/spelllevelabsorption.h
    src/engine/game/core/effect/stunned.h
    src/engine/game/core/effect/temporaryforcepoints.h
    src/engine/game/core/effect/temporaryhitpoints.h
    src/engine/game/core/effect/timestop.h
    src/engine/game/core/effect/trueseeing.h
    src/engine/game/core/effect/visual.h
    src/engine/game/core/effect/vpregenmodifier.h
    src/engine/game/core/effect/whirlwind.h
    src/engine/game/core/event.h
    src/engine/game/core/footstepsounds.h
    src/engine/game/core/game.h
    src/engine/game/core/location.h
    src/engine/game/core/map.h
    src/engine/game/core/object/area.h
    src/engine/game/core/object/creature.h
    src/engine/game/core/object/door.h
    src/engine/game/core/object/encounter.h
    src/engine/game/core/object/factory.h
    src/engine/game/core/object/item.h
    src/engine/game/core/object/module.h
    src/engine/game/core/object/object.h
    src/engine/game/core/object/placeable.h
    src/engine/game/core/object/placeablecamera.h
    src/engine/game/core/object/sound.h
    src/engine/game/core/object/spatial.h
    src/engine/game/core/object/trigger.h
    src/engine/game/core/object/waypoint.h
    src/engine/game/core/options.h
    src/engine/game/core/party.h
    src/engine/game/core/path.h
    src/engine/game/core/pathfinder.h
    src/engine/game/core/player.h
    src/engine/game/core/portrait.h
    src/engine/game/core/portraits.h
    src/engine/game/core/profileoverlay.h
    src/engine/game/core/reputes.h
    src/engine/game/core/room.h
    src/engine/game/core/savedgame.h
    src/engine/game/core/services.h
    src/engine/game/core/script/routine/argutil.h
    src/engine/game/core/script/routine/context.h
    src/engine/game/core/script/routine/declarations.h
    src/engine/game/core/script/routine/iroutines.h
    src/engine/game/core/script/routine/objectutil.h
    src/engine/game/core/script/routine/registrar.h
    src/engine/game/core/script/routine/routines.h
    src/engine/game/core/script/runner.h
    src/engine/game/core/soundsets.h
    src/engine/game/core/surface.h
    src/engine/game/core/surfaces.h
    src/engine/game/core/talent.h
    src/engine/game/core/types.h)

set(GAMECORE_SOURCES
    src/engine/game/core/action/action.cpp
    src/engine/game/core/action/attack.cpp
    src/engine/game/core/action/barkstring.cpp
    src/engine/game/core/action/castfakespellatlocation.cpp
    src/engine/game/core/action/castfakespellatobject.cpp
    src/engine/game/core/action/castspellatlocation.cpp
    src/engine/game/core/action/castspellatobject.cpp
    src/engine/game/core/action/closedoor.cpp
    src/engine/game/core/action/docommand.cpp
    src/engine/game/core/action/equipitem.cpp
    src/engine/game/core/action/equipmostdamagingmelee.cpp
    src/engine/game/core/action/equipmostdamagingranged.cpp
    src/engine/game/core/action/equipmosteffectivearmor.cpp
    src/engine/game/core/action/follow.cpp
    src/engine/game/core/action/followleader.cpp
    src/engine/game/core/action/followowner.cpp
    src/engine/game/core/action/forcefollowobject.cpp
    src/engine/game/core/action/giveitem.cpp
    src/engine/game/core/action/interactobject.cpp
    src/engine/game/core/action/jumptolocation.cpp
    src/engine/game/core/action/jumptoobject.cpp
    src/engine/game/core/action/lockobject.cpp
    src/engine/game/core/action/moveawayfromlocation.cpp
    src/engine/game/core/action/moveawayfromobject.cpp
    src/engine/game/core/action/movetolocation.cpp
    src/engine/game/core/action/movetoobject.cpp
    src/engine/game/core/action/movetopoint.cpp
    src/engine/game/core/action/opencontainer.cpp
    src/engine/game/core/action/opendoor.cpp
    src/engine/game/core/action/openlock.cpp
    src/engine/game/core/action/pauseconversation.cpp
    src/engine/game/core/action/pickupitem.cpp
    src/engine/game/core/action/playanimation.cpp
    src/engine/game/core/action/putdownitem.cpp
    src/engine/game/core/action/randomwalk.cpp
    src/engine/game/core/action/resumeconversation.cpp
    src/engine/game/core/action/speakstring.cpp
    src/engine/game/core/action/speakstringbystrref.cpp
    src/engine/game/core/action/startconversation.cpp
    src/engine/game/core/action/surrendertoenemies.cpp
    src/engine/game/core/action/switchweapons.cpp
    src/engine/game/core/action/takeitem.cpp
    src/engine/game/core/action/unequipitem.cpp
    src/engine/game/core/action/unlockobject.cpp
    src/engine/game/core/action/usefeat.cpp
    src/engine/game/core/action/useskill.cpp
    src/engine/game/core/action/usetalentatlocation.cpp
    src/engine/game/core/action/usetalentonobject.cpp
    src/engine/game/core/action/wait.cpp
    src/engine/game/core/animationutil.cpp
    src/engine/game/core/camera/animated.cpp
    src/engine/game/core/camera/camera.cpp
    src/engine/game/core/camera/camerastyle.cpp
    src/engine/game/core/camera/dialog.cpp
    src/engine/game/core/camera/firstperson.cpp
    src/engine/game/core/camera/static.cpp
    src/engine/game/core/camera/thirdperson.cpp
    src/engine/game/core/combat.cpp
    src/engine/game/core/console.cpp
    src/engine/game/core/cursors.cpp
    src/engine/game/core/d20/attributes.cpp
    src/engine/game/core/d20/class.cpp
    src/engine/game/core/d20/classes.cpp
    src/engine/game/core/d20/feats.cpp
    src/engine/game/core/d20/skills.cpp
    src/engine/game/core/d20/spells.cpp
    src/engine/game/core/dialog.cpp
    src/engine/game/core/effect/abilitydecrease.cpp
    src/engine/game/core/effect/abilityincrease.cpp
    src/engine/game/core/effect/acdecrease.cpp
    src/engine/game/core/effect/acincrease.cpp
    src/engine/game/core/effect/areaofeffect.cpp
    src/engine/game/core/effect/assureddeflection.cpp
    src/engine/game/core/effect/assuredhit.cpp
    src/engine/game/core/effect/attackdecrease.cpp
    src/engine/game/core/effect/attackincrease.cpp
    src/engine/game/core/effect/beam.cpp
    src/engine/game/core/effect/blasterdeflectiondecrease.cpp
    src/engine/game/core/effect/blasterdeflectionincrease.cpp
    src/engine/game/core/effect/blind.cpp
    src/engine/game/core/effect/bodyfuel.cpp
    src/engine/game/core/effect/choke.cpp
    src/engine/game/core/effect/concealment.cpp
    src/engine/game/core/effect/confused.cpp
    src/engine/game/core/effect/crush.cpp
    src/engine/game/core/effect/cutscenehorrified.cpp
    src/engine/game/core/effect/cutsceneparalyze.cpp
    src/engine/game/core/effect/cutscenestunned.cpp
    src/engine/game/core/effect/damage.cpp
    src/engine/game/core/effect/damagedecrease.cpp
    src/engine/game/core/effect/damageforcepoints.cpp
    src/engine/game/core/effect/damageimmunitydecrease.cpp
    src/engine/game/core/effect/damageimmunityincrease.cpp
    src/engine/game/core/effect/damageincrease.cpp
    src/engine/game/core/effect/damagereduction.cpp
    src/engine/game/core/effect/damageresistance.cpp
    src/engine/game/core/effect/damageshield.cpp
    src/engine/game/core/effect/death.cpp
    src/engine/game/core/effect/disguise.cpp
    src/engine/game/core/effect/dispelmagicall.cpp
    src/engine/game/core/effect/dispelmagicbest.cpp
    src/engine/game/core/effect/droidconfused.cpp
    src/engine/game/core/effect/droidscramble.cpp
    src/engine/game/core/effect/droidstun.cpp
    src/engine/game/core/effect/effect.cpp
    src/engine/game/core/effect/entangle.cpp
    src/engine/game/core/effect/factionmodifier.cpp
    src/engine/game/core/effect/forcebody.cpp
    src/engine/game/core/effect/forcedrain.cpp
    src/engine/game/core/effect/forcefizzle.cpp
    src/engine/game/core/effect/forcejump.cpp
    src/engine/game/core/effect/forcepushed.cpp
    src/engine/game/core/effect/forcepushtargeted.cpp
    src/engine/game/core/effect/forceresistancedecrease.cpp
    src/engine/game/core/effect/forceresistanceincrease.cpp
    src/engine/game/core/effect/forceresisted.cpp
    src/engine/game/core/effect/forceshield.cpp
    src/engine/game/core/effect/forcesight.cpp
    src/engine/game/core/effect/fpregenmodifier.cpp
    src/engine/game/core/effect/frightened.cpp
    src/engine/game/core/effect/fury.cpp
    src/engine/game/core/effect/haste.cpp
    src/engine/game/core/effect/heal.cpp
    src/engine/game/core/effect/healforcepoints.cpp
    src/engine/game/core/effect/hitpointschangewhendying.cpp
    src/engine/game/core/effect/horrified.cpp
    src/engine/game/core/effect/immunity.cpp
    src/engine/game/core/effect/invisibility.cpp
    src/engine/game/core/effect/knockdown.cpp
    src/engine/game/core/effect/lightsaberthrow.cpp
    src/engine/game/core/effect/linkeffects.cpp
    src/engine/game/core/effect/mindtrick.cpp
    src/engine/game/core/effect/misschance.cpp
    src/engine/game/core/effect/modifyattacks.cpp
    src/engine/game/core/effect/movementspeeddecrease.cpp
    src/engine/game/core/effect/movementspeedincrease.cpp
    src/engine/game/core/effect/paralyze.cpp
    src/engine/game/core/effect/poison.cpp
    src/engine/game/core/effect/psychicstatic.cpp
    src/engine/game/core/effect/regenerate.cpp
    src/engine/game/core/effect/resurrection.cpp
    src/engine/game/core/effect/savingthrowdecrease.cpp
    src/engine/game/core/effect/savingthrowincrease.cpp
    src/engine/game/core/effect/seeinvisible.cpp
    src/engine/game/core/effect/skilldecrease.cpp
    src/engine/game/core/effect/skillincrease.cpp
    src/engine/game/core/effect/sleep.cpp
    src/engine/game/core/effect/spellimmunity.cpp
    src/engine/game/core/effect/spelllevelabsorption.cpp
    src/engine/game/core/effect/stunned.cpp
    src/engine/game/core/effect/temporaryforcepoints.cpp
    src/engine/game/core/effect/temporaryhitpoints.cpp
    src/engine/game/core/effect/timestop.cpp
    src/engine/game/core/effect/trueseeing.cpp
    src/engine/game/core/effect/visual.cpp
    src/engine/game/core/effect/vpregenmodifier.cpp
    src/engine/game/core/effect/whirlwind.cpp
    src/engine/game/core/footstepsounds.cpp
    src/engine/game/core/game.cpp
    src/engine/game/core/gui/loadscreen.h
    src/engine/game/core/map.cpp
    src/engine/game/core/object/area.cpp
    src/engine/game/core/object/creature.cpp
    src/engine/game/core/object/door.cpp
    src/engine/game/core/object/encounter.cpp
    src/engine/game/core/object/factory.cpp
    src/engine/game/core/object/item.cpp
    src/engine/game/core/object/module.cpp
    src/engine/game/core/object/object.cpp
    src/engine/game/core/object/placeable.cpp
    src/engine/game/core/object/placeablecamera.cpp
    src/engine/game/core/object/sound.cpp
    src/engine/game/core/object/spatial.cpp
    src/engine/game/core/object/trigger.cpp
    src/engine/game/core/object/waypoint.cpp
    src/engine/game/core/party.cpp
    src/engine/game/core/path.cpp
    src/engine/game/core/pathfinder.cpp
    src/engine/game/core/player.cpp
    src/engine/game/core/portraits.cpp
    src/engine/game/core/profileoverlay.cpp
    src/engine/game/core/reputes.cpp
    src/engine/game/core/room.cpp
    src/engine/game/core/script/routine/argutil.cpp
    src/engine/game/core/script/routine/impl/action.cpp
    src/engine/game/core/script/routine/impl/ambientsound.cpp
    src/engine/game/core/script/routine/impl/convert.cpp
    src/engine/game/core/script/routine/impl/effect.cpp
    src/engine/game/core/script/routine/impl/encounter.cpp
    src/engine/game/core/script/routine/impl/event.cpp
    src/engine/game/core/script/routine/impl/log.cpp
    src/engine/game/core/script/routine/impl/main.cpp
    src/engine/game/core/script/routine/impl/math.cpp
    src/engine/game/core/script/routine/impl/minigame.cpp
    src/engine/game/core/script/routine/impl/movie.cpp
    src/engine/game/core/script/routine/impl/music.cpp
    src/engine/game/core/script/routine/impl/party.cpp
    src/engine/game/core/script/routine/impl/perception.cpp
    src/engine/game/core/script/routine/impl/planet.cpp
    src/engine/game/core/script/routine/impl/random.cpp
    src/engine/game/core/script/routine/impl/soundobject.cpp
    src/engine/game/core/script/routine/impl/stealth.cpp
    src/engine/game/core/script/routine/impl/string.cpp
    src/engine/game/core/script/routine/impl/talent.cpp
    src/engine/game/core/script/routine/impl/time.cpp
    src/engine/game/core/script/routine/impl/vector.cpp
    src/engine/game/core/script/runner.cpp
    src/engine/game/core/soundsets.cpp
    src/engine/game/core/surfaces.cpp)

add_library(libgamecore STATIC ${GAMECORE_HEADERS} ${GAMECORE_SOURCES})
set_target_properties(libgamecore PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libgamecore PRIVATE src/engine/pch.h)
target_link_libraries(libgamecore PUBLIC libscript libgui libscene libvideo libaudio libgraphics libresource libcommon)

## END libgamecore static library

## liblimbo static library

set(LIMBO_HEADERS
    src/engine/game/limbo/limbo.h
    src/engine/game/limbo/routine/registrar.h)

set(LIMBO_SOURCES
    src/engine/game/limbo/limbo.cpp
    src/engine/game/limbo/routine/registrar.cpp)

add_library(liblimbo STATIC ${LIMBO_HEADERS} ${LIMBO_SOURCES})
set_target_properties(liblimbo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(liblimbo PRIVATE src/engine/pch.h)
target_link_libraries(liblimbo PUBLIC libgamecore)

## END liblimbo static library

## libkotor static library

set(KOTOR_HEADERS
    src/engine/game/kotor/gui/barkbubble.h
    src/engine/game/kotor/gui/chargen/abilities.h
    src/engine/game/kotor/gui/chargen/chargen.h
    src/engine/game/kotor/gui/chargen/classselect.h
    src/engine/game/kotor/gui/chargen/custom.h
    src/engine/game/kotor/gui/chargen/feats.h
    src/engine/game/kotor/gui/chargen/levelup.h
    src/engine/game/kotor/gui/chargen/nameentry.h
    src/engine/game/kotor/gui/chargen/portraitselect.h
    src/engine/game/kotor/gui/chargen/quick.h
    src/engine/game/kotor/gui/chargen/quickorcustom.h
    src/engine/game/kotor/gui/chargen/skills.h
    src/engine/game/kotor/gui/computer.h
    src/engine/game/kotor/gui/container.h
    src/engine/game/kotor/gui/conversation.h
    src/engine/game/kotor/gui/dialog.h
    src/engine/game/kotor/gui/gui.h
    src/engine/game/kotor/gui/hud.h
    src/engine/game/kotor/gui/ingame/abilities.h
    src/engine/game/kotor/gui/ingame/character.h
    src/engine/game/kotor/gui/ingame/equip.h
    src/engine/game/kotor/gui/ingame/ingame.h
    src/engine/game/kotor/gui/ingame/inventory.h
    src/engine/game/kotor/gui/ingame/journal.h
    src/engine/game/kotor/gui/ingame/map.h
    src/engine/game/kotor/gui/ingame/messages.h
    src/engine/game/kotor/gui/ingame/options.h
    src/engine/game/kotor/gui/loadscreen.h
    src/engine/game/kotor/gui/mainmenu.h
    src/engine/game/kotor/gui/partyselect.h
    src/engine/game/kotor/gui/saveload.h
    src/engine/game/kotor/gui/selectoverlay.h
    src/engine/game/kotor/gui/sounds.h
    src/engine/game/kotor/kotor.h
    src/engine/game/kotor/routine/registrar.h)

set(KOTOR_SOURCES
    src/engine/game/kotor/gui/barkbubble.cpp
    src/engine/game/kotor/gui/chargen/abilities.cpp
    src/engine/game/kotor/gui/chargen/chargen.cpp
    src/engine/game/kotor/gui/chargen/classselect.cpp
    src/engine/game/kotor/gui/chargen/custom.cpp
    src/engine/game/kotor/gui/chargen/feats.cpp
    src/engine/game/kotor/gui/chargen/levelup.cpp
    src/engine/game/kotor/gui/chargen/nameentry.cpp
    src/engine/game/kotor/gui/chargen/portraitselect.cpp
    src/engine/game/kotor/gui/chargen/quick.cpp
    src/engine/game/kotor/gui/chargen/quickorcustom.cpp
    src/engine/game/kotor/gui/chargen/skills.cpp
    src/engine/game/kotor/gui/computer.cpp
    src/engine/game/kotor/gui/container.cpp
    src/engine/game/kotor/gui/conversation.cpp
    src/engine/game/kotor/gui/dialog.cpp
    src/engine/game/kotor/gui/gui.cpp
    src/engine/game/kotor/gui/hud.cpp
    src/engine/game/kotor/gui/ingame/abilities.cpp
    src/engine/game/kotor/gui/ingame/character.cpp
    src/engine/game/kotor/gui/ingame/equip.cpp
    src/engine/game/kotor/gui/ingame/ingame.cpp
    src/engine/game/kotor/gui/ingame/inventory.cpp
    src/engine/game/kotor/gui/ingame/journal.cpp
    src/engine/game/kotor/gui/ingame/map.cpp
    src/engine/game/kotor/gui/ingame/messages.cpp
    src/engine/game/kotor/gui/ingame/options.cpp
    src/engine/game/kotor/gui/loadscreen.cpp
    src/engine/game/kotor/gui/mainmenu.cpp
    src/engine/game/kotor/gui/partyselect.cpp
    src/engine/game/kotor/gui/saveload.cpp
    src/engine/game/kotor/gui/selectoverlay.cpp
    src/engine/game/kotor/gui/sounds.cpp
    src/engine/game/kotor/kotor.cpp
    src/engine/game/kotor/routine/registrar.cpp)

add_library(libkotor STATIC ${KOTOR_HEADERS} ${KOTOR_SOURCES})
set_target_properties(libkotor PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libkotor PRIVATE src/engine/pch.h)
target_link_libraries(libkotor PUBLIC libgamecore)

## END libkotor static library

## libtsl static library

set(TSL_HEADERS
    src/engine/game/tsl/routine/registrar.h
    src/engine/game/tsl/tsl.h)

set(TSL_SOURCES
    src/engine/game/tsl/routine/registrar.cpp
    src/engine/game/tsl/tsl.cpp)

add_library(libtsl STATIC ${TSL_HEADERS} ${TSL_SOURCES})
set_target_properties(libtsl PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libtsl PRIVATE src/engine/pch.h)
target_link_libraries(libtsl PUBLIC libkotor)

## END libtsl static library

## libgame static library

set(GAME_HEADERS
    src/engine/game/probe.h
    src/engine/game/types.h)

set(GAME_SOURCES
    src/engine/game/probe.cpp)

add_library(libgame STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(libgame PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_precompile_headers(libgame PRIVATE src/engine/pch.h)
target_link_libraries(libgame PUBLIC liblimbo libtsl)

## END libgame static library

## reone executable

set(REONE_HEADERS
    src/engine/di/container.h
    src/engine/di/module/audio.h
    src/engine/di/module/common.h
    src/engine/di/module/game.h
    src/engine/di/module/graphics.h
    src/engine/di/module/resource.h
    src/engine/di/module/scene.h
    src/engine/di/module/script.h
    src/engine/engine.h
    src/engine/optionsparser.h
    src/engine/pch.h)

set(REONE_SOURCES
    src/engine/di/container.cpp
    src/engine/di/module/audio.cpp
    src/engine/di/module/common.cpp
    src/engine/di/module/game.cpp
    src/engine/di/module/graphics.cpp
    src/engine/di/module/resource.cpp
    src/engine/di/module/scene.cpp
    src/engine/di/module/script.cpp
    src/engine/engine.cpp
    src/engine/main.cpp
    src/engine/optionsparser.cpp)

if(WIN32)
    list(APPEND REONE_SOURCES res/reone.rc)
endif()

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_precompile_headers(reone PRIVATE src/engine/pch.h)
target_link_libraries(reone PRIVATE libgame ${Boost_PROGRAM_OPTIONS_LIBRARY})

list(APPEND InstallTargets reone)

## END reone executable

## reone-tools executable

if(BUILD_TOOLS)
    set(TOOLS_HEADERS
        src/tools/program.h
        src/tools/tool/2da.h
        src/tools/tool/audio.h
        src/tools/tool/erf.h
        src/tools/tool/gff.h
        src/tools/tool/itool.h
        src/tools/tool/keybif.h
        src/tools/tool/lip.h
        src/tools/tool/ncs.h
        src/tools/tool/rim.h
        src/tools/tool/ssf.h
        src/tools/tool/tlk.h
        src/tools/tool/tpc.h
        src/tools/types.h)

    set(TOOLS_SOURCES
        src/tools/main.cpp
        src/tools/program.cpp
        src/tools/tool/2da.cpp
        src/tools/tool/audio.cpp
        src/tools/tool/erf.cpp
        src/tools/tool/gff.cpp
        src/tools/tool/keybif.cpp
        src/tools/tool/lip.cpp
        src/tools/tool/ncs.cpp
        src/tools/tool/rim.cpp
        src/tools/tool/ssf.cpp
        src/tools/tool/tlk.cpp
        src/tools/tool/tpc.cpp)

    add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})
    set_target_properties(reone-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    target_precompile_headers(reone-tools PRIVATE src/engine/pch.h)
    target_link_libraries(reone-tools PRIVATE libgame ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_REGEX_LIBRARY})

    list(APPEND InstallTargets reone-tools)
endif()

## END reone-tools executable

## reone-launcher executable

if(BUILD_LAUNCHER)
    if(wxWidgets_FOUND)
        include(${wxWidgets_USE_FILE})

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

        set_target_properties(reone-launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
        target_precompile_headers(reone-launcher PRIVATE src/engine/pch.h)
        target_link_libraries(reone-launcher ${wxWidgets_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

        list(APPEND InstallTargets reone-launcher)
    else()
        message(WARNING "wxWidgets not found - launcher cannot be built")
    endif()

endif()

## END reone-launcher executable

## Unit tests

if(BUILD_TESTS)
    set(TEST_SOURCES
        tests/common/async/executor.cpp
        tests/common/cache.cpp
        tests/common/mediastream.cpp
        tests/common/streamreader.cpp
        tests/common/timer.cpp
        tests/common/unicodeutil.cpp
        tests/game/pathfinder.cpp
        tests/main.cpp
        tests/resource/gffstruct.cpp
        tests/script/execution.cpp)

    add_executable(reone-tests ${TEST_SOURCES})
    target_precompile_headers(reone-tests PRIVATE src/engine/pch.h)
    target_link_libraries(reone-tests PRIVATE libgame ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY})

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
