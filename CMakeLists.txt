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

set(HEADERS
    src/audio/player.h
    src/audio/soundinstance.h
    src/audio/stream.h
    src/audio/types.h
    src/core/jobs.h
    src/core/log.h
    src/core/pathutil.h
    src/core/random.h
    src/core/streamutil.h
    src/core/types.h
    src/game/area.h
    src/game/dialog.h
    src/game/game.h
    src/game/gui/classselection.h
    src/game/gui/debug.h
    src/game/gui/dialog.h
    src/game/gui/hud.h
    src/game/gui/mainmenu.h
    src/game/gui/portraits.h
    src/game/item.h
    src/game/navmesh.h
    src/game/module.h
    src/game/multiplayer/area.h
    src/game/multiplayer/callbacks.h
    src/game/multiplayer/command.h
    src/game/multiplayer/creature.h
    src/game/multiplayer/door.h
    src/game/multiplayer/game.h
    src/game/multiplayer/module.h
    src/game/object/creature.h
    src/game/object/door.h
    src/game/object/object.h
    src/game/object/placeable.h
    src/game/object/trigger.h
    src/game/object/waypoint.h
    src/game/room.h
    src/game/templates.h
    src/game/types.h
    src/gui/control/button.h
    src/gui/control/control.h
    src/gui/control/label.h
    src/gui/control/listbox.h
    src/gui/control/panel.h
    src/gui/control/scrollbar.h
    src/gui/gui.h
    src/gui/types.h
    src/net/client.h
    src/net/connection.h
    src/net/server.h
    src/net/types.h
    src/program.h
    src/render/aabb.h
    src/render/animation.h
    src/render/camera/camera.h
    src/render/camera/firstperson.h
    src/render/camera/thirdperson.h
    src/render/font.h
    src/render/mesh/aabb.h
    src/render/mesh/guiquad.h
    src/render/mesh/mesh.h
    src/render/mesh/modelmesh.h
    src/render/model.h
    src/render/modelinstance.h
    src/render/modelnode.h
    src/render/shadermanager.h
    src/render/texture.h
    src/render/types.h
    src/render/walkmesh.h
    src/render/window.h
    src/resources/2dafile.h
    src/resources/biffile.h
    src/resources/binfile.h
    src/resources/bwmfile.h
    src/resources/erffile.h
    src/resources/folder.h
    src/resources/gfffile.h
    src/resources/keyfile.h
    src/resources/lytfile.h
    src/resources/manager.h
    src/resources/mdlfile.h
    src/resources/mp3file.h
    src/resources/ncsfile.h
    src/resources/rimfile.h
    src/resources/tgafile.h
    src/resources/tlkfile.h
    src/resources/tpcfile.h
    src/resources/types.h
    src/resources/txifile.h
    src/resources/util.h
    src/resources/visfile.h
    src/resources/wavfile.h
    src/script/execution.h
    src/script/manager.h
    src/script/program.h
    src/script/routines.h
    src/script/types.h
    src/script/variable.h)

