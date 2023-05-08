# Copyright (c) 2020-2022 The reone project contributors

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

set(GAME_INCLUDE_DIR ${REONE_INCLUDE_DIR}/reone/game)
set(GAME_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/game)

set(GAME_HEADERS
    ${GAME_INCLUDE_DIR}/action.h
    ${GAME_INCLUDE_DIR}/action/attackobject.h
    ${GAME_INCLUDE_DIR}/action/barkstring.h
    ${GAME_INCLUDE_DIR}/action/castfakespellatlocation.h
    ${GAME_INCLUDE_DIR}/action/castfakespellatobject.h
    ${GAME_INCLUDE_DIR}/action/castspellatobject.h
    ${GAME_INCLUDE_DIR}/action/closedoor.h
    ${GAME_INCLUDE_DIR}/action/docommand.h
    ${GAME_INCLUDE_DIR}/action/equipitem.h
    ${GAME_INCLUDE_DIR}/action/equipmostdamagingmelee.h
    ${GAME_INCLUDE_DIR}/action/equipmostdamagingranged.h
    ${GAME_INCLUDE_DIR}/action/follow.h
    ${GAME_INCLUDE_DIR}/action/followleader.h
    ${GAME_INCLUDE_DIR}/action/followowner.h
    ${GAME_INCLUDE_DIR}/action/giveitem.h
    ${GAME_INCLUDE_DIR}/action/interactobject.h
    ${GAME_INCLUDE_DIR}/action/jumptolocation.h
    ${GAME_INCLUDE_DIR}/action/jumptoobject.h
    ${GAME_INCLUDE_DIR}/action/lockobject.h
    ${GAME_INCLUDE_DIR}/action/moveawayfromobject.h
    ${GAME_INCLUDE_DIR}/action/movetolocation.h
    ${GAME_INCLUDE_DIR}/action/movetoobject.h
    ${GAME_INCLUDE_DIR}/action/movetopoint.h
    ${GAME_INCLUDE_DIR}/action/opencontainer.h
    ${GAME_INCLUDE_DIR}/action/opendoor.h
    ${GAME_INCLUDE_DIR}/action/openlock.h
    ${GAME_INCLUDE_DIR}/action/pauseconversation.h
    ${GAME_INCLUDE_DIR}/action/playanimation.h
    ${GAME_INCLUDE_DIR}/action/randomwalk.h
    ${GAME_INCLUDE_DIR}/action/resumeconversation.h
    ${GAME_INCLUDE_DIR}/action/startconversation.h
    ${GAME_INCLUDE_DIR}/action/surrendertoenemies.h
    ${GAME_INCLUDE_DIR}/action/switchweapons.h
    ${GAME_INCLUDE_DIR}/action/takeitem.h
    ${GAME_INCLUDE_DIR}/action/unequipitem.h
    ${GAME_INCLUDE_DIR}/action/unlockobject.h
    ${GAME_INCLUDE_DIR}/action/usefeat.h
    ${GAME_INCLUDE_DIR}/action/useskill.h
    ${GAME_INCLUDE_DIR}/action/usetalentonobject.h
    ${GAME_INCLUDE_DIR}/action/wait.h
    ${GAME_INCLUDE_DIR}/astar.h
    ${GAME_INCLUDE_DIR}/camerastyle.h
    ${GAME_INCLUDE_DIR}/camerastyles.h
    ${GAME_INCLUDE_DIR}/conversation.h
    ${GAME_INCLUDE_DIR}/cursors.h
    ${GAME_INCLUDE_DIR}/debug.h
    ${GAME_INCLUDE_DIR}/effect.h
    ${GAME_INCLUDE_DIR}/effect/abilitydecrease.h
    ${GAME_INCLUDE_DIR}/effect/abilityincrease.h
    ${GAME_INCLUDE_DIR}/effect/acdecrease.h
    ${GAME_INCLUDE_DIR}/effect/acincrease.h
    ${GAME_INCLUDE_DIR}/effect/assuredhit.h
    ${GAME_INCLUDE_DIR}/effect/attackdecrease.h
    ${GAME_INCLUDE_DIR}/effect/attackincrease.h
    ${GAME_INCLUDE_DIR}/effect/beam.h
    ${GAME_INCLUDE_DIR}/effect/blasterdeflectionincrease.h
    ${GAME_INCLUDE_DIR}/effect/blind.h
    ${GAME_INCLUDE_DIR}/effect/bodyfuel.h
    ${GAME_INCLUDE_DIR}/effect/choke.h
    ${GAME_INCLUDE_DIR}/effect/crush.h
    ${GAME_INCLUDE_DIR}/effect/cutscenehorrified.h
    ${GAME_INCLUDE_DIR}/effect/cutsceneparalyze.h
    ${GAME_INCLUDE_DIR}/effect/cutscenestunned.h
    ${GAME_INCLUDE_DIR}/effect/damage.h
    ${GAME_INCLUDE_DIR}/effect/damagedecrease.h
    ${GAME_INCLUDE_DIR}/effect/damageforcepoints.h
    ${GAME_INCLUDE_DIR}/effect/damageimmunityincrease.h
    ${GAME_INCLUDE_DIR}/effect/damageincrease.h
    ${GAME_INCLUDE_DIR}/effect/damageresistance.h
    ${GAME_INCLUDE_DIR}/effect/death.h
    ${GAME_INCLUDE_DIR}/effect/disguise.h
    ${GAME_INCLUDE_DIR}/effect/droidscramble.h
    ${GAME_INCLUDE_DIR}/effect/droidstun.h
    ${GAME_INCLUDE_DIR}/effect/entangle.h
    ${GAME_INCLUDE_DIR}/effect/factionmodifier.h
    ${GAME_INCLUDE_DIR}/effect/forcebody.h
    ${GAME_INCLUDE_DIR}/effect/forcefizzle.h
    ${GAME_INCLUDE_DIR}/effect/forcepushed.h
    ${GAME_INCLUDE_DIR}/effect/forcepushtargeted.h
    ${GAME_INCLUDE_DIR}/effect/forceresistanceincrease.h
    ${GAME_INCLUDE_DIR}/effect/forceresisted.h
    ${GAME_INCLUDE_DIR}/effect/forceshield.h
    ${GAME_INCLUDE_DIR}/effect/forcesight.h
    ${GAME_INCLUDE_DIR}/effect/fury.h
    ${GAME_INCLUDE_DIR}/effect/heal.h
    ${GAME_INCLUDE_DIR}/effect/healforcepoints.h
    ${GAME_INCLUDE_DIR}/effect/horrified.h
    ${GAME_INCLUDE_DIR}/effect/immunity.h
    ${GAME_INCLUDE_DIR}/effect/invisibility.h
    ${GAME_INCLUDE_DIR}/effect/lightsaberthrow.h
    ${GAME_INCLUDE_DIR}/effect/linkeffects.h
    ${GAME_INCLUDE_DIR}/effect/mindtrick.h
    ${GAME_INCLUDE_DIR}/effect/modifyattacks.h
    ${GAME_INCLUDE_DIR}/effect/movementspeeddecrease.h
    ${GAME_INCLUDE_DIR}/effect/movementspeedincrease.h
    ${GAME_INCLUDE_DIR}/effect/paralyze.h
    ${GAME_INCLUDE_DIR}/effect/poison.h
    ${GAME_INCLUDE_DIR}/effect/psychicstatic.h
    ${GAME_INCLUDE_DIR}/effect/regenerate.h
    ${GAME_INCLUDE_DIR}/effect/resurrection.h
    ${GAME_INCLUDE_DIR}/effect/savingthrowdecrease.h
    ${GAME_INCLUDE_DIR}/effect/savingthrowincrease.h
    ${GAME_INCLUDE_DIR}/effect/skillincrease.h
    ${GAME_INCLUDE_DIR}/effect/sleep.h
    ${GAME_INCLUDE_DIR}/effect/stunned.h
    ${GAME_INCLUDE_DIR}/effect/temporaryforcepoints.h
    ${GAME_INCLUDE_DIR}/effect/temporaryhitpoints.h
    ${GAME_INCLUDE_DIR}/effect/trueseeing.h
    ${GAME_INCLUDE_DIR}/effect/visual.h
    ${GAME_INCLUDE_DIR}/effect/vpregenmodifier.h
    ${GAME_INCLUDE_DIR}/effect/whirlwind.h
    ${GAME_INCLUDE_DIR}/event.h
    ${GAME_INCLUDE_DIR}/footstepsounds.h
    ${GAME_INCLUDE_DIR}/format/ltrreader.h
    ${GAME_INCLUDE_DIR}/format/lytreader.h
    ${GAME_INCLUDE_DIR}/format/ssfreader.h
    ${GAME_INCLUDE_DIR}/format/ssfwriter.h
    ${GAME_INCLUDE_DIR}/format/visreader.h
    ${GAME_INCLUDE_DIR}/game.h
    ${GAME_INCLUDE_DIR}/gameinterface.h
    ${GAME_INCLUDE_DIR}/gui/console.h
    ${GAME_INCLUDE_DIR}/gui/dialog.h
    ${GAME_INCLUDE_DIR}/gui/maininterface.h
    ${GAME_INCLUDE_DIR}/gui/mainmenu.h
    ${GAME_INCLUDE_DIR}/gui/profiler.h
    ${GAME_INCLUDE_DIR}/guisounds.h
    ${GAME_INCLUDE_DIR}/layout.h
    ${GAME_INCLUDE_DIR}/layouts.h
    ${GAME_INCLUDE_DIR}/location.h
    ${GAME_INCLUDE_DIR}/object.h
    ${GAME_INCLUDE_DIR}/object/area.h
    ${GAME_INCLUDE_DIR}/object/camera.h
    ${GAME_INCLUDE_DIR}/object/creature.h
    ${GAME_INCLUDE_DIR}/object/door.h
    ${GAME_INCLUDE_DIR}/object/encounter.h
    ${GAME_INCLUDE_DIR}/object/factory.h
    ${GAME_INCLUDE_DIR}/object/item.h
    ${GAME_INCLUDE_DIR}/object/module.h
    ${GAME_INCLUDE_DIR}/object/placeable.h
    ${GAME_INCLUDE_DIR}/object/room.h
    ${GAME_INCLUDE_DIR}/object/sound.h
    ${GAME_INCLUDE_DIR}/object/store.h
    ${GAME_INCLUDE_DIR}/object/trigger.h
    ${GAME_INCLUDE_DIR}/object/waypoint.h
    ${GAME_INCLUDE_DIR}/options.h
    ${GAME_INCLUDE_DIR}/path.h
    ${GAME_INCLUDE_DIR}/paths.h
    ${GAME_INCLUDE_DIR}/portrait.h
    ${GAME_INCLUDE_DIR}/portraits.h
    ${GAME_INCLUDE_DIR}/profiler.h  
    ${GAME_INCLUDE_DIR}/resourcelayout.h
    ${GAME_INCLUDE_DIR}/selectioncontroller.h
    ${GAME_INCLUDE_DIR}/script/routine/argutil.h
    ${GAME_INCLUDE_DIR}/script/routine/context.h
    ${GAME_INCLUDE_DIR}/script/routine/declarations.h
    ${GAME_INCLUDE_DIR}/script/routine/objectutil.h
    ${GAME_INCLUDE_DIR}/script/routines.h
    ${GAME_INCLUDE_DIR}/script/runner.h
    ${GAME_INCLUDE_DIR}/services.h
    ${GAME_INCLUDE_DIR}/soundsets.h
    ${GAME_INCLUDE_DIR}/surface.h
    ${GAME_INCLUDE_DIR}/surfaces.h
    ${GAME_INCLUDE_DIR}/talent.h
    ${GAME_INCLUDE_DIR}/types.h
    ${GAME_INCLUDE_DIR}/visibilities.h)

