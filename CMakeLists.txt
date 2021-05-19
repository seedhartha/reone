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

add_subdirectory(src/external/s3tc)

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

include_directories(${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} ${MAD_INCLUDE_DIR} ${FFMPEG_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/src/external/s3tc)

if(USE_EXTERNAL_GLM)
    include_directories(${CMAKE_SOURCE_DIR}/src/external/glm)
else()
    find_package(glm REQUIRED)
endif()

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS)

if(WIN32)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

## libcommon static library

set(COMMON_HEADERS
    src/engine/common/cache.h
    src/engine/common/collectionutil.h
    src/engine/common/log.h
    src/engine/common/mediastream.h
    src/engine/common/pathutil.h
    src/engine/common/random.h
    src/engine/common/stopwatch.h
    src/engine/common/streamreader.h
    src/engine/common/streamutil.h
    src/engine/common/streamwriter.h
    src/engine/common/timer.h
    src/engine/common/types.h)

set(COMMON_SOURCES
    src/engine/common/log.cpp
    src/engine/common/pathutil.cpp
    src/engine/common/random.cpp
    src/engine/common/stopwatch.cpp
    src/engine/common/streamreader.cpp
    src/engine/common/streamutil.cpp
    src/engine/common/streamwriter.cpp
    src/engine/common/timer.cpp)

add_library(libcommon STATIC ${COMMON_HEADERS} ${COMMON_SOURCES})
set_target_properties(libcommon PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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
    src/engine/resource/strings.h
    src/engine/resource/talktable.h
    src/engine/resource/types.h
    src/engine/resource/typeutil.h)

set(RESOURCE_SOURCES
    src/engine/resource/2da.cpp
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
    src/engine/resource/strings.cpp
    src/engine/resource/talktable.cpp
    src/engine/resource/typeutil.cpp)

