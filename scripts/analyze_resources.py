"""Script to process and analyze extracted resources."""

import glob
import json
import re
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
            with open(f, 'r') as fp:
                obj = json.load(fp)
                for value in extract_values(obj):
                    count_value(value, values)

    return values


def analyze_unique_json_values(extract_dir, path_pattern, extract_values):
    values = get_unique_json_values(extract_dir, path_pattern, extract_values)
    if values:
        print("{} unique values: {}".format(len(values), values))


def extract_values_simple(key, obj):
    values = []

    if "|" in key:
        # If key contains a vertical bar, directly access the object field
        if key in obj:
            values.append(obj[key])
    else:
        # Otherwise, iterate over all object fields
        for obj_key in obj.keys():
            if obj_key.startswith(key):
                values.append(obj[obj_key])

    return values


path_pattern = "uti.json"

# Simple

json_key = "Identified"
analyze_unique_json_values(extract_dir, path_pattern, partial(extract_values_simple, json_key))

# Nested


def extract_uti_property_names(obj):
    values = []

    if "PropertiesList|15" in obj:
        for prop in obj["PropertiesList|15"]:
            values.append(prop["PropertyName|2"])

    return values


analyze_unique_json_values(extract_dir, path_pattern, extract_uti_property_names)
