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

"""Script to inspect LIP files by StrRef."""

import json
import os
import sys

extract_dir = r"D:\OpenKotOR\Extract\KotOR"

if not os.path.exists(extract_dir):
    raise RuntimeError("Extraction directory does not exist")


def _describe_shape(shape):
    phonemes = [
        "EE",
        "EH",
        "SCHWA",
        "AH",
        "OH",
        "OOH",
        "Y",
        "S/TS",
        "F/V",
        "N/NG",
        "TH",
        "M/P/B",
        "T/D",
        "J/SH",
        "L/R",
        "K/G"]

    return phonemes[shape]


def normalize_text(text):
    result = "".join(filter(lambda s: str.isalpha(s) or s == " ", text)) # strip string of everything but letters and spaces
    return result.upper()


def examine_lip(extract_dir, strref):
    tlk_path = os.path.join(extract_dir, "dialog.tlk.json")
    lips_path = os.path.join(extract_dir, "lips")
    if os.path.exists(tlk_path) and os.path.exists(lips_path):
        with open(tlk_path, "r") as tlk:
            tlk_json = json.load(tlk)
            if "strings" in tlk_json and strref < len(tlk_json["strings"]):
                string_json = tlk_json["strings"][strref]
                text = string_json["text"]
                soundresref = string_json["soundResRef"].lower()
                if soundresref:
                    lip_path = os.path.join(lips_path, soundresref + ".lip.json")
                    if os.path.exists(lip_path):
                        with open(lip_path, "r") as lip:
                            lip_json = json.load(lip)
                            print("Text (original): " + text)
                            print("Text (normalized): " + normalize_text(text))
                            print("Phonemes: " + " ".join([_describe_shape(int(x["shape"])) for x in lip_json["keyframes"]]))


if len(sys.argv) > 1:
    strref = int(sys.argv[1])
    examine_lip(extract_dir, strref)
else:
    print("Usage: python inspect_lip.py STRREF")
