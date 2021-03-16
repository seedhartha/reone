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

cmake_minimum_required(VERSION 3.10)
project(reone)

add_subdirectory(external/libtga)
add_subdirectory(external/s3tc)

option(BUILD_TOOLS "build tools executable" ON)
option(BUILD_LAUNCHER "build launcher executable" OFF)
option(BUILD_TESTS "build unit tests" OFF)

option(USE_EXTERNAL_GLM "use GLM library from external subdirectory" ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)
find_package(wxWidgets COMPONENTS core base)

if(WIN32)
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

include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/external/libtga ${CMAKE_SOURCE_DIR}/external/s3tc)

if(USE_EXTERNAL_GLM)
    include_directories(${CMAKE_SOURCE_DIR}/external/glm)
else()
    find_package(glm REQUIRED)
endif()

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

## libcommon static library

set(COMMON_HEADERS
    src/common/cache.h
    src/common/collectionutil.h
    src/common/endianutil.h
    src/common/log.h
    src/common/mediastream.h
    src/common/pathutil.h
    src/common/random.h
    src/common/stopwatch.h
    src/common/streamreader.h
    src/common/streamutil.h
    src/common/streamwriter.h
    src/common/timer.h
    src/common/types.h)

set(COMMON_SOURCES
    src/common/endianutil.cpp
    src/common/log.cpp
    src/common/pathutil.cpp
    src/common/random.cpp
    src/common/stopwatch.cpp
    src/common/streamreader.cpp
    src/common/streamutil.cpp
    src/common/streamwriter.cpp
    src/common/timer.cpp)

