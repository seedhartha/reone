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


def get_lines_from_tlk(obj, speaker):
    lines = []
    if "strings" in obj:
        uniq_sound = set()
        for string in obj["strings"]:
            if ("soundResRef" in string) and (speaker in string["soundResRef"]):
                soundresref = string["soundResRef"].lower()
                text = string["text"]
                if soundresref.startswith("n") and (not (text.startswith("[") and text.endswith("]"))) and (not soundresref in uniq_sound):
                    wav_filename = os.path.join(wav_dir, soundresref + ".wav")
                    if os.path.exists(wav_filename):
                        lines.append("{}|{}|0\n".format(wav_filename, text))
                        uniq_sound.add(soundresref)

    return lines


def generate_filelist(extract_dir, speaker):
    # Extract lines from all TLK files
    lines = []
    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        if f.endswith(".tlk.json"):
            with open(f, "r") as fp:
                obj = json.load(fp)
                lines.extend(get_lines_from_tlk(obj, speaker))

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
