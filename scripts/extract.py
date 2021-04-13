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

game_dir = r"D:\Games\Star Wars - KotOR"
tools_dir = r"D:\Source\reone\build\bin\RelWithDebInfo"
extract_dir = r"D:\OpenKotOR\Extract\KotOR"
nwnnsscomp_dir = r"D:\OpenKotOR\Tools\DeNCS"

steps = {
    "extract_bifs": True,
    "extract_patch": True,
    "extract_modules": True,
    "extract_dialog": True,
    "extract_textures": True,
    "convert_to_json": False,
    "convert_to_tga": False,
    "convert_to_ascii_pth": False,
    "disassemble_scripts": False }


if not os.path.exists(game_dir):
    raise RuntimeError("Game directory does not exist")

if not os.path.exists(tools_dir):
    raise RuntimeError("Tools directory does not exist")

if not os.path.exists(extract_dir):
    raise RuntimeError("Extraction directory does not exist")


def append_dir_to_path(dir):
    if os.path.exists(dir) and (not dir in os.environ["PATH"]):
        separator = ":" if platform.system() == "Linux" else ";"
        os.environ["PATH"] = separator.join([os.environ["PATH"], dir])


def run_subprocess(args, silent=True, check_retcode=True):
    stdout = subprocess.DEVNULL if silent else None
    process = subprocess.run(args, stdout=stdout)
    if check_retcode:
        process.check_returncode()


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

def convert_to_json(extract_dir):
    CONVERTIBLE_EXT = [
        ".2da",
        ".gui",
        ".ifo", ".are", ".git", ".pth",
        ".utc", ".utd", ".ute", ".uti", ".utp", ".uts", ".utt", ".utw",
        ".dlg",
        ".tlk"]

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


append_dir_to_path(tools_dir)
append_dir_to_path(nwnnsscomp_dir)

if steps["extract_bifs"]:
    extract_bifs(game_dir, extract_dir)

if steps["extract_patch"]:
    extract_patch(game_dir, extract_dir)

if steps["extract_modules"]:
    extract_modules(game_dir, extract_dir)

if steps["extract_textures"]:
    extract_textures(game_dir, extract_dir)

if steps["extract_dialog"]:
    extract_dialog(game_dir, extract_dir)

if steps["convert_to_json"]:
    convert_to_json(extract_dir)

if steps["convert_to_tga"]:
    convert_to_tga(extract_dir)

if steps["convert_to_ascii_pth"]:
    convert_to_ascii_pth(extract_dir)

if steps["disassemble_scripts"]:
    disassemble_scripts(extract_dir)
