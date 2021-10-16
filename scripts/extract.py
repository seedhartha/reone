#!/usr/bin/env python3
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

"""
Script to automate resource extraction and conversion.
"""

import glob
import os
import platform
import shutil
import subprocess
import sys
import tkinter
import tkinter.filedialog


game_dir = os.getenv("REONE_GAME_DIR")
game_tsl = None
tools_dir = os.getenv("REONE_TOOLS_DIR")
tools_exe = None
extract_dir = os.getenv("REONE_EXTRACT_DIR")

steps = [
    ["extract_bifs", "Extract BIF files (y/n)?"],
    ["extract_patch", "Extract patch.erf (y/n)?"],
    ["extract_modules", "Extract modules (y/n)?"],
    ["extract_dialog", "Extract dialog.tlk (y/n)?"],
    ["extract_textures", "Extract texture packs (y/n)?"],
    ["extract_voices", "Extract streamwaves/streamvoices (y/n)?"],
    ["extract_lips", "Extract LIP files (y/n)?"],
    ["convert_to_json", "Convert 2DA, GFF, TLK and LIP to JSON (y/n)?"],
    ["convert_to_tga", "Convert TPC to TGA/TXI (y/n)?"],
    ["disassemble_scripts", "Disassemble NCS scripts (y/n)?"]
]


def find_path_ignore_case(dir, name):
    for f in os.listdir(dir):
        if f.lower() == name.lower():
            return os.path.join(dir, f)
    return None


def is_valid_game_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    if find_path_ignore_case(dir, "chitin.key") is not None:
        return True
    print("Game directory does not contain a keyfile")
    return False


def choose_directory(title):
    return tkinter.filedialog.askdirectory(title=title, mustexist=True)


def configure_game_dir():
    global game_dir, game_tsl
    if not is_valid_game_dir(game_dir):
        game_dir = choose_directory("Choose a game directory")
        if not is_valid_game_dir(game_dir):
            exit(1)
    game_tsl = find_path_ignore_case(game_dir, "swkotor2.exe") is not None


def is_valid_tools_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    if find_path_ignore_case(dir, "reone-tools") is not None:
        return True
    if find_path_ignore_case(dir, "reone-tools.exe") is not None:
        return True
    print("Tools directory does not contain a tools executable")
    return False


def configure_tools_dir():
    global tools_dir, tools_exe
    if not is_valid_tools_dir(tools_dir):
        tools_dir = choose_directory("Choose a tools directory")
        if not is_valid_tools_dir(tools_dir):
            exit(1)
    tools_exe = find_path_ignore_case(tools_dir, "reone-tools")
    if tools_exe is None:
        tools_exe = find_path_ignore_case(tools_dir, "reone-tools.exe")


def is_valid_extract_dir(dir):
    return dir and os.path.isdir(dir)


def configure_extract_dir():
    global extract_dir
    if not is_valid_extract_dir(extract_dir):
        extract_dir = choose_directory("Choose an extraction directory")
        if not is_valid_extract_dir(extract_dir):
            exit(1)


def get_or_create_dir(parent, name):
    path = os.path.join(parent, name)
    if not os.path.exists(path):
        os.mkdir(path)
    return path


def run_subprocess(args, silent=True, check_retcode=True):
    stdout = subprocess.DEVNULL if silent else None
    process = subprocess.run(args, stdout=stdout)
    if check_retcode:
        process.check_returncode()


def extract_bifs():
    global game_dir, extract_dir, tools_exe

    # Create destination directory if it does not exist
    dest_dir = get_or_create_dir(extract_dir, "data")

    # Extract all BIF files
    data_dir = find_path_ignore_case(game_dir, "data")
    if data_dir is None:
        return
    for f in os.listdir(data_dir):
        if f.lower().endswith(".bif"):
            print("Extracting {}...".format(f))
            bif_path = os.path.join(data_dir, f)
            run_subprocess([tools_exe, "--game", game_dir,
                           "--extract", bif_path, "--dest", dest_dir])


def extract_patch():
    global game_dir, extract_dir, tools_exe

    # Create destination directory if it does not exist
    dest_dir = get_or_create_dir(extract_dir, "patch")

    # Extract patch.erf
    patch_path = find_path_ignore_case(game_dir, "patch.erf")
    if patch_path is None:
        return
    print("Extracting patch.erf")
    run_subprocess([tools_exe, "--extract", patch_path, "--dest", dest_dir])


def extract_modules():
    global game_dir, extract_dir, tools_exe

    # Create destination directory if it does not exist
    dest_dir_base = get_or_create_dir(extract_dir, "modules")

    # Extract all module RIM and ERF files
    modules_dir = find_path_ignore_case(game_dir, "modules")
    if modules_dir is None:
        return
    for f in os.listdir(modules_dir):
        if f.lower().endswith(".rim") or f.lower().endswith(".erf"):
            dest_dir = get_or_create_dir(dest_dir_base, f[:-4])
            print("Extracting {}...".format(f))
            rim_path = os.path.join(modules_dir, f)
            run_subprocess(
                [tools_exe, "--extract", rim_path, "--dest", dest_dir])


