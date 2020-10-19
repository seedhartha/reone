# Copyright © 2020 Vsevolod Kremianskii

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
option(USE_EXTERNAL_GLM "use GLM library from external subdirectory" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(SDL2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)

if(WIN32)
    find_package(OpenAL CONFIG REQUIRED)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    find_package(Threads REQUIRED)
endif()

if(NOT USE_EXTERNAL_GLM)
    find_package(glm REQUIRED)
endif()

## libsystem static library

set(SYSTEM_HEADERS
    src/system/audio/player.h
    src/system/audio/soundinstance.h
    src/system/audio/stream.h
    src/system/audio/types.h
    src/system/audio/util.h
    src/system/core/debug.h
    src/system/core/jobs.h
    src/system/core/log.h
    src/system/core/pathutil.h
    src/system/core/random.h
    src/system/core/streamutil.h
    src/system/core/types.h
    src/system/gui/control/button.h
    src/system/gui/control/control.h
    src/system/gui/control/imagebutton.h
    src/system/gui/control/label.h
    src/system/gui/control/listbox.h
    src/system/gui/control/panel.h
    src/system/gui/control/scrollbar.h
    src/system/gui/gui.h
    src/system/gui/scenebuilder.h
    src/system/gui/textinput.h
    src/system/gui/types.h
    src/system/net/client.h
    src/system/net/connection.h
    src/system/net/command.h
    src/system/net/server.h
    src/system/net/types.h
    src/system/render/aabb.h
    src/system/render/font.h
    src/system/render/framebuffer.h
    src/system/render/fps.h
    src/system/render/mesh/aabb.h
    src/system/render/mesh/cube.h
    src/system/render/mesh/quad.h
    src/system/render/mesh/mesh.h
    src/system/render/mesh/modelmesh.h
    src/system/render/model/model.h
    src/system/render/model/animation.h
    src/system/render/model/modelnode.h
    src/system/render/pipeline/control.h
    src/system/render/pipeline/world.h
    src/system/render/scene/aabbnode.h
    src/system/render/scene/cameranode.h
    src/system/render/scene/cubenode.h
    src/system/render/scene/lightnode.h
    src/system/render/scene/modelnodescenenode.h
    src/system/render/scene/modelscenenode.h
    src/system/render/scene/scenegraph.h
    src/system/render/scene/scenenode.h
    src/system/render/scene/scenenodeanimator.h
    src/system/render/shaders.h
    src/system/render/texture.h
    src/system/render/types.h
    src/system/render/walkmesh.h
    src/system/render/window.h
    src/system/resource/2dafile.h
    src/system/resource/audio/mp3file.h
    src/system/resource/audio/wavfile.h
    src/system/resource/binfile.h
    src/system/resource/blueprint/creature.h
    src/system/resource/blueprint/door.h
    src/system/resource/blueprint/item.h
    src/system/resource/blueprint/placeable.h
    src/system/resource/blueprint/trigger.h
    src/system/resource/blueprint/waypoint.h
    src/system/resource/bwmfile.h
    src/system/resource/collection/biffile.h
    src/system/resource/collection/erffile.h
    src/system/resource/collection/folder.h
    src/system/resource/collection/keyfile.h
    src/system/resource/collection/pefile.h
    src/system/resource/collection/rimfile.h
    src/system/resource/dlgfile.h
    src/system/resource/gfffile.h
    src/system/resource/image/curfile.h
    src/system/resource/image/tgafile.h
    src/system/resource/image/tpcfile.h
    src/system/resource/image/txifile.h
    src/system/resource/lytfile.h
    src/system/resource/mdlfile.h
    src/system/resource/ncsfile.h
    src/system/resource/pthfile.h
    src/system/resource/resources.h
    src/system/resource/tlkfile.h
    src/system/resource/types.h
    src/system/resource/util.h
    src/system/resource/visfile.h
    src/system/script/execution.h
    src/system/script/program.h
    src/system/script/routine.h
    src/system/script/types.h
    src/system/script/util.h
    src/system/script/variable.h)

set(SYSTEM_SOURCES
    src/system/audio/player.cpp
    src/system/audio/soundinstance.cpp
    src/system/audio/stream.cpp
    src/system/audio/util.cpp
    src/system/core/debug.cpp
    src/system/core/jobs.cpp
    src/system/core/log.cpp
    src/system/core/pathutil.cpp
    src/system/core/random.cpp
    src/system/core/streamutil.cpp
    src/system/gui/control/button.cpp
    src/system/gui/control/control.cpp
    src/system/gui/control/imagebutton.cpp
    src/system/gui/control/label.cpp
    src/system/gui/control/listbox.cpp
    src/system/gui/control/panel.cpp
    src/system/gui/control/scrollbar.cpp
    src/system/gui/gui.cpp
    src/system/gui/scenebuilder.cpp
    src/system/gui/textinput.cpp
    src/system/net/client.cpp
    src/system/net/connection.cpp
    src/system/net/command.cpp
    src/system/net/server.cpp
    src/system/render/aabb.cpp
    src/system/render/font.cpp
    src/system/render/framebuffer.cpp
    src/system/render/fps.cpp
    src/system/render/mesh/aabb.cpp
    src/system/render/mesh/cube.cpp
    src/system/render/mesh/quad.cpp
    src/system/render/mesh/mesh.cpp
    src/system/render/mesh/modelmesh.cpp
    src/system/render/model/model.cpp
    src/system/render/model/animation.cpp
    src/system/render/model/modelnode.cpp
    src/system/render/pipeline/control.cpp
    src/system/render/pipeline/world.cpp
    src/system/render/scene/aabbnode.cpp
    src/system/render/scene/cameranode.cpp
    src/system/render/scene/cubenode.cpp
    src/system/render/scene/lightnode.cpp
    src/system/render/scene/modelnodescenenode.cpp
    src/system/render/scene/modelscenenode.cpp
    src/system/render/scene/scenegraph.cpp
    src/system/render/scene/scenenode.cpp
    src/system/render/scene/scenenodeanimator.cpp
    src/system/render/shaders.cpp
    src/system/render/texture.cpp
    src/system/render/walkmesh.cpp
    src/system/render/window.cpp
    src/system/resource/2dafile.cpp
    src/system/resource/audio/mp3file.cpp
    src/system/resource/audio/wavfile.cpp
    src/system/resource/binfile.cpp
    src/system/resource/blueprint/creature.cpp
    src/system/resource/blueprint/door.cpp
    src/system/resource/blueprint/item.cpp
    src/system/resource/blueprint/placeable.cpp
    src/system/resource/blueprint/trigger.cpp
    src/system/resource/blueprint/waypoint.cpp
    src/system/resource/bwmfile.cpp
    src/system/resource/collection/biffile.cpp
    src/system/resource/collection/erffile.cpp
    src/system/resource/collection/folder.cpp
    src/system/resource/collection/keyfile.cpp
    src/system/resource/collection/pefile.cpp
    src/system/resource/collection/rimfile.cpp
    src/system/resource/dlgfile.cpp
    src/system/resource/gfffile.cpp
    src/system/resource/image/curfile.cpp
    src/system/resource/image/tgafile.cpp
    src/system/resource/image/tpcfile.cpp
    src/system/resource/image/txifile.cpp
    src/system/resource/lytfile.cpp
    src/system/resource/mdlfile.cpp
    src/system/resource/ncsfile.cpp
    src/system/resource/pthfile.cpp
    src/system/resource/resources.cpp
    src/system/resource/tlkfile.cpp
    src/system/resource/util.cpp
    src/system/resource/visfile.cpp
    src/system/script/execution.cpp
    src/system/script/program.cpp
    src/system/script/routine.cpp
    src/system/script/util.cpp
    src/system/script/variable.cpp)

add_library(libsystem STATIC ${SYSTEM_HEADERS} ${SYSTEM_SOURCES})
set_target_properties(libsystem PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_compile_definitions(libsystem PUBLIC BOOST_BIND_GLOBAL_PLACEHOLDERS)
target_include_directories(libsystem SYSTEM PUBLIC ${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR})
target_link_libraries(libsystem PUBLIC ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY} GLEW::GLEW ${OPENGL_LIBRARIES} ${MAD_LIBRARY})

if(WIN32)
    target_link_libraries(libsystem PUBLIC SDL2::SDL2 OpenAL::OpenAL)
else()
    target_link_libraries(libsystem PUBLIC ${SDL2_LIBRARIES} ${OpenAL_LIBRARIES} Threads::Threads)
endif()

if(USE_EXTERNAL_GLM)
    target_include_directories(libsystem PUBLIC ${CMAKE_SOURCE_DIR}/external/glm)
endif()

## END libsystem static library

## libgame static library

set(GAME_HEADERS
    src/game/action/action.h
    src/game/action/commandaction.h
    src/game/action/follow.h
    src/game/action/movetoobject.h
    src/game/action/movetopoint.h
    src/game/action/objectaction.h
    src/game/action/startconversation.h
    src/game/actionexecutor.h
    src/game/actionqueue.h
    src/game/camera/animatedcamera.h
    src/game/camera/camera.h
    src/game/camera/dialogcamera.h
    src/game/camera/firstperson.h
    src/game/camera/thirdperson.h
    src/game/characters.h
    src/game/collisiondetect.h
    src/game/console.h
    src/game/game.h
    src/game/gui/chargen/chargen.h
    src/game/gui/chargen/classselect.h
    src/game/gui/chargen/name.h
    src/game/gui/chargen/portraitselect.h
    src/game/gui/chargen/quick.h
    src/game/gui/chargen/quickorcustom.h
    src/game/gui/colors.h
    src/game/gui/container.h
    src/game/gui/debugoverlay.h
    src/game/gui/dialog.h
    src/game/gui/equip.h
    src/game/gui/hud.h
    src/game/gui/loadscreen.h
    src/game/gui/mainmenu.h
    src/game/gui/selectoverlay.h
    src/game/object/area.h
    src/game/object/creature.h
    src/game/object/door.h
    src/game/object/item.h
    src/game/object/module.h
    src/game/object/object.h
    src/game/object/objectfactory.h
    src/game/object/placeable.h
    src/game/object/spatial.h
    src/game/object/trigger.h
    src/game/object/waypoint.h
    src/game/objectselect.h
    src/game/party.h
    src/game/pathfinder.h
    src/game/player.h
    src/game/portraits.h
    src/game/room.h
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
    src/game/camera/animatedcamera.cpp
    src/game/camera/camera.cpp
    src/game/camera/dialogcamera.cpp
    src/game/camera/firstperson.cpp
    src/game/camera/thirdperson.cpp
    src/game/characters.cpp
    src/game/collisiondetect.cpp
    src/game/console.cpp
    src/game/game.cpp
    src/game/gui/chargen/chargen.cpp
    src/game/gui/chargen/classselect.cpp
    src/game/gui/chargen/name.cpp
    src/game/gui/chargen/portraitselect.cpp
    src/game/gui/chargen/quick.cpp
    src/game/gui/chargen/quickorcustom.cpp
    src/game/gui/colors.cpp
    src/game/gui/container.cpp
    src/game/gui/debugoverlay.cpp
    src/game/gui/dialog.cpp
    src/game/gui/equip.cpp
    src/game/gui/hud.cpp
    src/game/gui/loadscreen.cpp
    src/game/gui/mainmenu.cpp
    src/game/gui/selectoverlay.cpp
    src/game/object/area.cpp
    src/game/object/creature.cpp
    src/game/object/door.cpp
    src/game/object/item.cpp
    src/game/object/module.cpp
    src/game/object/object.cpp
    src/game/object/objectfactory.cpp
    src/game/object/placeable.cpp
    src/game/object/spatial.cpp
    src/game/object/trigger.cpp
    src/game/object/waypoint.cpp
    src/game/objectselect.cpp
    src/game/party.cpp
    src/game/pathfinder.cpp
    src/game/player.cpp
    src/game/portraits.cpp
    src/game/room.cpp
    src/game/script/routines.cpp
    src/game/script/routines_common.cpp
    src/game/script/routines_kotor.cpp
    src/game/script/routines_tsl.cpp
    src/game/script/util.cpp)

add_library(libgame STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(libgame PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(libgame PUBLIC libsystem)

## END libgame static library

## libmp static library

set(MP_HEADERS
    src/mp/area.h
    src/mp/callbacks.h
    src/mp/command.h
    src/mp/creature.h
    src/mp/door.h
    src/mp/game.h
    src/mp/objectfactory.h
    src/mp/util.h)

set(MP_SOURCES
    src/mp/area.cpp
    src/mp/command.cpp
    src/mp/creature.cpp
    src/mp/door.cpp
    src/mp/game.cpp
    src/mp/objectfactory.cpp
    src/mp/util.cpp)

add_library(libmp STATIC ${MP_HEADERS} ${MP_SOURCES})
set_target_properties(libmp PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
target_link_libraries(libmp PUBLIC libgame)

## END libmp static library

## reone executable

set(REONE_HEADERS
    src/program.h)

set(REONE_SOURCES
    src/main.cpp
    src/program.cpp)

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(reone PRIVATE libmp)

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
    target_link_libraries(reone-tools PRIVATE libsystem)
endif()

## END reone-tools executable

## Unit tests

if(BUILD_TESTS)
    enable_testing()
    file(GLOB TEST_FILES "tests/*.cpp")
    foreach(TEST_FILE ${TEST_FILES})
        get_filename_component(TEST_NAME "${TEST_FILE}" NAME_WE)
        add_executable(test_${TEST_NAME} ${TEST_FILE})
        target_link_libraries(test_${TEST_NAME} PRIVATE libgame)
        add_test(${TEST_NAME} test_${TEST_NAME})
    endforeach()
endif()

## END Unit tests