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

"""Script to count unique JSON values in extracted resources."""

import glob
import json
import re
import sys
from functools import partial

extract_dir = r"D:\OpenKotOR\Extract\KotOR"

def get_unique_json_values(extract_dir, path_pattern, extract_values):

    def count_value(value, values):
        if value in values:
            values[value] += 1
        else:
            values[value] = 1

    values = dict()

    # Recursively process files in extraction directory, whose path matches a pattern
    for f in glob.glob("{}/**".format(extract_dir), recursive=True):
        if re.search(path_pattern, f):
            with open(f, "r") as fp:
                obj = json.load(fp)
                for value in extract_values(obj):
                    count_value(value, values)

    return values


def count_unique_json_values(extract_dir, path_pattern, extract_values):
    values = get_unique_json_values(extract_dir, path_pattern, extract_values)
    if values:
        print("{} unique values: {}".format(len(values), values))


def extract_values_simple(key, obj):
    return [obj[key]] if key in obj else []


if len(sys.argv) > 2:
    # If at least two command line arguments were passed, interpret them as
    # filepath pattern and JSON key. In this mode we count unique field values
    # in JSON files.
    path_pattern = sys.argv[1]
    json_key = sys.argv[2]
    count_unique_json_values(extract_dir, path_pattern, partial(extract_values_simple, json_key))
else:
    print("Usage: python count_json_values.py PATH_PATTERN JSON_KEY")
