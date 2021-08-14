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
TODO: support case sensitive paths (Unix)
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
extract_dir = os.getenv("REONE_EXTRACT_DIR")
nwnnsscomp_dir = os.getenv("REONE_NWNNSSCOMP_DIR")

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
    ["convert_to_ascii_pth", "Convert binary PTH to ASCII PTH (y/n)?"],
    ["disassemble_scripts", "Disassemble NCS scripts (y/n)?"]
    ]


def is_valid_game_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    for f in os.listdir(dir):
        if f == "chitin.key":
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
    tsl_exe_path = os.path.join(game_dir, "swkotor2.exe")
    game_tsl = os.path.isfile(tsl_exe_path)


def is_valid_tools_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    for f in os.listdir(dir):
        if os.path.splitext(f)[0] == "reone-tools":
            return True
    print("Tools directory does not contain a tools executable")
    return False


def append_dir_to_path(dir):
    if os.path.exists(dir) and (not dir in os.environ["PATH"]):
        separator = ":" if platform.system() == "Linux" else ";"
        os.environ["PATH"] = separator.join([os.environ["PATH"], dir])


def configure_tools_dir():
    global tools_dir
    if not is_valid_tools_dir(tools_dir):
        tools_dir = choose_directory("Choose a tools directory")
        if not is_valid_tools_dir(tools_dir):
            exit(1)
    append_dir_to_path(tools_dir)


def is_valid_extract_dir(dir):
    return dir and os.path.isdir(dir)


def configure_extract_dir():
    global extract_dir
    if not is_valid_extract_dir(extract_dir):
        extract_dir = choose_directory("Choose an extraction directory")
        if not is_valid_extract_dir(extract_dir):
            exit(1)


def is_valid_script_compiler_dir(dir):
    if not dir or not os.path.isdir(dir):
        return False
    for f in os.listdir(dir):
        if os.path.splitext(f)[0] == "nwnnsscomp":
            return True
    print("Script compiler directory does not contain a compiler executable")
    return False


def configure_script_compiler_dir():
    global nwnnsscomp_dir
    if not is_valid_script_compiler_dir(nwnnsscomp_dir):
        nwnnsscomp_dir = choose_directory("Choose a script compiler directory")
        if not is_valid_script_compiler_dir(nwnnsscomp_dir):
            exit(1)
    append_dir_to_path(nwnnsscomp_dir)


def run_subprocess(args, silent=True, check_retcode=True):
    stdout = subprocess.DEVNULL if silent else None
    process = subprocess.run(args, stdout=stdout)
    if check_retcode:
        process.check_returncode()


def extract_bifs():
    global game_dir, extract_dir

    # Create destination directory if it does not exist
    dest_dir = os.path.join(extract_dir, "data")
    if not os.path.exists(dest_dir):
        os.mkdir(dest_dir)

    # Extract all BIF files
    data_dir = os.path.join(game_dir, "data")
    if os.path.exists(data_dir):
        for f in os.listdir(data_dir):
            if f.lower().endswith(".bif"):
                print("Extracting {}...".format(f))
                bif_path = os.path.join(data_dir, f)
                run_subprocess(["reone-tools", "--game", game_dir, "--extract", bif_path, "--dest", dest_dir])


def extract_patch():
    global game_dir, extract_dir

    # Create destination directory if it does not exist
    dest_dir = os.path.join(extract_dir, "patch")
    if not os.path.exists(dest_dir):
        os.mkdir(dest_dir)

    # Extract patch.erf
    patch_path = os.path.join(game_dir, "patch.erf")
    if os.path.exists(patch_path):
        print("Extracting patch.erf")
        run_subprocess(["reone-tools", "--extract", patch_path, "--dest", dest_dir])


def extract_modules():
    global game_dir, extract_dir

    # Create destination directory if it does not exist
    dest_dir_base = os.path.join(extract_dir, "modules")
    if not os.path.exists(dest_dir_base):
        os.mkdir(dest_dir_base)

    # Extract all module RIM and ERF files
    modules_dir = os.path.join(game_dir, "modules")
    if os.path.exists(modules_dir):
        for f in os.listdir(modules_dir):
            if f.lower().endswith(".rim") or f.lower().endswith(".erf"):
                dest_dir = os.path.join(dest_dir_base, f[:-3])
                if not os.path.exists(dest_dir):
                    os.mkdir(dest_dir)
                print("Extracting {}...".format(f))
                rim_path = os.path.join(modules_dir, f)
                run_subprocess(["reone-tools", "--extract", rim_path, "--dest", dest_dir])


