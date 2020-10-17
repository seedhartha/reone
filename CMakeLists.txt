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
    src/audio/player.h
    src/audio/soundinstance.h
    src/audio/stream.h
    src/audio/types.h
    src/audio/util.h
    src/core/debug.h
    src/core/jobs.h
    src/core/log.h
    src/core/pathutil.h
    src/core/random.h
    src/core/streamutil.h
    src/core/types.h
    src/gui/control/button.h
    src/gui/control/control.h
    src/gui/control/imagebutton.h
    src/gui/control/label.h
    src/gui/control/listbox.h
    src/gui/control/panel.h
    src/gui/control/scrollbar.h
    src/gui/gui.h
    src/gui/scenebuilder.h
    src/gui/textinput.h
    src/gui/types.h
    src/net/client.h
    src/net/connection.h
    src/net/command.h
    src/net/server.h
    src/net/types.h
    src/render/aabb.h
    src/render/font.h
    src/render/framebuffer.h
    src/render/fps.h
    src/render/mesh/aabb.h
    src/render/mesh/cube.h
    src/render/mesh/quad.h
    src/render/mesh/mesh.h
    src/render/mesh/modelmesh.h
    src/render/model/model.h
    src/render/model/animation.h
    src/render/model/modelnode.h
    src/render/pipeline/control.h
    src/render/pipeline/world.h
    src/render/scene/aabbnode.h
    src/render/scene/cameranode.h
    src/render/scene/cubenode.h
    src/render/scene/lightnode.h
    src/render/scene/modelnodescenenode.h
    src/render/scene/modelscenenode.h
    src/render/scene/scenegraph.h
    src/render/scene/scenenode.h
    src/render/scene/scenenodeanimator.h
    src/render/shaders.h
    src/render/texture.h
    src/render/types.h
    src/render/walkmesh.h
    src/render/window.h
    src/resource/2dafile.h
    src/resource/audio/mp3file.h
    src/resource/audio/wavfile.h
    src/resource/binfile.h
    src/resource/blueprint/creature.h
    src/resource/blueprint/door.h
    src/resource/blueprint/item.h
    src/resource/blueprint/placeable.h
    src/resource/blueprint/trigger.h
    src/resource/blueprint/waypoint.h
    src/resource/bwmfile.h
    src/resource/collection/biffile.h
    src/resource/collection/erffile.h
    src/resource/collection/folder.h
    src/resource/collection/keyfile.h
    src/resource/collection/pefile.h
    src/resource/collection/rimfile.h
    src/resource/dlgfile.h
    src/resource/gfffile.h
    src/resource/image/curfile.h
    src/resource/image/tgafile.h
    src/resource/image/tpcfile.h
    src/resource/image/txifile.h
    src/resource/lytfile.h
    src/resource/mdlfile.h
    src/resource/ncsfile.h
    src/resource/pthfile.h
    src/resource/resources.h
    src/resource/tlkfile.h
    src/resource/types.h
    src/resource/util.h
    src/resource/visfile.h
    src/script/execution.h
    src/script/program.h
    src/script/routine.h
    src/script/types.h
    src/script/util.h
    src/script/variable.h)

set(SYSTEM_SOURCES
    src/audio/player.cpp
    src/audio/soundinstance.cpp
    src/audio/stream.cpp
    src/audio/util.cpp
    src/core/debug.cpp
    src/core/jobs.cpp
    src/core/log.cpp
    src/core/pathutil.cpp
    src/core/random.cpp
    src/core/streamutil.cpp
    src/gui/control/button.cpp
    src/gui/control/control.cpp
    src/gui/control/imagebutton.cpp
    src/gui/control/label.cpp
    src/gui/control/listbox.cpp
    src/gui/control/panel.cpp
    src/gui/control/scrollbar.cpp
    src/gui/gui.cpp
    src/gui/scenebuilder.cpp
    src/gui/textinput.cpp
    src/net/client.cpp
    src/net/connection.cpp
    src/net/command.cpp
    src/net/server.cpp
    src/render/aabb.cpp
    src/render/font.cpp
    src/render/framebuffer.cpp
    src/render/fps.cpp
    src/render/mesh/aabb.cpp
    src/render/mesh/cube.cpp
    src/render/mesh/quad.cpp
    src/render/mesh/mesh.cpp
    src/render/mesh/modelmesh.cpp
    src/render/model/model.cpp
    src/render/model/animation.cpp
    src/render/model/modelnode.cpp
    src/render/pipeline/control.cpp
    src/render/pipeline/world.cpp
    src/render/scene/aabbnode.cpp
    src/render/scene/cameranode.cpp
    src/render/scene/cubenode.cpp
    src/render/scene/lightnode.cpp
    src/render/scene/modelnodescenenode.cpp
    src/render/scene/modelscenenode.cpp
    src/render/scene/scenegraph.cpp
    src/render/scene/scenenode.cpp
    src/render/scene/scenenodeanimator.cpp
    src/render/shaders.cpp
    src/render/texture.cpp
    src/render/walkmesh.cpp
    src/render/window.cpp
    src/resource/2dafile.cpp
    src/resource/audio/mp3file.cpp
    src/resource/audio/wavfile.cpp
    src/resource/binfile.cpp
    src/resource/blueprint/creature.cpp
    src/resource/blueprint/door.cpp
    src/resource/blueprint/item.cpp
    src/resource/blueprint/placeable.cpp
    src/resource/blueprint/trigger.cpp
    src/resource/blueprint/waypoint.cpp
    src/resource/bwmfile.cpp
    src/resource/collection/biffile.cpp
    src/resource/collection/erffile.cpp
    src/resource/collection/folder.cpp
    src/resource/collection/keyfile.cpp
    src/resource/collection/pefile.cpp
    src/resource/collection/rimfile.cpp
    src/resource/dlgfile.cpp
    src/resource/gfffile.cpp
    src/resource/image/curfile.cpp
    src/resource/image/tgafile.cpp
    src/resource/image/tpcfile.cpp
    src/resource/image/txifile.cpp
    src/resource/lytfile.cpp
    src/resource/mdlfile.cpp
    src/resource/ncsfile.cpp
    src/resource/pthfile.cpp
    src/resource/resources.cpp
    src/resource/tlkfile.cpp
    src/resource/util.cpp
    src/resource/visfile.cpp
    src/script/execution.cpp
    src/script/program.cpp
    src/script/routine.cpp
    src/script/util.cpp
    src/script/variable.cpp)

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
    src/game/mp/area.h
    src/game/mp/callbacks.h
    src/game/mp/command.h
    src/game/mp/creature.h
    src/game/mp/door.h
    src/game/mp/game.h
    src/game/mp/objectfactory.h
    src/game/mp/util.h
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
    src/game/mp/area.cpp
    src/game/mp/command.cpp
    src/game/mp/creature.cpp
    src/game/mp/door.cpp
    src/game/mp/game.cpp
    src/game/mp/objectfactory.cpp
    src/game/mp/util.cpp
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

## reone executable

set(REONE_HEADERS
    src/program.h)

set(REONE_SOURCES
    src/main.cpp
    src/program.cpp)

add_executable(reone ${REONE_HEADERS} ${REONE_SOURCES})
set_target_properties(reone PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_link_libraries(reone PRIVATE libgame)

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
    target_link_libraries(reone-tools PRIVATE libgame)
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