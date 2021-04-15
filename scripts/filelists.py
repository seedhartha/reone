"""Script to generate a filelist for training a Flowtron model of a particular NPC."""

import glob
import json
import os
import random
import sys

extract_dir = r"D:\OpenKotOR\Extract\KotOR"
wav_dir = r"D:\OpenKotOR\TTS\bastila\train"


if not os.path.exists(extract_dir):
    raise RuntimeError("Extraction directory does not exist")

if not os.path.exists(wav_dir):
    raise RuntimeError("WAV directory does not exist")


def is_trainable_text(text):
    return not (text.startswith("[") and text.endswith("]"))


def index_or_negative_one(string, substr, beg=0):
    try:
        return string.index(substr, beg)
    except ValueError:
        return -1


def erase_brackets(string, left, right):
    left_idx = index_or_negative_one(string, left)
    if left_idx == -1:
        return (string, False)

    right_idx = index_or_negative_one(string, right, left_idx + 1)
    if right_idx == -1:
        return (string, False)

    return (string[:left_idx] + string[(right_idx + 1):], True)


def clear_text(text):
    while True:
        text, _ = erase_brackets(text, "[", "]")
        text, found = erase_brackets(text, "{", "}")
        if not found:
            break

    return text.strip()


def get_lines_from_dlg(obj, speaker, tlk_strings):
    lines = []
    if "EntryList|15" in obj:
        uniq_sound = set()
        for entry in obj["EntryList|15"]:
            if "VO_ResRef|11" in entry:
                voresref = entry["VO_ResRef|11"].lower()
                textstrref = int(entry["Text|12"].split("|")[0])
                if textstrref != -1:
                    text = tlk_strings[textstrref][1]
                    if voresref and (not voresref in uniq_sound) and is_trainable_text(text):
                        wav_filename = os.path.join(wav_dir, voresref + ".wav")
                        if os.path.exists(wav_filename):
                            lines.append("{}|{}|0\n".format(wav_filename, clear_text(text)))
                            uniq_sound.add(voresref)

    return lines


def generate_filelist(extract_dir, speaker):
    tlk_strings = dict()

    # Read strings from dialog.tlk into dictionary
    tlk_path = os.path.join(extract_dir, "dialog.tlk.json")
    if os.path.exists(tlk_path):
        with open(tlk_path, "r") as fp:
            obj = json.load(fp)
            if "strings" in obj:
                for string in obj["strings"]:
                    strref = int(string["_index"])
                    soundresref = string["soundResRef"].lower()
                    text = string["text"]
                    tlk_strings[strref] = (soundresref, text)

    # Extract lines from DLG files
    lines = []
    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        if f.endswith(".dlg.json"):
            with open(f, "r") as fp:
                obj = json.load(fp)
                lines.extend(get_lines_from_dlg(obj, speaker, tlk_strings))

    # Split lines into training and validation filelists
    random.shuffle(lines)
    num_val = int(5 * len(lines) / 100)
    lines_train = lines[num_val:]
    lines_val = lines[:num_val]

    with open(speaker + "_train_filelist.txt", "w") as fp:
        fp.writelines(lines_train)
    with open(speaker + "_val_filelist.txt", "w") as fp:
        fp.writelines(lines_val)


if len(sys.argv) > 1:
    generate_filelist(extract_dir, sys.argv[1])
else:
    print('Usage: python ttsfilelist.py speaker')
