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

import os
import platform
import subprocess
import tkinter
import tkinter.filedialog


def is_valid_game_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    for f in os.listdir(dir):
        if f == "chitin.key":
            return True
    print("Game directory does not contain a keyfile")
    return False


def is_valid_tools_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    for f in os.listdir(dir):
        if os.path.splitext(f)[0] == "reone-tools":
            return True
    print("Tools directory does not contain a tools executable")
    return False


def is_valid_script_compiler_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    for f in os.listdir(dir):
        if os.path.splitext(f)[0] == "nwnnsscomp":
            return True
    print("Script compiler directory does not contain a compiler executable")
    return False


def is_valid_extract_dir(dir):
    return dir and os.path.isdir(dir)


def init_window():
    root = tkinter.Tk()
    root.withdraw()


def choose_directory(title):
    return tkinter.filedialog.askdirectory(title=title, mustexist=True)


def append_dir_to_path(dir):
    if os.path.exists(dir) and (not dir in os.environ["PATH"]):
        separator = ":" if platform.system() == "Linux" else ";"
        os.environ["PATH"] = separator.join([os.environ["PATH"], dir])


def run_subprocess(args, silent=True, check_retcode=True):
    stdout = subprocess.DEVNULL if silent else None
    process = subprocess.run(args, stdout=stdout)
    if check_retcode:
        process.check_returncode()
