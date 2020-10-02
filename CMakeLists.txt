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
    src/core/debug.h
    src/core/jobs.h
    src/core/log.h
    src/core/pathutil.h
    src/core/random.h
    src/core/streamutil.h
    src/core/types.h
    src/game/actions.h
    src/game/blueprint/blueprints.h
    src/game/blueprint/creature.h
    src/game/blueprint/door.h
    src/game/blueprint/item.h
    src/game/blueprint/placeable.h
    src/game/blueprint/trigger.h
    src/game/blueprint/waypoint.h
    src/game/camera/camera.h
    src/game/camera/firstperson.h
    src/game/camera/thirdperson.h
    src/game/characters.h
    src/game/collisiondetect.h
    src/game/console.h
    src/game/dialog.h
    src/game/game.h
    src/game/gui/classselect.h
    src/game/gui/container.h
    src/game/gui/debug.h
    src/game/gui/dialog.h
    src/game/gui/equip.h
    src/game/gui/hud.h
    src/game/gui/mainmenu.h
    src/game/gui/portraitselect.h
    src/game/gui/target.h
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
    src/game/pathfinder.h
    src/game/paths.h
    src/game/player.h
    src/game/room.h
    src/game/script/routines.h
    src/game/script/util.h
    src/game/types.h
    src/game/util.h
    src/gui/control/button.h
    src/gui/control/control.h
    src/gui/control/imagebutton.h
    src/gui/control/label.h
    src/gui/control/listbox.h
    src/gui/control/panel.h
    src/gui/control/scrollbar.h
    src/gui/gui.h
    src/gui/types.h
    src/net/client.h
    src/net/connection.h
    src/net/command.h
    src/net/server.h
    src/net/types.h
    src/program.h
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
    src/render/scene/aabbnode.h
    src/render/scene/cameranode.h
    src/render/scene/cubenode.h
    src/render/scene/lightnode.h
    src/render/scene/meshnode.h
    src/render/scene/modelnode.h
    src/render/scene/scenegraph.h
    src/render/scene/scenenode.h
    src/render/shaders.h
    src/render/texture.h
    src/render/types.h
    src/render/walkmesh.h
    src/render/window.h
    src/resources/2dafile.h
    src/resources/biffile.h
    src/resources/binfile.h
    src/resources/bwmfile.h
    src/resources/curfile.h
    src/resources/erffile.h
    src/resources/folder.h
    src/resources/gfffile.h
    src/resources/keyfile.h
    src/resources/lytfile.h
    src/resources/mdlfile.h
    src/resources/mp3file.h
    src/resources/ncsfile.h
    src/resources/pefile.h
    src/resources/resources.h
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
    src/script/program.h
    src/script/routine.h
    src/script/types.h
    src/script/util.h
    src/script/variable.h)

set(SOURCES
    src/audio/player.cpp
    src/audio/soundinstance.cpp
    src/audio/stream.cpp
    src/core/debug.cpp
    src/core/jobs.cpp
    src/core/log.cpp
    src/core/pathutil.cpp
    src/core/random.cpp
    src/core/streamutil.cpp
    src/game/actions.cpp
    src/game/blueprint/blueprints.cpp
    src/game/blueprint/creature.cpp
    src/game/blueprint/door.cpp
    src/game/blueprint/item.cpp
    src/game/blueprint/placeable.cpp
    src/game/blueprint/trigger.cpp
    src/game/blueprint/waypoint.cpp
    src/game/camera/camera.cpp
    src/game/camera/firstperson.cpp
    src/game/camera/thirdperson.cpp
    src/game/characters.cpp
    src/game/collisiondetect.cpp
    src/game/console.cpp
    src/game/dialog.cpp
    src/game/game.cpp
    src/game/gui/classselect.cpp
    src/game/gui/colors.cpp
    src/game/gui/container.cpp
    src/game/gui/debug.cpp
    src/game/gui/dialog.cpp
    src/game/gui/equip.cpp
    src/game/gui/hud.cpp
    src/game/gui/mainmenu.cpp
    src/game/gui/portraitselect.cpp
    src/game/gui/target.cpp
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
    src/game/pathfinder.cpp
    src/game/paths.cpp
    src/game/player.cpp
    src/game/room.cpp
    src/game/script/routines.cpp
    src/game/script/routines_common.cpp
    src/game/script/routines_kotor.cpp
    src/game/script/routines_tsl.cpp
    src/game/script/util.cpp
    src/game/util.cpp
    src/gui/control/button.cpp
    src/gui/control/control.cpp
    src/gui/control/imagebutton.cpp
    src/gui/control/label.cpp
    src/gui/control/listbox.cpp
    src/gui/control/panel.cpp
    src/gui/control/scrollbar.cpp
    src/gui/gui.cpp
    src/main.cpp
    src/net/client.cpp
    src/net/connection.cpp
    src/net/command.cpp
    src/net/server.cpp
    src/program.cpp
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
    src/render/scene/aabbnode.cpp
    src/render/scene/cameranode.cpp
    src/render/scene/cubenode.cpp
    src/render/scene/lightnode.cpp
    src/render/scene/meshnode.cpp
    src/render/scene/modelnode.cpp
    src/render/scene/scenegraph.cpp
    src/render/scene/scenenode.cpp
    src/render/shaders.cpp
    src/render/texture.cpp
    src/render/walkmesh.cpp
    src/render/window.cpp
    src/resources/2dafile.cpp
    src/resources/biffile.cpp
    src/resources/binfile.cpp
    src/resources/bwmfile.cpp
    src/resources/curfile.cpp
    src/resources/erffile.cpp
    src/resources/folder.cpp
    src/resources/gfffile.cpp
    src/resources/keyfile.cpp
    src/resources/lytfile.cpp
    src/resources/mdlfile.cpp
    src/resources/mp3file.cpp
    src/resources/ncsfile.cpp
    src/resources/pefile.cpp
    src/resources/resources.cpp
    src/resources/rimfile.cpp
    src/resources/tgafile.cpp
    src/resources/tlkfile.cpp
    src/resources/tpcfile.cpp
    src/resources/txifile.cpp
    src/resources/util.cpp
    src/resources/visfile.cpp
    src/resources/wavfile.cpp
    src/script/execution.cpp
    src/script/program.cpp
    src/script/routine.cpp
    src/script/util.cpp
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
        src/core/debug.h
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
        src/resources/tlkfile.h
        src/resources/util.h
        tools/program.h
        tools/tools.h)

    set(TOOLS_SOURCES
        src/core/debug.cpp
        src/core/log.cpp
        src/core/pathutil.cpp
        src/resources/2dafile.cpp
        src/resources/biffile.cpp
        src/resources/binfile.cpp
        src/resources/erffile.cpp
        src/resources/gfffile.cpp
        src/resources/keyfile.cpp
        src/resources/rimfile.cpp
        src/resources/tlkfile.cpp
        src/resources/util.cpp
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

    target_compile_definitions(reone-tools PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)
    target_include_directories(reone-tools SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
    target_include_directories(reone-tools PRIVATE ${CMAKE_SOURCE_DIR})
    target_link_libraries(reone-tools PRIVATE ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY})

    if(USE_EXTERNAL_GLM)
        target_include_directories(reone-tools PRIVATE ${CMAKE_SOURCE_DIR}/external/glm)
    endif()
endif()
