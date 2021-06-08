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
import shutil
import sys

from reo_shared import *

game_dir = os.getenv("REONE_GAME_DIR", r"D:\Games\Star Wars - KotOR")
tools_dir = os.getenv("REONE_TOOLS_DIR", r"D:\Source\reone\build\bin\RelWithDebInfo")
extract_dir = os.getenv("REONE_EXTRACT_DIR", r"D:\OpenKotOR\Extract\KotOR")
nwnnsscomp_dir = os.getenv("REONE_NWNNSSCOMP_DIR", r"D:\OpenKotOR\Tools\DeNCS")

steps = [
    ["extract_bifs", "Extract BIF files (y/n)?", None],
    ["extract_patch", "Extract patch.erf (y/n)?", None],
    ["extract_modules", "Extract modules (y/n)?", None],
    ["extract_dialog", "Extract dialog.tlk (y/n)?", None],
    ["extract_textures", "Extract texture packs (y/n)?", None],
    ["extract_voices", "Extract streamwaves/streamvoices (y/n)?", None],
    ["extract_lips", "Extract LIP files (y/n)?", None],
    ["convert_to_json", "Convert 2DA, GFF, TLK and LIP to JSON (y/n)?", None],
    ["convert_to_tga", "Convert TPC to TGA/TXI (y/n)?", None],
    ["convert_to_ascii_pth", "Convert binary PTH to ASCII PTH (y/n)?", None],
    ["disassemble_scripts", "Disassemble NCS scripts (y/n)?", None] ]


def extract_bifs(game_dir, extract_dir):
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


def extract_patch(game_dir, extract_dir):
    # Create destination directory if it does not exist
    dest_dir = os.path.join(extract_dir, "patch")
    if not os.path.exists(dest_dir):
        os.mkdir(dest_dir)

    # Extract patch.erf
    patch_path = os.path.join(game_dir, "patch.erf")
    if os.path.exists(patch_path):
        print("Extracting patch.erf")
        run_subprocess(["reone-tools", "--extract", patch_path, "--dest", dest_dir])


def extract_modules(game_dir, extract_dir):
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


def extract_textures(game_dir, extract_dir):
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


def extract_dialog(game_dir, extract_dir):
    tlk_path = os.path.join(game_dir, "dialog.tlk")
    if os.path.exists(tlk_path):
        print("Copying {}...".format(tlk_path))
        try:
            shutil.copy(tlk_path, extract_dir)
        except PermissionError:
            pass


def extract_voices(game_dir, extract_dir):
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


def extract_lips(game_dir, extract_dir):
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


def convert_to_json(extract_dir):
    CONVERTIBLE_EXT = [
        ".2da",
        ".gui",
        ".ifo", ".are", ".git",
        ".utc", ".utd", ".ute", ".uti", ".utp", ".uts", ".utt", ".utw",
        ".dlg",
        ".tlk",
        ".lip"]

    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        _, extension = os.path.splitext(f)
        if extension in CONVERTIBLE_EXT:
            json_path = f + ".json"
            if not os.path.exists(json_path):
                print("Converting {} to JSON...".format(f))
                run_subprocess(["reone-tools", "--to-json", f])


def convert_to_tga(extract_dir):
    for f in glob.glob("{}/**/*.tpc".format(extract_dir), recursive=True):
        filename, _ = os.path.splitext(f)
        tga_path = os.path.join(os.path.dirname(f), filename + ".tga")
        if not os.path.exists(tga_path):
            print("Converting {} to TGA/TXI...".format(f))
            run_subprocess(["reone-tools", "--to-tga", f], check_retcode=False)


def convert_to_ascii_pth(extract_dir):
    for f in glob.glob("{}/**/*.pth".format(extract_dir), recursive=True):
        if not f.endswith("-ascii.pth"):
            print("Converting {} to ASCII PTH...".format(f))
            run_subprocess(["reone-tools", "--to-ascii", f])


def disassemble_scripts(extract_dir):
    for f in glob.glob("{}/**/*.ncs".format(extract_dir), recursive=True):
        filename, _ = os.path.splitext(f)
        pcode_path = os.path.join(os.path.dirname(f), filename + ".pcode")
        if not os.path.exists(pcode_path):
            print("Disassembling {}...".format(f))
            run_subprocess(["nwnnsscomp", "-d", f, "-o", pcode_path])


init_window()

if not is_valid_game_dir(game_dir):
    game_dir = choose_directory("Choose a game directory")
    if not is_valid_game_dir(game_dir):
        exit(1)

if not is_valid_tools_dir(tools_dir):
    tools_dir = choose_directory("Choose a tools directory")
    if not is_valid_tools_dir(tools_dir):
        exit(1)
append_dir_to_path(tools_dir)

if not is_valid_extract_dir(extract_dir):
    extract_dir = choose_directory("Choose an extraction directory")
    if not is_valid_extract_dir(extract_dir):
        exit(1)

assume_yes = False
if len(sys.argv) > 1 and sys.argv[1] == '-y':
    assume_yes = True

for step in steps:
    if step[2] is None:
        if not assume_yes:
            choice = input(step[1] + " ")
        run = assume_yes or choice.lower()[0] == 'y'
    else:
        run = step[2]

    if run:
        if step[0] == "extract_bifs":
            extract_bifs(game_dir, extract_dir)

        if step[0] == "extract_patch":
            extract_patch(game_dir, extract_dir)

        if step[0] == "extract_modules":
            extract_modules(game_dir, extract_dir)

        if step[0] == "extract_textures":
            extract_textures(game_dir, extract_dir)

        if step[0] == "extract_dialog":
            extract_dialog(game_dir, extract_dir)

        if step[0] == "extract_voices":
            extract_voices(game_dir, extract_dir)

        if step[0] == "extract_lips":
            extract_lips(game_dir, extract_dir)

        if step[0] == "convert_to_json":
            convert_to_json(extract_dir)

        if step[0] == "convert_to_tga":
            convert_to_tga(extract_dir)

        if step[0] == "convert_to_ascii_pth":
            convert_to_ascii_pth(extract_dir)

        if step[0] == "disassemble_scripts":
            if not is_valid_script_compiler_dir(nwnnsscomp_dir):
                nwnnsscomp_dir = choose_directory("Choose a script compiler directory")
                if not is_valid_script_compiler_dir(nwnnsscomp_dir):
                    exit(1)
                append_dir_to_path(nwnnsscomp_dir)

            disassemble_scripts(extract_dir)
