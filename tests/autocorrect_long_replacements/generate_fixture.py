# SPDX-License-Identifier: GPL-2.0-or-later
"""Generate synthetic regression data without reading any dictionary file."""

import sys
from pathlib import Path


CORRECTIONS = [
    ("aaaa", "x" * 45),
    ("bbbb", "x" * 47),
    ("cccc", "x" * 48),
    ("dddd", "x" * 80),
    ("eeee", "ee" + "x" * 45),
    ("ffff", "ff" + "x" * 46),
    ("gggg:", "x" * 47),
    ("hhhh:", "x" * 48),
]


def generate_header():
    trie = {}
    for typo, correction in CORRECTIONS:
        node = trie
        for char in reversed(typo):
            keycode = 44 if char == ":" else ord(char) - ord("a") + 4
            node = node.setdefault(keycode, {})

        word = typo.strip(":")
        prefix = 0
        while prefix < min(len(word), len(correction)) and word[prefix] == correction[prefix]:
            prefix += 1
        backspaces = len(word) - prefix - 1 + typo.endswith(":")
        node["leaf"] = [128 + backspaces, *correction[prefix:].encode("ascii"), 0]

    data = []

    def serialize(node):
        offset = len(data)
        if "leaf" in node:
            data.extend(node["leaf"])
            return offset

        keys = sorted(node)
        for index, keycode in enumerate(keys):
            data.extend([keycode | (64 if index == 0 else 0), 0, 0])
        data.append(0)
        for index, keycode in enumerate(keys):
            child = serialize(node[keycode])
            data[offset + index * 3 + 1:offset + index * 3 + 3] = [child & 255, child >> 8]
        return offset

    serialize(trie)
    return "\n".join([
        "// Generated synthetic test data. No private dictionary is used.",
        "#pragma once",
        "#define AUTOCORRECT_MIN_LENGTH 4",
        "#define AUTOCORRECT_MAX_LENGTH 5",
        f"#define DICTIONARY_SIZE {len(data)}",
        "static const uint8_t autocorrect_data[DICTIONARY_SIZE] PROGMEM = {",
        "    " + ", ".join(map(str, data)),
        "};",
        "",
    ])


if __name__ == "__main__":
    Path(sys.argv[1]).write_text(generate_header())