add_library(libcommon STATIC ${COMMON_HEADERS} ${COMMON_SOURCES})
set_target_properties(libcommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libcommon static library

## libresource static library

set(RESOURCE_HEADERS
    src/resource/2da.h
    src/resource/gameidutil.h
    src/resource/gffstruct.h
    src/resource/folder.h
    src/resource/format/2dareader.h
    src/resource/format/2dawriter.h
    src/resource/format/bifreader.h
    src/resource/format/binreader.h
    src/resource/format/erfreader.h
    src/resource/format/gffreader.h
    src/resource/format/gffwriter.h
    src/resource/format/keyreader.h
    src/resource/format/ltrreader.h
    src/resource/format/lytreader.h
    src/resource/format/pereader.h
    src/resource/format/rimreader.h
    src/resource/format/rimwriter.h
    src/resource/format/ssfreader.h
    src/resource/format/tlkreader.h
    src/resource/format/visreader.h
    src/resource/keybifprovider.h
    src/resource/resourceprovider.h
    src/resource/resources.h
    src/resource/strings.h
    src/resource/types.h
    src/resource/typeutil.h)

set(RESOURCE_SOURCES
    src/resource/2da.cpp
    src/resource/gameidutil.cpp
    src/resource/gffstruct.cpp
    src/resource/folder.cpp
    src/resource/format/2dareader.cpp
    src/resource/format/2dawriter.cpp
    src/resource/format/bifreader.cpp
    src/resource/format/binreader.cpp
    src/resource/format/erfreader.cpp
    src/resource/format/gffreader.cpp
    src/resource/format/gffwriter.cpp
    src/resource/format/keyreader.cpp
    src/resource/format/ltrreader.cpp
    src/resource/format/lytreader.cpp
    src/resource/format/pereader.cpp
    src/resource/format/rimreader.cpp
    src/resource/format/rimwriter.cpp
    src/resource/format/ssfreader.cpp
    src/resource/format/tlkreader.cpp
    src/resource/format/visreader.cpp
    src/resource/keybifprovider.cpp
    src/resource/resources.cpp
    src/resource/strings.cpp
    src/resource/typeutil.cpp)

add_library(libresource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES})
set_target_properties(libresource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libresource static library

## librender static library

set(RENDER_HEADERS
    src/render/aabb.h
    src/render/cursor.h
    src/render/featureutil.h
    src/render/font.h
    src/render/fonts.h
    src/render/framebuffer.h
    src/render/image/curreader.h
    src/render/image/tgareader.h
    src/render/image/tpcreader.h
    src/render/image/txireader.h
    src/render/lip/lipanimation.h
    src/render/lip/lipreader.h
    src/render/lip/lips.h
    src/render/material.h
    src/render/materials.h
    src/render/mesh.h
    src/render/meshes.h
    src/render/model/animation.h
    src/render/model/emitter.h
    src/render/model/mdlreader.h
    src/render/model/model.h
    src/render/model/modelloader.h
    src/render/model/modelmesh.h
    src/render/model/modelnode.h
    src/render/model/models.h
    src/render/pbribl.h
    src/render/pixelutil.h
    src/render/renderbuffer.h
    src/render/shaders.h
    src/render/stateutil.h
    src/render/texture.h
    src/render/textures.h
    src/render/textureutil.h
    src/render/textutil.h
    src/render/types.h
    src/render/walkmesh/bwmreader.h
    src/render/walkmesh/walkmesh.h
    src/render/walkmesh/walkmeshes.h
    src/render/walkmesh/walkmeshutil.h
    src/render/window.h)

set(RENDER_SOURCES
    src/render/aabb.cpp
    src/render/cursor.cpp
    src/render/featureutil.cpp
    src/render/font.cpp
    src/render/fonts.cpp
    src/render/framebuffer.cpp
    src/render/image/curreader.cpp
    src/render/image/tgareader.cpp
    src/render/image/tpcreader.cpp
    src/render/image/txireader.cpp
    src/render/lip/lipanimation.cpp
    src/render/lip/lipreader.cpp
    src/render/lip/lips.cpp
    src/render/materials.cpp
    src/render/mesh.cpp
    src/render/meshes.cpp
    src/render/model/animation.cpp
    src/render/model/mdlreader.cpp
    src/render/model/model.cpp
    src/render/model/modelmesh.cpp
    src/render/model/modelnode.cpp
    src/render/model/models.cpp
    src/render/pbribl.cpp
    src/render/pixelutil.cpp
    src/render/renderbuffer.cpp
    src/render/shaders.cpp
    src/render/stateutil.cpp
    src/render/texture.cpp
    src/render/textures.cpp
    src/render/textureutil.cpp
    src/render/textutil.cpp
    src/render/walkmesh/bwmreader.cpp
    src/render/walkmesh/walkmesh.cpp
    src/render/walkmesh/walkmeshes.cpp
    src/render/walkmesh/walkmeshutil.cpp
    src/render/window.cpp)

add_library(librender STATIC ${RENDER_HEADERS} ${RENDER_SOURCES})
set_target_properties(librender PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END librender static library

## libaudio static library

set(AUDIO_HEADERS
    src/audio/files.h
    src/audio/format/mp3reader.h
    src/audio/format/wavreader.h
    src/audio/player.h
    src/audio/soundhandle.h
    src/audio/soundinstance.h
    src/audio/stream.h
    src/audio/types.h)

set(AUDIO_SOURCES
    src/audio/files.cpp
    src/audio/format/mp3reader.cpp
    src/audio/format/wavreader.cpp
    src/audio/player.cpp
    src/audio/soundhandle.cpp
    src/audio/soundinstance.cpp
    src/audio/stream.cpp)

add_library(libaudio STATIC ${AUDIO_HEADERS} ${AUDIO_SOURCES})
set_target_properties(libaudio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libaudio static library

## libvideo static library

set(VIDEO_HEADERS
    src/video/bikreader.h
    src/video/video.h)

set(VIDEO_SOURCES
    src/video/bikreader.cpp
    src/video/video.cpp)

add_library(libvideo STATIC ${VIDEO_HEADERS} ${VIDEO_SOURCES})
set_target_properties(libvideo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libvideo static library

## libscene static library

set(SCENE_HEADERS
    src/scene/animation/channel.h
    src/scene/animation/properties.h
    src/scene/animation/scenenodeanimator.h
    src/scene/animation/scenenodestate.h
    src/scene/node/cameranode.h
    src/scene/node/emitternode.h
    src/scene/node/lightnode.h
    src/scene/node/meshnode.h
    src/scene/node/modelnodescenenode.h
    src/scene/node/modelscenenode.h
    src/scene/node/scenenode.h
    src/scene/particle.h
    src/scene/pipeline/control.h
    src/scene/pipeline/world.h
    src/scene/scenegraph.h
    src/scene/types.h)

set(SCENE_SOURCES
    src/scene/animation/channel.cpp
    src/scene/animation/scenenodeanimator.cpp
    src/scene/node/cameranode.cpp
    src/scene/node/emitternode.cpp
    src/scene/node/lightnode.cpp
    src/scene/node/meshnode.cpp
    src/scene/node/modelnodescenenode.cpp
    src/scene/node/modelscenenode.cpp
    src/scene/node/scenenode.cpp
    src/scene/particle.cpp
    src/scene/pipeline/control.cpp
    src/scene/pipeline/world.cpp
    src/scene/scenegraph.cpp)

add_library(libscene STATIC ${SCENE_HEADERS} ${SCENE_SOURCES})
set_target_properties(libscene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libscene static library

## libgui static library

set(GUI_HEADERS
    src/gui/control/button.h
    src/gui/control/control.h
    src/gui/control/imagebutton.h
    src/gui/control/label.h
    src/gui/control/listbox.h
    src/gui/control/panel.h
    src/gui/control/progressbar.h
    src/gui/control/scrollbar.h
    src/gui/control/togglebutton.h
    src/gui/gui.h
    src/gui/scenebuilder.h
    src/gui/textinput.h
    src/gui/types.h)

set(GUI_SOURCES
    src/gui/control/button.cpp
    src/gui/control/control.cpp
    src/gui/control/imagebutton.cpp
    src/gui/control/label.cpp
    src/gui/control/listbox.cpp
    src/gui/control/panel.cpp
    src/gui/control/progressbar.cpp
    src/gui/control/scrollbar.cpp
    src/gui/control/togglebutton.cpp
    src/gui/gui.cpp
    src/gui/scenebuilder.cpp
    src/gui/textinput.cpp)

add_library(libgui STATIC ${GUI_HEADERS} ${GUI_SOURCES})
set_target_properties(libgui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libgui static library

## libscript static library

set(SCRIPT_HEADERS
    src/script/enginetype.h
    src/script/execution.h
    src/script/instrutil.h
    src/script/ncsreader.h
    src/script/object.h
    src/script/program.h
    src/script/routine.h
    src/script/scripts.h
    src/script/types.h
    src/script/variable.h)

set(SCRIPT_SOURCES
    src/script/execution.cpp
    src/script/instrutil.cpp
    src/script/ncsreader.cpp
    src/script/object.cpp
    src/script/program.cpp
    src/script/routine.cpp
    src/script/scripts.cpp
    src/script/variable.cpp)

add_library(libscript STATIC ${SCRIPT_HEADERS} ${SCRIPT_SOURCES})
set_target_properties(libscript PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libscript static library

## libgame static library

set(GAME_HEADERS
    src/game/action/action.h
    src/game/action/attack.h
    src/game/action/commandaction.h
    src/game/action/follow.h
    src/game/action/locationaction.h
    src/game/action/movetolocation.h
    src/game/action/movetoobject.h
    src/game/action/movetopoint.h
    src/game/action/objectaction.h
    src/game/action/playanimation.h
    src/game/action/startconversation.h
    src/game/action/waitaction.h
    src/game/actionexecutor.h
    src/game/actionqueue.h
    src/game/animationutil.h
    src/game/blueprint/blueprint.h
    src/game/blueprint/blueprints.h
    src/game/blueprint/creature.h
    src/game/blueprint/door.h
    src/game/blueprint/encounter.h
    src/game/blueprint/item.h
    src/game/blueprint/placeable.h
    src/game/blueprint/sound.h
    src/game/blueprint/trigger.h
    src/game/blueprint/waypoint.h
    src/game/camera/animatedcamera.h
    src/game/camera/camera.h
    src/game/camera/camerastyle.h
    src/game/camera/dialogcamera.h
    src/game/camera/firstperson.h
    src/game/camera/staticcamera.h
    src/game/camera/thirdperson.h
    src/game/characterutil.h
    src/game/collisiondetect.h
    src/game/console.h
    src/game/combat/attackresolver.h
    src/game/combat/attackresult.h
    src/game/combat/attackutil.h
    src/game/combat/combat.h
    src/game/combat/damageresolver.h
    src/game/creaturefinder.h
    src/game/cursors.h
    src/game/dialog.h
    src/game/enginetype/effect.h
    src/game/enginetype/event.h
    src/game/enginetype/location.h
    src/game/game.h
    src/game/gui/barkbubble.h
    src/game/gui/chargen/abilities.h
    src/game/gui/chargen/chargen.h
    src/game/gui/chargen/classselect.h
    src/game/gui/chargen/custom.h
    src/game/gui/chargen/feats.h
    src/game/gui/chargen/levelup.h
    src/game/gui/chargen/nameentry.h
    src/game/gui/chargen/portraitselect.h
    src/game/gui/chargen/quick.h
    src/game/gui/chargen/quickorcustom.h
    src/game/gui/chargen/skills.h
    src/game/gui/colorutil.h
    src/game/gui/container.h
    src/game/gui/conversation.h
    src/game/gui/computer.h
    src/game/gui/debugoverlay.h
    src/game/gui/dialog.h
    src/game/gui/gui.h
    src/game/gui/hud.h
    src/game/gui/ingame/abilities.h
    src/game/gui/ingame/character.h
    src/game/gui/ingame/equip.h
    src/game/gui/ingame/ingame.h
    src/game/gui/ingame/inventory.h
    src/game/gui/ingame/journal.h
    src/game/gui/ingame/map.h
    src/game/gui/ingame/messages.h
    src/game/gui/ingame/options.h
    src/game/gui/loadscreen.h
    src/game/gui/mainmenu.h
    src/game/gui/partyselect.h
    src/game/gui/profileoverlay.h
    src/game/gui/saveload.h
    src/game/gui/selectoverlay.h
    src/game/gui/sounds.h
    src/game/map.h
    src/game/object/area.h
    src/game/object/creature.h
    src/game/object/creatureanimresolver.h
    src/game/object/creaturemodelbuilder.h
    src/game/object/door.h
    src/game/object/encounter.h
    src/game/object/item.h
    src/game/object/module.h
    src/game/object/object.h
    src/game/object/objectfactory.h
    src/game/object/placeable.h
    src/game/object/placeablecamera.h
    src/game/object/sound.h
    src/game/object/spatial.h
    src/game/object/trigger.h
    src/game/object/waypoint.h
    src/game/objectconverter.h
    src/game/objectselect.h
    src/game/options.h
    src/game/party.h
    src/game/path.h
    src/game/pathfinder.h
    src/game/player.h
    src/game/portrait.h
    src/game/portraitutil.h
    src/game/reputes.h
    src/game/room.h
    src/game/rp/abilities.h
    src/game/rp/attributes.h
    src/game/rp/class.h
    src/game/rp/classes.h
    src/game/rp/savingthrows.h
    src/game/rp/skills.h
    src/game/savedgame.h
    src/game/script/routines.h
    src/game/script/runner.h
    src/game/soundsets.h
    src/game/types.h)

set(GAME_SOURCES
    src/game/action/action.cpp
    src/game/action/commandaction.cpp
    src/game/action/follow.cpp
    src/game/action/movetoobject.cpp
    src/game/action/movetopoint.cpp
    src/game/action/objectaction.cpp
    src/game/action/startconversation.cpp
    src/game/actionexecutor.cpp
    src/game/actionqueue.cpp
    src/game/animationutil.cpp
    src/game/blueprint/blueprints.cpp
    src/game/blueprint/creature.cpp
    src/game/blueprint/door.cpp
    src/game/blueprint/encounter.cpp
    src/game/blueprint/item.cpp
    src/game/blueprint/placeable.cpp
    src/game/blueprint/sound.cpp
    src/game/blueprint/trigger.cpp
    src/game/blueprint/waypoint.cpp
    src/game/camera/animatedcamera.cpp
    src/game/camera/camera.cpp
    src/game/camera/camerastyle.cpp
    src/game/camera/dialogcamera.cpp
    src/game/camera/firstperson.cpp
    src/game/camera/staticcamera.cpp
    src/game/camera/thirdperson.cpp
    src/game/characterutil.cpp
    src/game/collisiondetect.cpp
    src/game/console.cpp
    src/game/combat/attackresolver.cpp
    src/game/combat/combat.cpp
    src/game/combat/damageresolver.cpp
    src/game/creaturefinder.cpp
    src/game/cursors.cpp
    src/game/dialog.cpp
    src/game/game.cpp
    src/game/gui/barkbubble.cpp
    src/game/gui/chargen/abilities.cpp
    src/game/gui/chargen/chargen.cpp
    src/game/gui/chargen/classselect.cpp
    src/game/gui/chargen/custom.cpp
    src/game/gui/chargen/feats.cpp
    src/game/gui/chargen/levelup.cpp
    src/game/gui/chargen/nameentry.cpp
    src/game/gui/chargen/portraitselect.cpp
    src/game/gui/chargen/quick.cpp
    src/game/gui/chargen/quickorcustom.cpp
    src/game/gui/chargen/skills.cpp
    src/game/gui/colorutil.cpp
    src/game/gui/container.cpp
    src/game/gui/conversation.cpp
    src/game/gui/computer.cpp
    src/game/gui/debugoverlay.cpp
    src/game/gui/dialog.cpp
    src/game/gui/gui.cpp
    src/game/gui/hud.cpp
    src/game/gui/ingame/abilities.cpp
    src/game/gui/ingame/character.cpp
    src/game/gui/ingame/equip.cpp
    src/game/gui/ingame/ingame.cpp
    src/game/gui/ingame/inventory.cpp
    src/game/gui/ingame/journal.cpp
    src/game/gui/ingame/map.cpp
    src/game/gui/ingame/messages.cpp
    src/game/gui/ingame/options.cpp
    src/game/gui/loadscreen.cpp
    src/game/gui/mainmenu.cpp
    src/game/gui/partyselect.cpp
    src/game/gui/profileoverlay.cpp
    src/game/gui/saveload.cpp
    src/game/gui/selectoverlay.cpp
    src/game/gui/sounds.cpp
    src/game/map.cpp
    src/game/object/area.cpp
    src/game/object/creature.cpp
    src/game/object/creatureanimresolver.cpp
    src/game/object/creaturemodelbuilder.cpp
    src/game/object/door.cpp
    src/game/object/encounter.cpp
    src/game/object/item.cpp
    src/game/object/module.cpp
    src/game/object/object.cpp
    src/game/object/objectfactory.cpp
    src/game/object/placeable.cpp
    src/game/object/placeablecamera.cpp
    src/game/object/sound.cpp
    src/game/object/spatial.cpp
    src/game/object/trigger.cpp
    src/game/object/waypoint.cpp
    src/game/objectselect.cpp
    src/game/party.cpp
    src/game/path.cpp
    src/game/pathfinder.cpp
    src/game/player.cpp
    src/game/portraitutil.cpp
    src/game/reputes.cpp
    src/game/room.cpp
    src/game/rp/abilities.cpp
    src/game/rp/attributes.cpp
    src/game/rp/class.cpp
    src/game/rp/classes.cpp
    src/game/rp/skills.cpp
    src/game/savedgame.cpp
    src/game/script/routines.cpp
    src/game/script/routines_actions.cpp
    src/game/script/routines_common.cpp
    src/game/script/routines_effects.cpp
    src/game/script/routines_enginetypes.cpp
    src/game/script/routines_events.cpp
    src/game/script/routines_items.cpp
    src/game/script/routines_kotor.cpp
    src/game/script/routines_objects.cpp
    src/game/script/routines_math.cpp
    src/game/script/routines_rp.cpp
    src/game/script/routines_party.cpp
    src/game/script/routines_time.cpp
    src/game/script/routines_tsl.cpp
    src/game/script/routines_vars.cpp
    src/game/script/runner.cpp
    src/game/soundsets.cpp)

add_library(libgame STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(libgame PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libgame static library

## reone executable

set(REONE_HEADERS
    src/program.h)

set(REONE_SOURCES
    src/main.cpp
    src/program.cpp)

if(WIN32)
    list(APPEND REONE_SOURCES res/reone.rc)
endif()

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_link_libraries(reone PRIVATE
    libgame libscript libgui libscene libvideo libaudio librender libresource libcommon
    libs3tc
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    GLEW::GLEW
    ${OPENGL_LIBRARIES}
    ${MAD_LIBRARY}
    ${FFMPEG_LIBRARIES})

if(WIN32)
    target_link_libraries(reone PRIVATE SDL2::SDL2 OpenAL::OpenAL)
else()
    target_link_libraries(reone PRIVATE ${SDL2_LIBRARIES} ${OpenAL_LIBRARIES} Threads::Threads -latomic)
endif()

## END reone executable

## reone-tools executable

if(BUILD_TOOLS)
    set(TOOLS_HEADERS
        tools/program.h
        tools/tools.h
        tools/types.h)

    set(TOOLS_SOURCES
        tools/2datool.cpp
        tools/erftool.cpp
        tools/gfftool.cpp
        tools/keybiftool.cpp
        tools/main.cpp
        tools/program.cpp
        tools/rimtool.cpp
        tools/tlktool.cpp
        tools/tpctool.cpp)

    add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})
    set_target_properties(reone-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    target_link_libraries(reone-tools PRIVATE
        librender libresource libcommon
        libtga libs3tc
        GLEW::GLEW
        ${OPENGL_LIBRARIES}
        ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

    if(WIN32)
        target_link_libraries(reone PRIVATE SDL2::SDL2)
    else()
        target_link_libraries(reone PRIVATE ${SDL2_LIBRARIES})
    endif()
endif()

## END reone-tools executable

## reone-launcher executable

if(BUILD_LAUNCHER)
    if(wxWidgets_FOUND)
        set(LAUNCHER_HEADERS
            launcher/app.h
            launcher/frame.h)

        set(LAUNCHER_SOURCES
            launcher/app.cpp
            launcher/frame.cpp)

        if(WIN32)
            list(APPEND LAUNCHER_SOURCES res/reone.rc)
            add_executable(reone-launcher WIN32 ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES})
        else()
            add_executable(reone-launcher ${LAUNCHER_HEADERS} ${LAUNCHER_SOURCES})
        endif()

        include(${wxWidgets_USE_FILE})
        target_link_libraries(reone-launcher ${wxWidgets_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

        set_target_properties(reone-launcher PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    else()
        message(WARNING "wxWidgets not found - launcher cannot be built")
    endif()

endif()

## END reone-launcher executable

## Unit tests

if(BUILD_TESTS)
    enable_testing()
    file(GLOB TEST_FILES "tests/*.cpp")
    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
        add_executable(test_${TEST_NAME} ${TEST_FILE})
        target_link_libraries(test_${TEST_NAME} PRIVATE libgame libscript libresource libcommon ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY})

        if(WIN32)
            target_link_libraries(test_${TEST_NAME} PRIVATE SDL2::SDL2)
        else()
            target_link_libraries(test_${TEST_NAME} PRIVATE ${SDL2_LIBRARIES})
        endif()

        add_test(${TEST_NAME} test_${TEST_NAME})
    endforeach()
endif()

## END Unit tests
