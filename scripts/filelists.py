"""Script to generate a filelist for training a Flowtron model of a particular NPC."""

import glob
import json
import os
import random
import sys

extract_dir = r"D:\OpenKotOR\Extract\KotOR"


if not os.path.exists(extract_dir):
    raise RuntimeError("Extraction directory does not exist")


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
        text, found = erase_brackets(text, ":", ":")
        if not found:
            break

    return text.strip()


def match_entry(entry_speaker, entry_voresref, speaker, voresref):
    if entry_speaker:
        return speaker in entry_speaker

    return voresref in entry_voresref if voresref else False


def get_lines_from_dlg(obj, speaker, voresref, tlk_strings, uniq_sounds):
    global extract_dir

    wav_dir = os.path.join(extract_dir, "voices")
    if not os.path.exists(wav_dir):
        return []

    lines = []
    if "EntryList|15" in obj:
        for entry in obj["EntryList|15"]:
            if "VO_ResRef|11" in entry:
                textstrref = int(entry["Text|12"].split("|")[0])
                entry_speaker = entry["Speaker|10"].lower()
                entry_voresref = entry["VO_ResRef|11"].lower()
                if textstrref != -1 and match_entry(entry_speaker, entry_voresref, speaker, voresref) and not entry_voresref in uniq_sounds:
                    text = clear_text(tlk_strings[textstrref])
                    if text:
                        wav_filename = os.path.join(wav_dir, entry_voresref + ".wav")
                        if os.path.exists(wav_filename):
                            lines.append("{}|{}|0\n".format(wav_filename, text))
                            uniq_sounds.add(entry_voresref)

    return lines


def generate_filelist(speaker, voresref):
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
                lines.extend(get_lines_from_dlg(obj, speaker, voresref, tlk_strings, uniq_sounds))

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
    # Interpet first argument as Speaker substring, and second argument as VO_ResRef substring
    speaker = sys.argv[1]
    voresref = sys.argv[2] if len(sys.argv) > 2 else None
    generate_filelist(speaker, voresref)
else:
    print('Usage: python ttsfilelist.py speaker')
