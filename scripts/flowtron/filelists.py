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

"""Script to generate a filelist for training a Flowtron model of a particular NPC."""

import glob
import json
import os
import random
import sys

sys.path.append("..")

from reo_shared import *

extract_dir = r"D:\OpenKotOR\Extract\KotOR"


def index_or_negative_one(string, substr, beg=0):
    try:
        return string.index(substr, beg)
    except ValueError:
        return -1


def erase_special_chars(string, left, right):
    left_idx = index_or_negative_one(string, left)
    if left_idx == -1:
        return (string, False)

    right_idx = index_or_negative_one(string, right, left_idx + len(left))
    if right_idx == -1:
        return (string, False)

    return (string[:left_idx] + string[(right_idx + len(right)):], True)


def clear_text(text):
    while True:
        text, found = erase_special_chars(text, "[", "]")
        if not found:
            text, found = erase_special_chars(text, "{", "}")
            if not found:
                text, found = erase_special_chars(text, "::", "::")
                if not found:
                    break

    return text.strip()


def match_entry(entry_speaker, entry_voresref, speaker, voresref):
    if entry_speaker:
        return speaker in entry_speaker

    return voresref in entry_voresref if voresref else False


def get_lines_from_dlg(obj, dlg_speaker, dlg_voresref, tlk_strings, uniq_sounds, speaker_id):
    global extract_dir

    wav_dir = os.path.join(extract_dir, "voices")
    if not os.path.exists(wav_dir):
        return []

    lines = []
    if "EntryList" in obj:
        for entry in obj["EntryList"]:
            if "VO_ResRef" in entry:
                textstrref = int(entry["Text"].split("|")[0])
                entry_speaker = entry["Speaker"].lower()
                entry_voresref = entry["VO_ResRef"].lower()
                if textstrref != -1 and match_entry(entry_speaker, entry_voresref, dlg_speaker, dlg_voresref) and not entry_voresref in uniq_sounds:
                    text = clear_text(tlk_strings[textstrref])
                    if text:
                        wav_filename = os.path.join(wav_dir, entry_voresref + ".wav")
                        if os.path.exists(wav_filename):
                            lines.append("{}|{}|{}\n".format(wav_filename, text, speaker_id))
                            uniq_sounds.add(entry_voresref)

    return lines


def generate_filelist(dlg_speaker, dlg_voresref, speaker_id):
    """
    :param speaker: substring to search for in Speaker field of dialog entries
    :param voresref: substring to search for in VO_ResRef field of dialog entries
    """
    global extract_dir

    tlk_strings = dict()
    uniq_sounds = set()

    # Read strings from dialog.tlk into dictionary
    tlk_path = os.path.join(extract_dir, "dialog.tlk.json")
    if os.path.exists(tlk_path):
        with open(tlk_path, "r") as fp:
            obj = json.load(fp)
            if "strings" in obj:
                for string in obj["strings"]:
                    strref = int(string["_index"])
                    text = string["text"]
                    tlk_strings[strref] = text

    # Extract lines from DLG files
    lines = []
    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        if f.endswith(".dlg.json"):
            with open(f, "r") as fp:
                obj = json.load(fp)
                lines.extend(get_lines_from_dlg(obj, dlg_speaker, dlg_voresref, tlk_strings, uniq_sounds, speaker_id))

    # Split lines into training and validation filelists
    random.shuffle(lines)
    num_val = int(5 * len(lines) / 100)
    lines_train = lines[num_val:]
    lines_val = lines[:num_val]

    with open(dlg_speaker + "_train_filelist.txt", "w") as fp:
        fp.writelines(lines_train)
    with open(dlg_speaker + "_val_filelist.txt", "w") as fp:
        fp.writelines(lines_val)


init_window()

if not is_valid_extract_dir(extract_dir):
    extract_dir = choose_directory("Choose an extraction directory")
    if not is_valid_extract_dir(extract_dir):
        exit(1)

if len(sys.argv) > 1:
    dlg_speaker = sys.argv[1] # DLG Speaker substring
    dlg_voresref = sys.argv[2] if len(sys.argv) > 2 else None # DLG VO_ResRef substring
    speaker_id = sys.argv[3] if len(sys.argv) > 3 else None # Flowtron Speaker ID
    generate_filelist(dlg_speaker, dlg_voresref, speaker_id)
else:
    print("Usage: python filelist.py DLG_SPEAKER [DLG_VORESREF] [SPEAKER_ID]")