def extract_textures():
    global game_dir, extract_dir

    TEXTURE_PACKS = ["swpc_tex_gui.erf", "swpc_tex_tpa.erf"]

    # Create destination directory if it does not exist
    dest_dir = os.path.join(extract_dir, "data", "textures")
    if not os.path.exists(dest_dir):
        os.makedirs(dest_dir)

    # Extract textures packs
    texture_packs_dir = os.path.join(game_dir, "texturepacks")
    if os.path.exists(texture_packs_dir):
        for f in os.listdir(texture_packs_dir):
            if f in TEXTURE_PACKS:
                texture_pack_dir = os.path.join(texture_packs_dir, f)
                print("Extracting {}...".format(texture_pack_dir))
                run_subprocess(["reone-tools", "--extract", texture_pack_dir, "--dest", dest_dir])


def extract_dialog():
    global game_dir, extract_dir

    tlk_path = os.path.join(game_dir, "dialog.tlk")
    if os.path.exists(tlk_path):
        print("Copying {}...".format(tlk_path))
        try:
            shutil.copy(tlk_path, extract_dir)
        except PermissionError:
            pass


def extract_voices():
    global game_dir, extract_dir

    # Create destination directory if it does not exist
    dest_dir = os.path.join(extract_dir, "voices")
    if not os.path.exists(dest_dir):
        os.mkdir(dest_dir)

    # Extract audio files from streamwaves/streamvoice
    voices_dir = os.path.join(game_dir, "streamwaves")
    if not os.path.exists(voices_dir):
        voices_dir = os.path.join(game_dir, "streamvoice")
    if os.path.exists(voices_dir):
        for f in glob.glob("{}/**".format(voices_dir), recursive=True):
            _, extension = os.path.splitext(f)
            if extension == ".wav":
                unwrapped_path = os.path.join(dest_dir, os.path.basename(f))
                try:
                    shutil.copyfile(f, unwrapped_path)
                except PermissionError:
                    pass


def extract_lips():
    global game_dir, extract_dir

    # Create destination directory if it does not exist
    dest_dir = os.path.join(extract_dir, "lips")
    if not os.path.exists(dest_dir):
        os.mkdir(dest_dir)

    # Extract LIP files from lips
    lips_dir = os.path.join(game_dir, "lips")
    if os.path.exists(lips_dir):
        for f in os.listdir(lips_dir):
            _, extension = os.path.splitext(f)
            if extension == ".mod":
                mod_path = os.path.join(lips_dir, f)
                print("Extracting {}...".format(mod_path))
                run_subprocess(["reone-tools", "--extract", mod_path, "--dest", dest_dir])


def convert_to_json():
    global extract_dir

    CONVERTIBLE_EXT = [
        ".2da",
        ".gui",
        ".ifo", ".are", ".git",
        ".utc", ".utd", ".ute", ".uti", ".utp", ".uts", ".utt", ".utw",
        ".dlg",
        ".tlk",
        ".lip"
        ]

    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        _, extension = os.path.splitext(f)
        if extension in CONVERTIBLE_EXT:
            json_path = f + ".json"
            if not os.path.exists(json_path):
                print("Converting {} to JSON...".format(f))
                run_subprocess(["reone-tools", "--to-json", f])


def convert_to_tga():
    global extract_dir

    for f in glob.glob("{}/**/*.tpc".format(extract_dir), recursive=True):
        filename, _ = os.path.splitext(f)
        tga_path = os.path.join(os.path.dirname(f), filename + ".tga")
        if not os.path.exists(tga_path):
            print("Converting {} to TGA/TXI...".format(f))
            run_subprocess(["reone-tools", "--to-tga", f], check_retcode=False)


def convert_to_ascii_pth():
    global extract_dir

    for f in glob.glob("{}/**/*.pth".format(extract_dir), recursive=True):
        if not f.endswith("-ascii.pth"):
            print("Converting {} to ASCII PTH...".format(f))
            run_subprocess(["reone-tools", "--to-ascii", f])


def disassemble_scripts():
    global game_tsl, extract_dir

    for f in glob.glob("{}/**/*.ncs".format(extract_dir), recursive=True):
        filename, _ = os.path.splitext(f)
        pcode_path = os.path.join(os.path.dirname(f), filename + ".pcode")
        if not os.path.exists(pcode_path):
            print("Disassembling {}...".format(f))

            args = ["nwnnsscomp", "-d", f, "-o", pcode_path]
            if game_tsl:
                args.append("-g 2")

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

        if step[0] == "convert_to_ascii_pth":
            configure_tools_dir()
            convert_to_ascii_pth()

        if step[0] == "disassemble_scripts":
            configure_game_dir()
            configure_script_compiler_dir()
            disassemble_scripts()
