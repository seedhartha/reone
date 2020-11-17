# Copyright (c) 2020 Vsevolod Kremianskii

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

option(BUILD_TOOLS "build tools executable" ON)
option(BUILD_TESTS "build unit tests" OFF)
option(ENABLE_VIDEO "enable video playback" ON)
option(USE_EXTERNAL_GLM "use GLM library from external subdirectory" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)

if(WIN32)
    find_package(OpenAL CONFIG REQUIRED)
    find_package(SDL2 CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    find_package(Threads REQUIRED)
endif()

include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR})

if(ENABLE_VIDEO)
    if(WIN32)
        find_package(FFMPEG REQUIRED)
    else()
        pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavutil libswresample libswscale)
    endif()
    include_directories(${FFMPEG_INCLUDE_DIRS})
endif()

if(USE_EXTERNAL_GLM)
    include_directories(${CMAKE_SOURCE_DIR}/external/glm)
else()
    find_package(glm REQUIRED)
endif()

add_definitions(-DBOOST_BIND_GLOBAL_PLACEHOLDERS)

## libcommon static library

set(COMMON_HEADERS
    src/common/endianutil.h
    src/common/jobs.h
    src/common/log.h
    src/common/pathutil.h
    src/common/random.h
    src/common/streamreader.h
    src/common/streamutil.h
    src/common/streamwriter.h
    src/common/timer.h
    src/common/types.h)

set(COMMON_SOURCES
    src/common/endianutil.cpp
    src/common/jobs.cpp
    src/common/log.cpp
    src/common/pathutil.cpp
    src/common/random.cpp
    src/common/streamreader.cpp
    src/common/streamutil.cpp
    src/common/streamwriter.cpp)

