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

include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/external/s3tc)

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
    src/resource/gffstruct.h
    src/resource/folder.h
    src/resource/format/2dareader.h
    src/resource/format/2dawriter.h
    src/resource/format/bifreader.h
    src/resource/format/binreader.h
    src/resource/format/erfreader.h
    src/resource/format/erfwriter.h
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
    src/resource/format/tlkwriter.h
    src/resource/format/visreader.h
    src/resource/keybifprovider.h
    src/resource/resourceprovider.h
    src/resource/resources.h
    src/resource/strings.h
    src/resource/talktable.h
    src/resource/types.h
    src/resource/typeutil.h)

set(RESOURCE_SOURCES
    src/resource/2da.cpp
    src/resource/gffstruct.cpp
    src/resource/folder.cpp
    src/resource/format/2dareader.cpp
    src/resource/format/2dawriter.cpp
    src/resource/format/bifreader.cpp
    src/resource/format/binreader.cpp
    src/resource/format/erfreader.cpp
    src/resource/format/erfwriter.cpp
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
    src/resource/format/tlkwriter.cpp
    src/resource/format/visreader.cpp
    src/resource/keybifprovider.cpp
    src/resource/resources.cpp
    src/resource/strings.cpp
    src/resource/talktable.cpp
    src/resource/typeutil.cpp)