def extract_textures():
    global game_dir, extract_dir, tools_exe

    TEXTURE_PACKS = ["swpc_tex_gui.erf", "swpc_tex_tpa.erf"]

    # Create destination directory if it does not exist
    data_dir = get_or_create_dir(extract_dir, "data")
    dest_dir = get_or_create_dir(data_dir, "textures")

    # Extract textures packs
    texture_packs_dir = find_path_ignore_case(game_dir, "texturepacks")
    if texture_packs_dir is None:
        return
    for f in os.listdir(texture_packs_dir):
        if f in TEXTURE_PACKS:
            texture_pack_dir = os.path.join(texture_packs_dir, f)
            print("Extracting {}...".format(texture_pack_dir))
            run_subprocess(
                [tools_exe, "--extract", texture_pack_dir, "--dest", dest_dir])


def extract_dialog():
    global game_dir, extract_dir

    tlk_path = find_path_ignore_case(game_dir, "dialog.tlk")
    if tlk_path is None:
        return
    print("Copying {}...".format(tlk_path))
    try:
        shutil.copy(tlk_path, extract_dir)
    except PermissionError:
        pass


def extract_voices():
    global game_dir, extract_dir

    # Create destination directory if it does not exist
    dest_dir = get_or_create_dir(extract_dir, "voices")

    # Extract audio files from streamwaves/streamvoice
    voices_dir = find_path_ignore_case(game_dir, "streamwaves")
    if voices_dir is None:
        voices_dir = find_path_ignore_case(game_dir, "streamvoice")
    if voices_dir is None:
        return
    for f in glob.glob("{}/**".format(voices_dir), recursive=True):
        _, extension = os.path.splitext(f)
        if extension.lower() == ".wav":
            dest_path = os.path.join(dest_dir, os.path.basename(f))
            try:
                shutil.copyfile(f, dest_path)
            except PermissionError:
                pass


def extract_lips():
    global game_dir, extract_dir, tools_exe

    # Create destination directory if it does not exist
    dest_dir = get_or_create_dir(extract_dir, "lips")

    # Extract LIP files from lips
    lips_dir = find_path_ignore_case(game_dir, "lips")
    if lips_dir is None:
        return
    for f in os.listdir(lips_dir):
        _, extension = os.path.splitext(f)
        if extension.lower() == ".mod":
            mod_path = os.path.join(lips_dir, f)
            print("Extracting {}...".format(mod_path))
            run_subprocess(
                [tools_exe, "--extract", mod_path, "--dest", dest_dir])


def is_convertible_to_json(path):
    CONVERTIBLE_EXT = [
        ".2da",
        ".gui",
        ".ifo", ".are", ".git",
        ".utc", ".utd", ".ute", ".uti", ".utp", ".uts", ".utt", ".utw",
        ".dlg",
        ".tlk",
        ".lip",
        ".pth"
    ]

    _, extension = os.path.splitext(path)
    return extension.lower() in CONVERTIBLE_EXT


def convert_to_json():
    global extract_dir, tools_exe

    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        if is_convertible_to_json(f):
            json_path = f + ".json"
            if os.path.exists(json_path):
                continue
            print("Converting {} to JSON...".format(f))
            run_subprocess([tools_exe, "--to-json", f])


def convert_to_tga():
    global extract_dir, tools_exe

    for f in glob.glob("{}/**/*.tpc".format(extract_dir), recursive=True):
        filename, _ = os.path.splitext(f)
        tga_path = os.path.join(os.path.dirname(f), filename + ".tga")
        if os.path.exists(tga_path):
            continue
        print("Converting {} to TGA/TXI...".format(f))
        run_subprocess([tools_exe, "--to-tga", f], check_retcode=False)


def disassemble_scripts():
    global game_tsl, extract_dir, tools_exe

    for f in glob.glob("{}/**/*.ncs".format(extract_dir), recursive=True):
        filename, _ = os.path.splitext(f)
        pcode_path = f + ".pcode"
        if os.path.exists(pcode_path):
            continue
        print("Disassembling {}...".format(f))

        args = [tools_exe, "--to-pcode", f]
        if game_tsl:
            args.append("--tsl=1")

        run_subprocess(args, silent=False)


root = tkinter.Tk()
root.withdraw()

configure_extract_dir()

assume_yes = len(sys.argv) > 1 and sys.argv[1] == '-y'

for step in steps:
    run = assume_yes or input(step[1] + " ").lower()[0] == 'y'

    if run:
        if step[0] == "extract_bifs":
            configure_game_dir()
            configure_tools_dir()
            extract_bifs()

        if step[0] == "extract_patch":
            configure_game_dir()
            configure_tools_dir()
            extract_patch()

        if step[0] == "extract_modules":
            configure_game_dir()
            configure_tools_dir()
            extract_modules()

        if step[0] == "extract_textures":
            configure_game_dir()
            configure_tools_dir()
            extract_textures()

        if step[0] == "extract_dialog":
            configure_game_dir()
            configure_tools_dir()
            extract_dialog()

        if step[0] == "extract_voices":
            configure_game_dir()
            configure_tools_dir()
            extract_voices()

        if step[0] == "extract_lips":
            configure_game_dir()
            configure_tools_dir()
            extract_lips()

        if step[0] == "convert_to_json":
            configure_tools_dir()
            convert_to_json()

        if step[0] == "convert_to_tga":
            configure_tools_dir()
            convert_to_tga()

        if step[0] == "disassemble_scripts":
            configure_game_dir()
            configure_tools_dir()
            disassemble_scripts()
