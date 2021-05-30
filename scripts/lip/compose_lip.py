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

"""Script to compose LIP files from text."""

import json
import os
import platform
import subprocess

tools_dir = r"D:\Source\reone\build\bin\RelWithDebInfo"
cmudict_path = "cmudict-0.7b.txt"

if not os.path.exists(tools_dir):
    raise RuntimeError("Tools directory does not exist")

if not os.path.exists(cmudict_path):
    raise RuntimeError("CMUdict file does not exist")


def normalize_text(text):
    result = "".join(filter(lambda s: str.isalpha(s) or s == " ", text)) # strip string of everything but letters and spaces
    return result.upper()


def append_dir_to_path(dir):
    if os.path.exists(dir) and (not dir in os.environ["PATH"]):
        separator = ":" if platform.system() == "Linux" else ";"
        os.environ["PATH"] = separator.join([os.environ["PATH"], dir])


def load_cmudict():
    """
    Load words and their pronunciation from CMUdict file.
    """
    with open(cmudict_path, "r") as fp:
        lines = list(filter(lambda line: str.isalpha(line[0]), fp.readlines()))
        line_tokens = [line.split(" ") for line in lines]
        return { tokens[0] : list(map(lambda s: "".join(filter(str.isalpha, s)), tokens[2:])) for tokens in line_tokens }


def compute_cmu_phonemes(text, cmudict):
    wordsunk = set()
    phonemes = []

    for word in text.split(" "):
        if word in cmudict:
            # If word pronunciation is known, append its phonemes to the list
            phonemes.extend(cmudict[word])
        else:
            # If word pronunciation is not known, append phonemes of each individual letter to the list
            for letter in word:
                if letter in cmudict:
                    phonemes.extend(cmudict[letter])
            wordsunk.add(word)

    return (phonemes, wordsunk)


def get_lip_phoneme_shape(phoneme):
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

    return phonemes.index(phoneme)


def convert_cmu_phoneme_to_lip(phoneme):
    phonemes = {
        "AA": "AH",
        "AE": "SCHWA",
        "AH": "AH",
        "AO": "OH",
        "AW": "AH",
        "AY": "AH",
        "B": "M/P/B",
        "CH": "J/SH",
        "D": "T/D",
        "DH": "TH",
        "EH": "EH",
        "ER": "EH",
        "EY": "SCHWA",
        "F": "F/V",
        "G": "K/G",
        "HH": "K/G",
        "IH": "EE",
        "IY": "EE",
        "JH": "J/SH",
        "K": "K/G",
        "L": "L/R",
        "M": "M/P/B",
        "N": "N/NG",
        "NG": "N/NG",
        "OW": "OH",
        "OY": "OH",
        "P": "M/P/B",
        "R": "L/R",
        "S": "S/TS",
        "SH": "J/SH",
        "T": "T/D",
        "TH": "TH",
        "UH": "OOH",
        "UW": "OOH",
        "V": "F/V",
        "W": "Y",
        "Y": "Y",
        "Z": "S/TS",
        "ZH": "J/SH" }

    return phonemes[phoneme] if phoneme in phonemes else None


def write_lip_json(lip_phonemes, soundlen):
    lip_path = "output.lip.json"
    with open(lip_path, "w") as lip:
        num_phonemes = len(lip_phonemes)
        obj = {
            "length": soundlen,
            "keyframes": [{"time": round(soundlen * idx / (num_phonemes - 1), 9), "shape": get_lip_phoneme_shape(phoneme)} for idx, phoneme in enumerate(lip_phonemes)]
        }
        json.dump(obj, lip, indent=4)

    return lip_path


def run_subprocess(args, silent=True, check_retcode=True):
    stdout = subprocess.DEVNULL if silent else None
    process = subprocess.run(args, stdout=stdout)
    if check_retcode:
        process.check_returncode()


text = input("Text (original): ")
if not text:
    exit(0)

text = normalize_text(text)
print("Text (normalized): " + text)

soundlen = input("Sound length (1.0): ")
soundlen = float(soundlen) if soundlen else 1.0

append_dir_to_path(tools_dir)

cmu_phonemes, wordsunk = compute_cmu_phonemes(text, load_cmudict())
print("CMU phonemes: " + " ".join(cmu_phonemes))

if wordsunk:
    print("Unrecognized words: " + " ".join(wordsunk))

lip_phonemes = list(filter(bool, map(convert_cmu_phoneme_to_lip, cmu_phonemes)))
print("LIP phonemes: " + " ".join(lip_phonemes))

lip_path = write_lip_json(lip_phonemes, soundlen)
run_subprocess(["reone-tools", "--to-lip", lip_path])