add_library(libresource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES})
set_target_properties(libresource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libresource static library

## libgraphics static library

set(GRAPHICS_HEADERS
    src/graphics/aabb.h
    src/graphics/cursor.h
    src/graphics/featureutil.h
    src/graphics/font.h
    src/graphics/fonts.h
    src/graphics/framebuffer.h
    src/graphics/image/curreader.h
    src/graphics/image/tgareader.h
    src/graphics/image/tgawriter.h
    src/graphics/image/tpcreader.h
    src/graphics/image/txireader.h
    src/graphics/lip/lipanimation.h
    src/graphics/lip/lipreader.h
    src/graphics/lip/lips.h
    src/graphics/lip/lipwriter.h
    src/graphics/material.h
    src/graphics/materials.h
    src/graphics/mesh.h
    src/graphics/meshes.h
    src/graphics/model/aabbnode.h
    src/graphics/model/animatedproperty.h
    src/graphics/model/animation.h
    src/graphics/model/emitter.h
    src/graphics/model/lensflare.h
    src/graphics/model/mdlreader.h
    src/graphics/model/model.h
    src/graphics/model/modelmesh.h
    src/graphics/model/modelnode.h
    src/graphics/model/models.h
    src/graphics/pbribl.h
    src/graphics/pixelutil.h
    src/graphics/renderbuffer.h
    src/graphics/shaders.h
    src/graphics/stateutil.h
    src/graphics/texture.h
    src/graphics/textures.h
    src/graphics/textureutil.h
    src/graphics/textutil.h
    src/graphics/types.h
    src/graphics/walkmesh/bwmreader.h
    src/graphics/walkmesh/walkmesh.h
    src/graphics/walkmesh/walkmeshes.h
    src/graphics/window.h)

set(GRAPHICS_SOURCES
    src/graphics/aabb.cpp
    src/graphics/cursor.cpp
    src/graphics/featureutil.cpp
    src/graphics/font.cpp
    src/graphics/fonts.cpp
    src/graphics/framebuffer.cpp
    src/graphics/image/curreader.cpp
    src/graphics/image/tgareader.cpp
    src/graphics/image/tgawriter.cpp
    src/graphics/image/tpcreader.cpp
    src/graphics/image/txireader.cpp
    src/graphics/lip/lipanimation.cpp
    src/graphics/lip/lipreader.cpp
    src/graphics/lip/lips.cpp
    src/graphics/lip/lipwriter.cpp
    src/graphics/materials.cpp
    src/graphics/mesh.cpp
    src/graphics/meshes.cpp
    src/graphics/model/aabbnode.cpp
    src/graphics/model/animation.cpp
    src/graphics/model/mdlreader.cpp
    src/graphics/model/model.cpp
    src/graphics/model/modelmesh.cpp
    src/graphics/model/modelnode.cpp
    src/graphics/model/models.cpp
    src/graphics/pbribl.cpp
    src/graphics/pixelutil.cpp
    src/graphics/renderbuffer.cpp
    src/graphics/shaders.cpp
    src/graphics/stateutil.cpp
    src/graphics/texture.cpp
    src/graphics/textures.cpp
    src/graphics/textureutil.cpp
    src/graphics/textutil.cpp
    src/graphics/walkmesh/bwmreader.cpp
    src/graphics/walkmesh/walkmesh.cpp
    src/graphics/walkmesh/walkmeshes.cpp
    src/graphics/window.cpp)

add_library(libgraphics STATIC ${GRAPHICS_HEADERS} ${GRAPHICS_SOURCES})
set_target_properties(libgraphics PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libgraphics static library

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
    src/scene/animation/eventlistener.h
    src/scene/animation/channel.h
    src/scene/animation/properties.h
    src/scene/animation/scenenodeanimator.h
    src/scene/animation/scenenodestate.h
    src/scene/grasscluster.h
    src/scene/node/cameranode.h
    src/scene/node/emitternode.h
    src/scene/node/grassnode.h
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
    src/scene/node/grassnode.cpp
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
    src/script/program.h
    src/script/routine.h
    src/script/scripts.h
    src/script/types.h
    src/script/variable.h)

set(SCRIPT_SOURCES
    src/script/execution.cpp
    src/script/instrutil.cpp
    src/script/ncsreader.cpp
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
    src/game/camera/animatedcamera.h
    src/game/camera/camera.h
    src/game/camera/camerastyle.h
    src/game/camera/dialogcamera.h
    src/game/camera/firstperson.h
    src/game/camera/staticcamera.h
    src/game/camera/thirdperson.h
    src/game/character.h
    src/game/characterutil.h
    src/game/collisiondetect.h
    src/game/console.h
    src/game/combat/combat.h
    src/game/creaturefinder.h
    src/game/cursors.h
    src/game/dialog.h
    src/game/enginetype/effect.h
    src/game/enginetype/event.h
    src/game/enginetype/location.h
    src/game/enginetype/talent.h
    src/game/footstepsounds.h
    src/game/game.h
    src/game/gameidutil.h
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
    src/game/perception.h
    src/game/player.h
    src/game/portrait.h
    src/game/portraits.h
    src/game/reputes.h
    src/game/room.h
    src/game/d20/abilities.h
    src/game/d20/attributes.h
    src/game/d20/class.h
    src/game/d20/classes.h
    src/game/d20/savingthrows.h
    src/game/d20/skills.h
    src/game/savedgame.h
    src/game/script/objectutil.h
    src/game/script/routines.h
    src/game/script/runutil.h
    src/game/soundsets.h
    src/game/surface.h
    src/game/surfaces.h
    src/game/types.h)

set(GAME_SOURCES
    src/game/actionexecutor.cpp
    src/game/actionqueue.cpp
    src/game/animationutil.cpp
    src/game/camera/animatedcamera.cpp
    src/game/camera/camera.cpp
    src/game/camera/camerastyle.cpp
    src/game/camera/dialogcamera.cpp
    src/game/camera/firstperson.cpp
    src/game/camera/staticcamera.cpp
    src/game/camera/thirdperson.cpp
    src/game/characterutil.cpp
    src/game/collisiondetect.cpp
    src/game/combat/combat.cpp
    src/game/combat/combat_attack.cpp
    src/game/combat/combat_damage.cpp
    src/game/combat/combat_projectiles.cpp
    src/game/console.cpp
    src/game/creaturefinder.cpp
    src/game/cursors.cpp
    src/game/d20/abilities.cpp
    src/game/d20/attributes.cpp
    src/game/d20/class.cpp
    src/game/d20/classes.cpp
    src/game/d20/skills.cpp
    src/game/dialog.cpp
    src/game/footstepsounds.cpp
    src/game/game.cpp
    src/game/game_kotor.cpp
    src/game/game_save.cpp
    src/game/game_tsl.cpp
    src/game/gameidutil.cpp
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
    src/game/gui/computer.cpp
    src/game/gui/container.cpp
    src/game/gui/conversation.cpp
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
    src/game/object/area_are.cpp
    src/game/object/area_git.cpp
    src/game/object/creature.cpp
    src/game/object/creature_animations.cpp
    src/game/object/creature_appearance.cpp
    src/game/object/creature_blueprint.cpp
    src/game/object/door.cpp
    src/game/object/door_blueprint.cpp
    src/game/object/encounter.cpp
    src/game/object/encounter_blueprint.cpp
    src/game/object/item.cpp
    src/game/object/item_blueprint.cpp
    src/game/object/module.cpp
    src/game/object/object.cpp
    src/game/object/objectfactory.cpp
    src/game/object/placeable.cpp
    src/game/object/placeable_blueprint.cpp
    src/game/object/placeablecamera.cpp
    src/game/object/sound.cpp
    src/game/object/sound_blueprint.cpp
    src/game/object/spatial.cpp
    src/game/object/trigger.cpp
    src/game/object/trigger_blueprint.cpp
    src/game/object/waypoint.cpp
    src/game/object/waypoint_blueprint.cpp
    src/game/objectselect.cpp
    src/game/party.cpp
    src/game/path.cpp
    src/game/pathfinder.cpp
    src/game/perception.cpp
    src/game/player.cpp
    src/game/portraits.cpp
    src/game/reputes.cpp
    src/game/room.cpp
    src/game/script/routines.cpp
    src/game/script/routines_actions.cpp
    src/game/script/routines_combat.cpp
    src/game/script/routines_common.cpp
    src/game/script/routines_d20.cpp
    src/game/script/routines_effects.cpp
    src/game/script/routines_events.cpp
    src/game/script/routines_faction.cpp
    src/game/script/routines_items.cpp
    src/game/script/routines_kotor.cpp
    src/game/script/routines_locations.cpp
    src/game/script/routines_math.cpp
    src/game/script/routines_objects.cpp
    src/game/script/routines_party.cpp
    src/game/script/routines_perception.cpp
    src/game/script/routines_stealth.cpp
    src/game/script/routines_strings.cpp
    src/game/script/routines_time.cpp
    src/game/script/routines_tsl.cpp
    src/game/script/routines_vars.cpp
    src/game/script/runutil.cpp
    src/game/soundsets.cpp
    src/game/surfaces.cpp)

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
    libgame libscript libgui libscene libvideo libaudio libgraphics libresource libcommon
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
        tools/audiotool.cpp
        tools/erftool.cpp
        tools/gfftool.cpp
        tools/keybiftool.cpp
        tools/liptool.cpp
        tools/main.cpp
        tools/program.cpp
        tools/pthtool.cpp
        tools/rimtool.cpp
        tools/tlktool.cpp
        tools/tpctool.cpp)

    add_executable(reone-tools ${TOOLS_HEADERS} ${TOOLS_SOURCES})
    set_target_properties(reone-tools PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

    target_link_libraries(reone-tools PRIVATE
        libgraphics libresource libcommon
        libs3tc
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
