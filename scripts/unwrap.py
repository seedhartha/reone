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
import platform
import shutil
import subprocess
import sys

wav_dir = r"D:\OpenKotOR\Extract\TSL\voices"
tools_dir = r"D:\Source\reone\build\bin\RelWithDebInfo"


if not os.path.exists(wav_dir):
    raise RuntimeError("WAV directory does not exist")

if not os.path.exists(tools_dir):
    raise RuntimeError("Tools directory does not exist")


def append_dir_to_path(dir):
    if os.path.exists(dir) and (not dir in os.environ["PATH"]):
        separator = ":" if platform.system() == "Linux" else ";"
        os.environ["PATH"] = separator.join([os.environ["PATH"], dir])


def run_subprocess(args, silent=True, check_retcode=True):
    stdout = subprocess.DEVNULL if silent else None
    process = subprocess.run(args, stdout=stdout)
    if check_retcode:
        process.check_returncode()


def unwrap_from_filelist(filelist):
    if os.path.exists(filelist):
        unwrap_dir = os.path.join(wav_dir, "unwrap")
        if not os.path.exists(unwrap_dir):
            os.mkdir(unwrap_dir)

        with open(filelist, "r") as fp:
            entries = [line.split("|")[0] for line in fp.readlines()]
            for f in entries:
                filename, ext = os.path.splitext(f)
                mp3_path = os.path.join(wav_dir, filename + ".mp3")
                if not os.path.exists(mp3_path):
                    print("Unwrapping {}...".format(f))
                    run_subprocess(["reone-tools", "--unwrap", f])
                    try:
                        shutil.move(mp3_path, unwrap_dir)
                    except PermissionError:
                        pass


append_dir_to_path(tools_dir)

if len(sys.argv) > 1:
    # Interpet first argument as a path to the filelist
    filelist = sys.argv[1]
    unwrap_from_filelist(filelist)
else:
    print('Usage: python ttsfilelist.py filelist')
