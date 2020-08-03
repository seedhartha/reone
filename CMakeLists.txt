cmake_minimum_required(VERSION 3.10)

project(reone)

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
    src/game/game.h
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
    src/gui/debug.h
    src/gui/gui.h
    src/gui/hud.h
    src/gui/mainmenu.h
    src/gui/modules.h
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
    src/resources/rimfile.h
    src/resources/tgafile.h
    src/resources/tlkfile.h
    src/resources/tpcfile.h
    src/resources/types.h
    src/resources/txifile.h
    src/resources/util.h
    src/resources/visfile.h
    src/resources/wavfile.h
    src/tools/tools.h)

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
    src/game/game.cpp
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
    src/gui/debug.cpp
    src/gui/gui.cpp
    src/gui/hud.cpp
    src/gui/mainmenu.cpp
    src/gui/modules.cpp
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
    src/resources/rimfile.cpp
    src/resources/tgafile.cpp
    src/resources/tlkfile.cpp
    src/resources/tpcfile.cpp
    src/resources/txifile.cpp
    src/resources/util.cpp
    src/resources/visfile.cpp
    src/resources/wavfile.cpp
    src/tools/tools.cpp)

option(WITH_TOOLS "build with list, extract and convert tools" ON)
option(USE_EXTERNAL_GLM "use GLM library from external subdirectory" OFF)

if(WITH_TOOLS)
    list(APPEND SOURCES
        src/tools/2datool.cpp
        src/tools/biftool.cpp
        src/tools/erftool.cpp
        src/tools/gfftool.cpp
        src/tools/keytool.cpp
        src/tools/rimtool.cpp)
endif()

add_executable(reone ${HEADERS} ${SOURCES})

set_target_properties(reone PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Boost REQUIRED COMPONENTS filesystem program_options system)
find_package(SDL2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(MAD REQUIRED)

if(NOT USE_EXTERNAL_GLM)
    find_package(glm REQUIRED)
endif()

target_compile_definitions(reone PRIVATE BOOST_BIND_GLOBAL_PLACEHOLDERS)
target_include_directories(reone SYSTEM PRIVATE ${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR})
target_link_libraries(reone PRIVATE ${Boost_FILESYSTEM_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_SYSTEM_LIBRARY} GLEW::GLEW ${OPENGL_LIBRARIES} ${MAD_LIBRARY})

if(WIN32)
    find_package(OpenAL CONFIG REQUIRED)
    target_link_libraries(reone PRIVATE SDL2::SDL2 OpenAL::OpenAL)
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(OpenAL REQUIRED openal)
    find_package(Threads REQUIRED)
    target_link_libraries(reone PRIVATE ${SDL2_LIBRARIES} ${OpenAL_LIBRARIES} Threads::Threads)
endif()

if(WITH_TOOLS)
    target_compile_definitions(reone PRIVATE REONE_WITH_TOOLS)
endif()

if(USE_EXTERNAL_GLM)
    target_include_directories(reone PRIVATE ${CMAKE_SOURCE_DIR}/external/glm)
endif()
