#!/usr/bin/env python3

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

"""
Analyzes action calls by inspecting .pcode files in extraction directory.
"""

import glob
import os
import tkinter
import tkinter.filedialog

ACTIONS = [
    "RandomWalk",
    "MoveToLocation",
    "MoveToObject",
    "MoveAwayFromObject",
    "EquipItem",
    "UnequipItem",
    "PickUpItem",
    "PutDownItem",
    "Attack",
    "SpeakString",
    "PlayAnimation",
    "OpenDoor",
    "CloseDoor",
    "CastSpellAtObject",
    "GiveItem",
    "TakeItem",
    "ForceFollowObject",
    "JumpToObject",
    "Wait",
    "StartConversation",
    "PauseConversation",
    "ResumeConversation",
    "JumpToLocation",
    "CastSpellAtLocation",
    "SpeakStringByStrRef",
    "UseFeat",
    "UseSkill",
    "DoCommand",
    "UseTalentOnObject",
    "UseTalentAtLocation",
    "InteractObject",
    "MoveAwayFromLocation",
    "SurrenderToEnemies",
    "ForceMoveToLocation",
    "ForceMoveToObject",
    "EquipMostDamagingMelee",
    "EquipMostDamagingRanged",
    "EquipMostEffectiveArmor",
    "UnlockObject",
    "LockObject",
    "CastFakeSpellAtObject",
    "CastFakeSpellAtLocation",
    "BarkString",
    "FollowLeader",
    "FollowOwner",
    "SwitchWeapons"]


def is_valid_extract_dir(dir):
    return dir and os.path.isdir(dir)


root = tkinter.Tk()
root.withdraw()

extract_dir = os.getenv("REONE_EXTRACT_DIR")

if not is_valid_extract_dir(extract_dir):
    extract_dir = tkinter.filedialog.askdirectory(title="Choose extraction directory",
                                                  mustexist=True)
    if not is_valid_extract_dir(extract_dir):
        exit(1)

requested_module = input("Enter module name (optional): ")

calls_dict = {}

for pn in glob.glob("{}/**".format(extract_dir), recursive=True):
    _, ext = os.path.splitext(pn)
    if ext.lower() != ".pcode":
        continue
    modules_idx = pn.find("/modules/")
    if modules_idx != -1:
        ss = pn[modules_idx+9:]
        module = ss[:ss.find("/")]
        if requested_module and module.count(requested_module) == 0:
            continue
    with open(pn, "r") as f:
        lines = f.readlines()
        for line in lines:
            if not line.startswith("ACTION Action"):
                continue
            ss = line[13:]
            an = ss[:ss.find(", ")]
            if not an in calls_dict:
                calls_dict[an] = 1
            else:
                calls_dict[an] += 1


with open("actioncalls.txt", "w") as f:
    calls_list = [(an, calls_dict[an]) if an in calls_dict else (an, 0)
                  for an in ACTIONS]
    sorted_calls = sorted(calls_list, key=lambda x: x[1], reverse=True)
    lines = map(lambda x: "{} {}\n".format(x[0], x[1]), sorted_calls)
    f.writelines(lines)