set(GAME_SOURCES
    ${GAME_SOURCE_DIR}/action.cpp
    ${GAME_SOURCE_DIR}/action/movetoobject.cpp
    ${GAME_SOURCE_DIR}/astar.cpp
    ${GAME_SOURCE_DIR}/camerastyles.cpp
    ${GAME_SOURCE_DIR}/conversation.cpp
    ${GAME_SOURCE_DIR}/cursors.cpp
    ${GAME_SOURCE_DIR}/debug.cpp
    ${GAME_SOURCE_DIR}/footstepsounds.cpp
    ${GAME_SOURCE_DIR}/format/ltrreader.cpp
    ${GAME_SOURCE_DIR}/format/lytreader.cpp
    ${GAME_SOURCE_DIR}/format/ssfreader.cpp
    ${GAME_SOURCE_DIR}/format/ssfwriter.cpp
    ${GAME_SOURCE_DIR}/format/visreader.cpp
    ${GAME_SOURCE_DIR}/game.cpp
    ${GAME_SOURCE_DIR}/gui/console.cpp
    ${GAME_SOURCE_DIR}/gui/dialog.cpp
    ${GAME_SOURCE_DIR}/gui/maininterface.cpp
    ${GAME_SOURCE_DIR}/gui/mainmenu.cpp
    ${GAME_SOURCE_DIR}/gui/profiler.cpp
    ${GAME_SOURCE_DIR}/guisounds.cpp
    ${GAME_SOURCE_DIR}/layouts.cpp
    ${GAME_SOURCE_DIR}/object.cpp
    ${GAME_SOURCE_DIR}/object/area.cpp
    ${GAME_SOURCE_DIR}/object/camera.cpp
    ${GAME_SOURCE_DIR}/object/creature.cpp
    ${GAME_SOURCE_DIR}/object/door.cpp
    ${GAME_SOURCE_DIR}/object/encounter.cpp
    ${GAME_SOURCE_DIR}/object/item.cpp
    ${GAME_SOURCE_DIR}/object/module.cpp
    ${GAME_SOURCE_DIR}/object/placeable.cpp
    ${GAME_SOURCE_DIR}/object/room.cpp
    ${GAME_SOURCE_DIR}/object/sound.cpp
    ${GAME_SOURCE_DIR}/object/store.cpp
    ${GAME_SOURCE_DIR}/object/trigger.cpp
    ${GAME_SOURCE_DIR}/object/waypoint.cpp
    ${GAME_SOURCE_DIR}/paths.cpp
    ${GAME_SOURCE_DIR}/portraits.cpp
    ${GAME_SOURCE_DIR}/resourcelayout.cpp
    ${GAME_SOURCE_DIR}/selectioncontroller.cpp
    ${GAME_SOURCE_DIR}/script/routine/argutil.cpp
    ${GAME_SOURCE_DIR}/script/routine/impl/action.cpp
    ${GAME_SOURCE_DIR}/script/routine/impl/effect.cpp
    ${GAME_SOURCE_DIR}/script/routine/impl/main.cpp
    ${GAME_SOURCE_DIR}/script/routine/impl/minigame.cpp
    ${GAME_SOURCE_DIR}/script/routines.cpp
    ${GAME_SOURCE_DIR}/script/runner.cpp
    ${GAME_SOURCE_DIR}/soundsets.cpp
    ${GAME_SOURCE_DIR}/surfaces.cpp
    ${GAME_SOURCE_DIR}/visibilities.cpp)

add_library(game STATIC ${GAME_HEADERS} ${GAME_SOURCES} ${CLANG_FORMAT_PATH})
set_target_properties(game PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}$<$<CONFIG:Debug>:/debug>/lib)
set_target_properties(game PROPERTIES DEBUG_POSTFIX "d")
target_precompile_headers(game PRIVATE ${CMAKE_SOURCE_DIR}/src/pch.h)
target_link_libraries(game PUBLIC script gui scene movie audio graphics resource common)
