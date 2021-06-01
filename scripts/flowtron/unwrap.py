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

"""Script to automate audio file unwrapping based on Flowtron filelists."""

import os
import shutil
import sys

sys.path.append("..")

from reo_shared import *

tools_dir = r"D:\Source\reone\build\bin\RelWithDebInfo"

def unwrap_from_filelist(filelist):
    if not os.path.exists(filelist):
        return
    with open(filelist, "r") as fp:
        entries = [line.split("|")[0] for line in fp.readlines()]
        for f in entries:
            filename, _ = os.path.splitext(f)
            mp3_path = filename + ".mp3"
            if not os.path.exists(mp3_path):
                print("Unwrapping {}...".format(f))
                run_subprocess(["reone-tools", "--unwrap", f])


if not is_valid_tools_dir(tools_dir):
    tools_dir = choose_directory("Choose a tools directory")
    if not is_valid_tools_dir(tools_dir):
        exit(1)
append_dir_to_path(tools_dir)

if len(sys.argv) > 1:
    # Interpet first argument as a path to the filelist
    filelist = sys.argv[1]
    unwrap_from_filelist(filelist)
else:
    print("Usage: python unwrap.py FILELIST_PATH")