add_library(libcommon STATIC ${COMMON_HEADERS} ${COMMON_SOURCES})
set_target_properties(libcommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libcommon static library

## libresource static library

set(RESOURCE_HEADERS
    src/resource/2dafile.h
    src/resource/biffile.h
    src/resource/binfile.h
    src/resource/erffile.h
    src/resource/folder.h
    src/resource/gfffile.h
    src/resource/keyfile.h
    src/resource/lytfile.h
    src/resource/pefile.h
    src/resource/resources.h
    src/resource/rimfile.h
    src/resource/tlkfile.h
    src/resource/types.h
    src/resource/util.h
    src/resource/visfile.h)

set(RESOURCE_SOURCES
    src/resource/2dafile.cpp
    src/resource/biffile.cpp
    src/resource/binfile.cpp
    src/resource/erffile.cpp
    src/resource/folder.cpp
    src/resource/gfffile.cpp
    src/resource/keyfile.cpp
    src/resource/lytfile.cpp
    src/resource/pefile.cpp
    src/resource/rimfile.cpp
    src/resource/resources.cpp
    src/resource/tlkfile.cpp
    src/resource/util.cpp
    src/resource/visfile.cpp)

add_library(libresource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES})
set_target_properties(libresource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libresource static library

## librender static library

set(RENDER_HEADERS
    src/render/aabb.h
    src/render/cursor.h
    src/render/font.h
    src/render/fonts.h
    src/render/format/bwmfile.h
    src/render/format/curfile.h
    src/render/format/mdlfile.h
    src/render/format/tgafile.h
    src/render/format/tpcfile.h
    src/render/format/txifile.h
    src/render/fps.h
    src/render/framebuffer.h
    src/render/mesh/aabb.h
    src/render/mesh/cube.h
    src/render/mesh/quad.h
    src/render/mesh/mesh.h
    src/render/mesh/modelmesh.h
    src/render/model/animation.h
    src/render/model/model.h
    src/render/model/modelnode.h
    src/render/models.h
    src/render/pipeline/control.h
    src/render/pipeline/world.h
    src/render/shaders.h
    src/render/texture.h
    src/render/textures.h
    src/render/types.h
    src/render/util.h
    src/render/walkmesh.h
    src/render/walkmeshes.h
    src/render/window.h)

set(RENDER_SOURCES
    src/render/aabb.cpp
    src/render/cursor.cpp
    src/render/font.cpp
    src/render/fonts.cpp
    src/render/format/bwmfile.cpp
    src/render/format/curfile.cpp
    src/render/format/mdlfile.cpp
    src/render/format/tgafile.cpp
    src/render/format/tpcfile.cpp
    src/render/format/txifile.cpp
    src/render/fps.cpp
    src/render/framebuffer.cpp
    src/render/mesh/aabb.cpp
    src/render/mesh/cube.cpp
    src/render/mesh/quad.cpp
    src/render/mesh/mesh.cpp
    src/render/mesh/modelmesh.cpp
    src/render/model/animation.cpp
    src/render/model/model.cpp
    src/render/model/modelnode.cpp
    src/render/models.cpp
    src/render/pipeline/control.cpp
    src/render/pipeline/world.cpp
    src/render/shaders.cpp
    src/render/texture.cpp
    src/render/textures.cpp
    src/render/util.cpp
    src/render/walkmesh.cpp
    src/render/walkmeshes.cpp
    src/render/window.cpp)

add_library(librender STATIC ${RENDER_HEADERS} ${RENDER_SOURCES})
set_target_properties(librender PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END librender static library

## libaudio static library

set(AUDIO_HEADERS
    src/audio/files.h
    src/audio/format/mp3file.h
    src/audio/format/wavfile.h
    src/audio/player.h
    src/audio/soundhandle.h
    src/audio/soundinstance.h
    src/audio/stream.h
    src/audio/types.h
    src/audio/util.h)

set(AUDIO_SOURCES
    src/audio/files.cpp
    src/audio/format/mp3file.cpp
    src/audio/format/wavfile.cpp
    src/audio/player.cpp
    src/audio/soundhandle.cpp
    src/audio/soundinstance.cpp
    src/audio/stream.cpp
    src/audio/util.cpp)

add_library(libaudio STATIC ${AUDIO_HEADERS} ${AUDIO_SOURCES})
set_target_properties(libaudio PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libaudio static library

## libvideo static library

set(VIDEO_HEADERS
    src/video/bikfile.h
    src/video/video.h)

set(VIDEO_SOURCES
    src/video/bikfile.cpp
    src/video/video.cpp)

add_library(libvideo STATIC ${VIDEO_HEADERS} ${VIDEO_SOURCES})
set_target_properties(libvideo PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

if(ENABLE_VIDEO)
    target_compile_definitions(libvideo PRIVATE -DREONE_ENABLE_VIDEO)
endif()

## END libvideo static library

## libscene static library

set(SCENE_HEADERS
    src/scene/aabbnode.h
    src/scene/cameranode.h
    src/scene/cubenode.h
    src/scene/lightnode.h
    src/scene/modelnodescenenode.h
    src/scene/modelscenenode.h
    src/scene/octree.h
    src/scene/scenegraph.h
    src/scene/scenenode.h
    src/scene/scenenodeanimator.h)

set(SCENE_SOURCES
    src/scene/aabbnode.cpp
    src/scene/cameranode.cpp
    src/scene/cubenode.cpp
    src/scene/lightnode.cpp
    src/scene/modelnodescenenode.cpp
    src/scene/modelscenenode.cpp
    src/scene/octree.cpp
    src/scene/scenegraph.cpp
    src/scene/scenenode.cpp
    src/scene/scenenodeanimator.cpp)

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
    src/script/execution.h
    src/script/ncsfile.h
    src/script/program.h
    src/script/routine.h
    src/script/scripts.h
    src/script/types.h
    src/script/util.h
    src/script/variable.h)

set(SCRIPT_SOURCES
    src/script/execution.cpp
    src/script/ncsfile.cpp
    src/script/program.cpp
    src/script/routine.cpp
    src/script/scripts.cpp
    src/script/util.cpp
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
    src/game/action/movetoobject.h
    src/game/action/movetopoint.h
    src/game/action/objectaction.h
    src/game/action/startconversation.h
    src/game/actionexecutor.h
    src/game/actionqueue.h
    src/game/blueprint/blueprints.h
    src/game/blueprint/creature.h
    src/game/blueprint/door.h
    src/game/blueprint/item.h
    src/game/blueprint/placeable.h
    src/game/blueprint/sound.h
    src/game/blueprint/trigger.h
    src/game/blueprint/waypoint.h
    src/game/camera/animatedcamera.h
    src/game/camera/camera.h
    src/game/camera/dialogcamera.h
    src/game/camera/firstperson.h
    src/game/camera/staticcamera.h
    src/game/camera/thirdperson.h
    src/game/camera/types.h
    src/game/characters.h
    src/game/collisiondetect.h
    src/game/console.h
    src/game/combat.h
    src/game/faction.h
    src/game/effect.h
    src/game/cursors.h
    src/game/dialog.h
    src/game/enginetype/location.h
    src/game/enginetype/typefactory.h
    src/game/game.h
    src/game/gui/chargen/chargen.h
    src/game/gui/chargen/classselect.h
    src/game/gui/chargen/nameentry.h
    src/game/gui/chargen/portraitselect.h
    src/game/gui/chargen/quick.h
    src/game/gui/chargen/quickorcustom.h
    src/game/gui/colors.h
    src/game/gui/container.h
    src/game/gui/debugoverlay.h
    src/game/gui/dialog.h
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
    src/game/gui/saveload.h
    src/game/gui/selectoverlay.h
    src/game/object/area.h
    src/game/object/camera.h
    src/game/object/creature.h
    src/game/object/door.h
    src/game/object/item.h
    src/game/object/module.h
    src/game/object/object.h
    src/game/object/objectfactory.h
    src/game/object/placeable.h
    src/game/object/sound.h
    src/game/object/spatial.h
    src/game/object/trigger.h
    src/game/object/types.h
    src/game/object/waypoint.h
    src/game/objectselect.h
    src/game/party.h
    src/game/path.h
    src/game/pathfinder.h
    src/game/player.h
    src/game/portraits.h
    src/game/room.h
    src/game/rp/attributes.h
    src/game/rp/classes.h
    src/game/rp/types.h
    src/game/savedgame.h
    src/game/script/routines.h
    src/game/script/util.h
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
    src/game/blueprint/blueprints.cpp
    src/game/blueprint/creature.cpp
    src/game/blueprint/door.cpp
    src/game/blueprint/item.cpp
    src/game/blueprint/placeable.cpp
    src/game/blueprint/sound.cpp
    src/game/blueprint/trigger.cpp
    src/game/blueprint/waypoint.cpp
    src/game/camera/animatedcamera.cpp
    src/game/camera/camera.cpp
    src/game/camera/dialogcamera.cpp
    src/game/camera/firstperson.cpp
    src/game/camera/staticcamera.cpp
    src/game/camera/thirdperson.cpp
    src/game/characters.cpp
    src/game/collisiondetect.cpp
    src/game/console.cpp
    src/game/combat.cpp
    src/game/faction.cpp
    src/game/cursors.cpp
    src/game/dialog.cpp
    src/game/enginetype/typefactory.cpp
    src/game/game.cpp
    src/game/gui/chargen/chargen.cpp
    src/game/gui/chargen/classselect.cpp
    src/game/gui/chargen/nameentry.cpp
    src/game/gui/chargen/portraitselect.cpp
    src/game/gui/chargen/quick.cpp
    src/game/gui/chargen/quickorcustom.cpp
    src/game/gui/colors.cpp
    src/game/gui/container.cpp
    src/game/gui/debugoverlay.cpp
    src/game/gui/dialog.cpp
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
    src/game/gui/saveload.cpp
    src/game/gui/selectoverlay.cpp
    src/game/object/area.cpp
    src/game/object/camera.cpp
    src/game/object/creature.cpp
    src/game/object/door.cpp
    src/game/object/item.cpp
    src/game/object/module.cpp
    src/game/object/object.cpp
    src/game/object/objectfactory.cpp
    src/game/object/placeable.cpp
    src/game/object/sound.cpp
    src/game/object/spatial.cpp
    src/game/object/trigger.cpp
    src/game/object/waypoint.cpp
    src/game/objectselect.cpp
    src/game/party.cpp
    src/game/path.cpp
    src/game/pathfinder.cpp
    src/game/player.cpp
    src/game/portraits.cpp
    src/game/room.cpp
    src/game/rp/attributes.cpp
    src/game/rp/classes.cpp
    src/game/savedgame.cpp
    src/game/script/routines.cpp
    src/game/script/routines_actions.cpp
    src/game/script/routines_common.cpp
    src/game/script/routines_enginetypes.cpp
    src/game/script/routines_events.cpp
    src/game/script/routines_kotor.cpp
    src/game/script/routines_objects.cpp
    src/game/script/routines_math.cpp
    src/game/script/routines_rp.cpp
    src/game/script/routines_party.cpp
    src/game/script/routines_tsl.cpp
    src/game/script/routines_vars.cpp
    src/game/script/util.cpp)

add_library(libgame STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(libgame PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libgame static library

## libnet static library

set(NET_HEADERS
    src/net/client.h
    src/net/connection.h
    src/net/command.h
    src/net/server.h
    src/net/types.h)

set(NET_SOURCES
    src/net/client.cpp
    src/net/connection.cpp
    src/net/command.cpp
    src/net/server.cpp)

add_library(libnet STATIC ${NET_HEADERS} ${NET_SOURCES})
set_target_properties(libnet PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libnet static library

## libmp static library

set(MP_HEADERS
    src/mp/command.h
    src/mp/game.h
    src/mp/types.h
    src/mp/util.h)

set(MP_SOURCES
    src/mp/command.cpp
    src/mp/game.cpp
    src/mp/util.cpp)

add_library(libmp STATIC ${MP_HEADERS} ${MP_SOURCES})
set_target_properties(libmp PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libmp static library

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
    libmp libnet libgame libscript libgui libscene libvideo librender libaudio libresource libcommon
    ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY}
    GLEW::GLEW
    ${OPENGL_LIBRARIES}
    ${MAD_LIBRARY})

if(ENABLE_VIDEO)
    target_link_libraries(reone PRIVATE ${FFMPEG_LIBRARIES})
endif()

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
        tools/tools.h)

    set(TOOLS_SOURCES
        tools/main.cpp
        tools/2datool.cpp
        tools/biftool.cpp
        tools/erftool.cpp
        tools/gfftool.cpp
        tools/keytool.cpp
        tools/program.cpp
        tools/rimtool.cpp
        tools/tlktool.cpp
        tools/tools.cpp)

    add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})
    set_target_properties(reone-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    target_link_libraries(reone-tools PRIVATE libresource libcommon ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})
endif()

## END reone-tools executable

## Unit tests

if(BUILD_TESTS)
    enable_testing()
    file(GLOB TEST_FILES "tests/*.cpp")
    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
        add_executable(test_${TEST_NAME} ${TEST_FILE})
        target_link_libraries(test_${TEST_NAME} PRIVATE libgame libscript libcommon)

        if(WIN32)
            target_link_libraries(test_${TEST_NAME} PRIVATE SDL2::SDL2)
        else()
            target_link_libraries(test_${TEST_NAME} PRIVATE ${SDL2_LIBRARIES})
        endif()

        add_test(${TEST_NAME} test_${TEST_NAME})
    endforeach()
endif()

## END Unit tests