set(SOURCES
    src/audio/player.cpp
    src/audio/soundinstance.cpp
    src/audio/stream.cpp
    src/core/jobs.cpp
    src/core/log.cpp
    src/core/pathutil.cpp
    src/core/random.cpp
    src/core/streamutil.cpp
    src/game/area.cpp
    src/game/dialog.cpp
    src/game/game.cpp
    src/game/gui/classselection.cpp
    src/game/gui/colors.cpp
    src/game/gui/debug.cpp
    src/game/gui/dialog.cpp
    src/game/gui/hud.cpp
    src/game/gui/mainmenu.cpp
    src/game/gui/portraits.cpp
    src/game/item.cpp
    src/game/module.cpp
    src/game/multiplayer/area.cpp
    src/game/multiplayer/command.cpp
    src/game/multiplayer/creature.cpp
    src/game/multiplayer/door.cpp
    src/game/multiplayer/game.cpp
    src/game/multiplayer/module.cpp
    src/game/navmesh.cpp
    src/game/object/creature.cpp
    src/game/object/door.cpp
    src/game/object/object.cpp
    src/game/object/placeable.cpp
    src/game/object/trigger.cpp
    src/game/object/waypoint.cpp
    src/game/room.cpp
    src/game/templates.cpp
    src/gui/control/button.cpp
    src/gui/control/control.cpp
    src/gui/control/label.cpp
    src/gui/control/listbox.cpp
    src/gui/control/panel.cpp
    src/gui/control/scrollbar.cpp
    src/gui/gui.cpp
    src/main.cpp
    src/net/client.cpp
    src/net/connection.cpp
    src/net/server.cpp
    src/program.cpp
    src/render/aabb.cpp
    src/render/animation.cpp
    src/render/camera/camera.cpp
    src/render/camera/firstperson.cpp
    src/render/camera/thirdperson.cpp
    src/render/font.cpp
    src/render/mesh/aabb.cpp
    src/render/mesh/guiquad.cpp
    src/render/mesh/mesh.cpp
    src/render/mesh/modelmesh.cpp
    src/render/model.cpp
    src/render/modelinstance.cpp
    src/render/modelnode.cpp
    src/render/shadermanager.cpp
    src/render/texture.cpp
    src/render/walkmesh.cpp
    src/render/window.cpp
    src/resources/2dafile.cpp
    src/resources/biffile.cpp
    src/resources/binfile.cpp
    src/resources/bwmfile.cpp
    src/resources/erffile.cpp
    src/resources/folder.cpp
    src/resources/gfffile.cpp
    src/resources/keyfile.cpp
    src/resources/lytfile.cpp
    src/resources/manager.cpp
    src/resources/mdlfile.cpp
    src/resources/mp3file.cpp
    src/resources/ncsfile.cpp
    src/resources/rimfile.cpp
    src/resources/tgafile.cpp
    src/resources/tlkfile.cpp
    src/resources/tpcfile.cpp
    src/resources/txifile.cpp
    src/resources/util.cpp
    src/resources/visfile.cpp
    src/resources/wavfile.cpp
    src/script/execution.cpp
    src/script/manager.cpp
    src/script/program.cpp
    src/script/routines.cpp
    src/script/routines_kotor.cpp
    src/script/routines_tsl.cpp
    src/script/variable.cpp)

add_executable(reone ${HEADERS} ${SOURCES})

target_compile_definitions(reone PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)
target_include_directories(reone SYSTEM PRIVATE ${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR})
target_link_libraries(reone PRIVATE ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY} GLEW::GLEW ${OPENGL_LIBRARIES} ${MAD_LIBRARY})

if(WIN32)
    target_link_libraries(reone PRIVATE SDL2::SDL2 OpenAL::OpenAL)
else()
    target_link_libraries(reone PRIVATE ${SDL2_LIBRARIES} ${OpenAL_LIBRARIES} Threads::Threads)
endif()

if(USE_EXTERNAL_GLM)
    target_include_directories(reone PRIVATE ${CMAKE_SOURCE_DIR}/external/glm)
endif()

if(BUILD_TOOLS)
    set(TOOLS_HEADERS
        src/core/log.h
        src/core/pathutil.h
        src/core/types.h
        src/resources/2dafile.h
        src/resources/biffile.h
        src/resources/binfile.h
        src/resources/erffile.h
        src/resources/gfffile.h
        src/resources/keyfile.h
        src/resources/rimfile.h
        src/resources/util.h
        tools/program.h
        tools/tools.h)

    set(TOOLS_SOURCES
        src/core/log.cpp
        src/core/pathutil.cpp
        src/resources/2dafile.cpp
        src/resources/biffile.cpp
        src/resources/binfile.cpp
        src/resources/erffile.cpp
        src/resources/gfffile.cpp
        src/resources/keyfile.cpp
        src/resources/rimfile.cpp
        src/resources/util.cpp
        tools/main.cpp
        tools/2datool.cpp
        tools/biftool.cpp
        tools/erftool.cpp
        tools/gfftool.cpp
        tools/keytool.cpp
        tools/program.cpp
        tools/rimtool.cpp
        tools/tools.cpp)

    add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})

    target_compile_definitions(reone-tools PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)
    target_include_directories(reone-tools SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(reone-tools PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(reone-tools PRIVATE ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

    if(USE_EXTERNAL_GLM)
        target_include_directories(reone-tools PRIVATE ${CMAKE_SOURCE_DIR}/external/glm)
    endif()
endif()