add_library(libresource STATIC ${RESOURCE_HEADERS} ${RESOURCE_SOURCES})
set_target_properties(libresource PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libresource static library

## libgraphics static library

set(GRAPHICS_HEADERS
    src/engine/graphics/aabb.h
    src/engine/graphics/baryutil.h
    src/engine/graphics/beziercurve.h
    src/engine/graphics/cursor.h
    src/engine/graphics/featureutil.h
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
    src/engine/graphics/pbribl.h
    src/engine/graphics/pixelutil.h
    src/engine/graphics/renderbuffer.h
    src/engine/graphics/shader/shaders.h
    src/engine/graphics/stateutil.h
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
    src/engine/graphics/cursor.cpp
    src/engine/graphics/featureutil.cpp
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
    src/engine/graphics/shader/shaders.cpp
    src/engine/graphics/shader/shaders_common.cpp
    src/engine/graphics/shader/shaders_pbr.cpp
    src/engine/graphics/shader/shaders_phong.cpp
    src/engine/graphics/stateutil.cpp
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

## END libgraphics static library

## libaudio static library

set(AUDIO_HEADERS
    src/engine/audio/files.h
    src/engine/audio/format/mp3reader.h
    src/engine/audio/format/wavreader.h
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

## END libvideo static library

## libscene static library

set(SCENE_HEADERS
    src/engine/scene/animeventlistener.h
    src/engine/scene/animproperties.h
    src/engine/scene/node/cameranode.h
    src/engine/scene/node/dummynode.h
    src/engine/scene/node/emitternode.h
    src/engine/scene/node/grassnode.h
    src/engine/scene/node/lightnode.h
    src/engine/scene/node/meshnode.h
    src/engine/scene/node/modelnode.h
    src/engine/scene/node/modelnodescenenode.h
    src/engine/scene/node/scenenode.h
    src/engine/scene/nodeelement.h
    src/engine/scene/pipeline/control.h
    src/engine/scene/pipeline/world.h
    src/engine/scene/scenegraph.h
    src/engine/scene/types.h)

set(SCENE_SOURCES
    src/engine/scene/node/cameranode.cpp
    src/engine/scene/node/emitternode.cpp
    src/engine/scene/node/grassnode.cpp
    src/engine/scene/node/lightnode.cpp
    src/engine/scene/node/meshnode.cpp
    src/engine/scene/node/modelnode.cpp
    src/engine/scene/node/modelnode_animation.cpp
    src/engine/scene/node/modelnodescenenode.cpp
    src/engine/scene/node/scenenode.cpp
    src/engine/scene/pipeline/control.cpp
    src/engine/scene/pipeline/world.cpp
    src/engine/scene/scenegraph.cpp)

add_library(libscene STATIC ${SCENE_HEADERS} ${SCENE_SOURCES})
set_target_properties(libscene PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

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
    src/engine/gui/control/scrollbar.cpp
    src/engine/gui/control/togglebutton.cpp
    src/engine/gui/gui.cpp
    src/engine/gui/scenebuilder.cpp
    src/engine/gui/textinput.cpp)

add_library(libgui STATIC ${GUI_HEADERS} ${GUI_SOURCES})
set_target_properties(libgui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libgui static library

## libscript static library

set(SCRIPT_HEADERS
    src/engine/script/enginetype.h
    src/engine/script/execution.h
    src/engine/script/instrutil.h
    src/engine/script/ncsreader.h
    src/engine/script/program.h
    src/engine/script/routine.h
    src/engine/script/scripts.h
    src/engine/script/types.h
    src/engine/script/variable.h)

set(SCRIPT_SOURCES
    src/engine/script/execution.cpp
    src/engine/script/instrutil.cpp
    src/engine/script/ncsreader.cpp
    src/engine/script/program.cpp
    src/engine/script/routine.cpp
    src/engine/script/scripts.cpp
    src/engine/script/variable.cpp)

add_library(libscript STATIC ${SCRIPT_HEADERS} ${SCRIPT_SOURCES})
set_target_properties(libscript PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libscript static library

## libgame static library

set(GAME_HEADERS
    src/engine/game/action/action.h
    src/engine/game/action/attack.h
    src/engine/game/action/commandaction.h
    src/engine/game/action/follow.h
    src/engine/game/action/locationaction.h
    src/engine/game/action/movetolocation.h
    src/engine/game/action/movetoobject.h
    src/engine/game/action/movetopoint.h
    src/engine/game/action/objectaction.h
    src/engine/game/action/playanimation.h
    src/engine/game/action/startconversation.h
    src/engine/game/action/waitaction.h
    src/engine/game/animationutil.h
    src/engine/game/camera/animatedcamera.h
    src/engine/game/camera/camera.h
    src/engine/game/camera/camerastyle.h
    src/engine/game/camera/dialogcamera.h
    src/engine/game/camera/firstperson.h
    src/engine/game/camera/staticcamera.h
    src/engine/game/camera/thirdperson.h
    src/engine/game/character.h
    src/engine/game/characterutil.h
    src/engine/game/console.h
    src/engine/game/combat/combat.h
    src/engine/game/cursors.h
    src/engine/game/dialog.h
    src/engine/game/enginetype/effect.h
    src/engine/game/enginetype/event.h
    src/engine/game/enginetype/location.h
    src/engine/game/enginetype/talent.h
    src/engine/game/footstepsounds.h
    src/engine/game/game.h
    src/engine/game/gameidutil.h
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
    src/engine/game/gui/colorutil.h
    src/engine/game/gui/container.h
    src/engine/game/gui/conversation.h
    src/engine/game/gui/computer.h
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
    src/engine/game/objectconverter.h
    src/engine/game/options.h
    src/engine/game/party.h
    src/engine/game/path.h
    src/engine/game/pathfinder.h
    src/engine/game/player.h
    src/engine/game/portrait.h
    src/engine/game/portraits.h
    src/engine/game/reputes.h
    src/engine/game/room.h
    src/engine/game/d20/attributes.h
    src/engine/game/d20/class.h
    src/engine/game/d20/classes.h
    src/engine/game/d20/feat.h
    src/engine/game/d20/feats.h
    src/engine/game/d20/savingthrows.h
    src/engine/game/d20/spell.h
    src/engine/game/d20/spells.h
    src/engine/game/savedgame.h
    src/engine/game/script/objectutil.h
    src/engine/game/script/routines.h
    src/engine/game/script/runutil.h
    src/engine/game/soundsets.h
    src/engine/game/surface.h
    src/engine/game/surfaces.h
    src/engine/game/types.h)

set(GAME_SOURCES
    src/engine/game/actionexecutor.cpp
    src/engine/game/animationutil.cpp
    src/engine/game/camera/animatedcamera.cpp
    src/engine/game/camera/camera.cpp
    src/engine/game/camera/camerastyle.cpp
    src/engine/game/camera/dialogcamera.cpp
    src/engine/game/camera/firstperson.cpp
    src/engine/game/camera/staticcamera.cpp
    src/engine/game/camera/thirdperson.cpp
    src/engine/game/characterutil.cpp
    src/engine/game/combat/combat.cpp
    src/engine/game/combat/combat_attack.cpp
    src/engine/game/combat/combat_damage.cpp
    src/engine/game/combat/combat_projectiles.cpp
    src/engine/game/console.cpp
    src/engine/game/cursors.cpp
    src/engine/game/d20/attributes.cpp
    src/engine/game/d20/attributes_abilities.cpp
    src/engine/game/d20/attributes_classes.cpp
    src/engine/game/d20/attributes_skills.cpp
    src/engine/game/d20/class.cpp
    src/engine/game/d20/classes.cpp
    src/engine/game/d20/feats.cpp
    src/engine/game/d20/spells.cpp
    src/engine/game/dialog.cpp
    src/engine/game/footstepsounds.cpp
    src/engine/game/game.cpp
    src/engine/game/game_kotor.cpp
    src/engine/game/game_save.cpp
    src/engine/game/game_tsl.cpp
    src/engine/game/gameidutil.cpp
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
    src/engine/game/gui/colorutil.cpp
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
    src/engine/game/map.cpp
    src/engine/game/object/area.cpp
    src/engine/game/object/area_are.cpp
    src/engine/game/object/area_collision.cpp
    src/engine/game/object/area_git.cpp
    src/engine/game/object/area_perception.cpp
    src/engine/game/object/area_search.cpp
    src/engine/game/object/area_selection.cpp
    src/engine/game/object/creature.cpp
    src/engine/game/object/creature_animations.cpp
    src/engine/game/object/creature_appearance.cpp
    src/engine/game/object/creature_blueprint.cpp
    src/engine/game/object/door.cpp
    src/engine/game/object/door_blueprint.cpp
    src/engine/game/object/encounter.cpp
    src/engine/game/object/encounter_blueprint.cpp
    src/engine/game/object/item.cpp
    src/engine/game/object/item_blueprint.cpp
    src/engine/game/object/module.cpp
    src/engine/game/object/object.cpp
    src/engine/game/object/object_actions.cpp
    src/engine/game/object/objectfactory.cpp
    src/engine/game/object/placeable.cpp
    src/engine/game/object/placeable_blueprint.cpp
    src/engine/game/object/placeablecamera.cpp
    src/engine/game/object/sound.cpp
    src/engine/game/object/sound_blueprint.cpp
    src/engine/game/object/spatial.cpp
    src/engine/game/object/trigger.cpp
    src/engine/game/object/trigger_blueprint.cpp
    src/engine/game/object/waypoint.cpp
    src/engine/game/object/waypoint_blueprint.cpp
    src/engine/game/party.cpp
    src/engine/game/path.cpp
    src/engine/game/pathfinder.cpp
    src/engine/game/player.cpp
    src/engine/game/portraits.cpp
    src/engine/game/reputes.cpp
    src/engine/game/room.cpp
    src/engine/game/script/routines.cpp
    src/engine/game/script/routines_actions.cpp
    src/engine/game/script/routines_combat.cpp
    src/engine/game/script/routines_common.cpp
    src/engine/game/script/routines_d20.cpp
    src/engine/game/script/routines_effects.cpp
    src/engine/game/script/routines_events.cpp
    src/engine/game/script/routines_faction.cpp
    src/engine/game/script/routines_items.cpp
    src/engine/game/script/routines_kotor.cpp
    src/engine/game/script/routines_locations.cpp
    src/engine/game/script/routines_math.cpp
    src/engine/game/script/routines_objects.cpp
    src/engine/game/script/routines_party.cpp
    src/engine/game/script/routines_perception.cpp
    src/engine/game/script/routines_stealth.cpp
    src/engine/game/script/routines_strings.cpp
    src/engine/game/script/routines_time.cpp
    src/engine/game/script/routines_tsl.cpp
    src/engine/game/script/routines_vars.cpp
    src/engine/game/script/runutil.cpp
    src/engine/game/soundsets.cpp
    src/engine/game/surfaces.cpp)

add_library(libgame STATIC ${GAME_HEADERS} ${GAME_SOURCES})
set_target_properties(libgame PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

## END libgame static library

## reone executable

set(REONE_HEADERS
    src/engine/program.h)

set(REONE_SOURCES
    src/engine/main.cpp
    src/engine/program.cpp)

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
    else()
        message(WARNING "wxWidgets not found - launcher cannot be built")
    endif()

endif()

## END reone-launcher executable

## Unit tests

if(BUILD_TESTS)
    enable_testing()
    file(GLOB TEST_FILES "src/tests/*.cpp")
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
