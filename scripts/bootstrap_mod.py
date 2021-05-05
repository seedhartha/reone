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

"""Script to bootstrap module from existing modules."""

import os
import shutil

extract_dir = r"D:\OpenKotOR\Extract\KotOR"
if not os.path.exists(extract_dir):
    raise RuntimeError("Extraction directory not found")

data_dir = os.path.join(extract_dir, "data")
if not os.path.exists(data_dir):
    raise RuntimeError("Data directory not found")

modules_dir = os.path.join(extract_dir, "modules")
if not os.path.exists(modules_dir):
    raise RuntimeError("Modules directory not found")

bootstrap_dir = r"D:\OpenKotOR\Mods"
if not os.path.exists(modules_dir):
    raise RuntimeError("Bootstrap directory not found")

tools_dir = r"D:\Source\reone\build\bin\RelWithDebInfo"
if not os.path.exists(modules_dir):
    raise RuntimeError("Tools directory not found")


def fetch_module_names():
    global modules_dir
    names = []
    for entry in os.listdir(modules_dir):
        module_dir = os.path.join(modules_dir, entry)
        if os.path.isdir(module_dir):
            names.append(entry.lower())

    return names


def create_build_script(module_dir, mod_name):
    bat_path = os.path.join(module_dir, "build.ps1")
    with open(bat_path, "w") as f:
        lines = [
            "$env:PATH+=\";{}\"\n".format(tools_dir),
            "reone-tools --to-gff {}\\module.ifo.json\n".format(mod_name),
            "reone-tools --to-gff {0}\\{0}.are.json\n".format(mod_name),
            "reone-tools --to-gff {0}\\{0}.git.json\n".format(mod_name),
            "reone-tools --to-mod {}".format(mod_name)
        ]
        f.writelines(lines)


def determine_actual_module_name(name):
    global modules_dir

    module_dir = os.path.join(modules_dir, name)
    for entry in os.listdir(module_dir):
        filename, ext = os.path.splitext(entry)
        if ext == ".are":
            return filename

    return name


def copy_module_files(mod_name_from, mod_name_from_actual, mod_name_to, module_dir_out, unpack_dir):
    global extract_dir
    global data_dir

    # Copy IFO, ARE and GIT
    module_dir = os.path.join(modules_dir, mod_name_from)
    ext_to_copy = [".ifo", ".are", ".git"]
    for entry in os.listdir(module_dir):
        path = os.path.join(module_dir, entry)
        filename, ext = os.path.splitext(entry)
        if ext == ".json" and list(filter(lambda x: x in filename, ext_to_copy)):
            filename = filename.replace(mod_name_from_actual, mod_name_to)
            dest = os.path.join(unpack_dir, filename + ext)
            print("Copying {} to {}...".format(path, dest))
            try:
                shutil.copy(path, dest)
            except Exception as e:
                print(e)

    # Copy PTH
    module_tpl_dir = os.path.join(modules_dir, mod_name_from + "_s")
    for entry in os.listdir(module_tpl_dir):
        path = os.path.join(module_tpl_dir, entry)
        filename, ext = os.path.splitext(entry)
        if ext == ".pth" and not "-ascii" in filename:
            filename = filename.replace(mod_name_from_actual, mod_name_to)
            dest = os.path.join(unpack_dir, filename + ext)
            print("Copying {} to {}...".format(path, dest))
            try:
                shutil.copy(path, dest)
            except Exception as e:
                print(e)

    # Copy LYT and VIS
    for entry in os.listdir(data_dir):
        path = os.path.join(data_dir, entry)
        filename, ext = os.path.splitext(entry)
        if filename == mod_name_from_actual and (ext == ".lyt" or ext == ".vis"):
            filename = filename.replace(mod_name_from_actual, mod_name_to)
            dest = os.path.join(module_dir_out, filename + ext)
            print("Copying {} to {}...".format(path, dest))
            try:
                shutil.copy(path, dest)
            except Exception as e:
                print(e)


module_names = fetch_module_names()

while True:
    mod_name_from = input("Existing module name: ")
    mod_found = mod_name_from.lower() in module_names
    if mod_found:
        break
    else:
        print("Variants:")
        for name in module_names:
            print(name)

while True:
    mod_name_to = input("New module name: ")
    if mod_name_to.lower() in module_names:
        print("Duplicate module name!")
        continue
    elif mod_name_to:
        break

print("Bootstrapping module {} from {}...".format(mod_name_to, mod_name_from))

module_dir_out = os.path.join(bootstrap_dir, mod_name_to)
if not os.path.exists(module_dir_out):
    os.mkdir(module_dir_out)

create_build_script(module_dir_out, mod_name_to)

unpack_dir = os.path.join(module_dir_out, mod_name_to)
if not os.path.exists(unpack_dir):
    os.mkdir(unpack_dir)

mod_name_from_actual = determine_actual_module_name(mod_name_from)
copy_module_files(mod_name_from, mod_name_from_actual, mod_name_to, module_dir_out, unpack_dir)
