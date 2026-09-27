#!/usr/bin/env python3
import itertools
import json
import pathlib
import sys

from convert import pad, u8, u32
from generated import species

ANSI_BOLD_WHITE = "\033[1;37m"
ANSI_BOLD_RED = "\033[1;31m"
ANSI_RED = "\033[31m"
ANSI_CLEAR = "\033[0m"

def as_species(s: str) -> bytes:
    return u32(species.Species[s].value)

def convert_land(encs: list) -> bytes:
    return b''.join(itertools.chain.from_iterable([
        (
            u32(encs[i]['level']),
            as_species(encs[i]['species']),
        )
        for i in range(12)
    ]))

def convert_water(encs: list) -> bytes:
    return b''.join(itertools.chain.from_iterable([
        (
            u8(encs[i]['level_max']),
            u8(encs[i]['level_min']),
            pad(2),
            as_species(encs[i]['species']),
        )
        for i in range(5)
    ]))

# phmode: per-location override for a water/rod table's slot odds (5 values,
# should sum to 100) - all-zero (the default when a location doesn't specify
# one) means "use this encounter method's normal hardcoded odds" instead.
def convert_water_slot_rates(rates: list) -> bytes:
    return b''.join(u8(r) for r in rates) + pad(3)


input_path = pathlib.Path(sys.argv[1])
output_path = pathlib.Path(sys.argv[2])

try:
    data = {}
    with open(input_path, 'r', encoding='utf-8') as input_file:
        data = json.load(input_file)
except json.decoder.JSONDecodeError as e:
    doc_lines = e.doc.splitlines()
    start_line = max(e.lineno - 2, 0)
    end_line = min(e.lineno + 1, len(doc_lines))

    error_lines = [f"{line_num:>4} | {line}" for line_num, line in zip(list(range(start_line + 1, end_line + 1)), doc_lines[start_line : end_line])][ : end_line - start_line]
    error_line_index = e.lineno - start_line - 1
    error_lines[error_line_index] = error_lines[error_line_index][ : 5] + f"{ANSI_RED}{error_lines[error_line_index][5 : ]}{ANSI_CLEAR}"
    error_out = "\n".join(error_lines)

    print(f"{ANSI_BOLD_WHITE}{input_path}:{e.lineno}:{e.colno}: {ANSI_BOLD_RED}error: {ANSI_BOLD_WHITE}{e.msg}{ANSI_CLEAR}\n{error_out}", file=sys.stderr)
    sys.exit(1)

packables = bytearray([])
packables.extend(u32(data['land_rate']))
packables.extend(convert_land(data['land_encounters']))

for enc_type, i in itertools.product(['day', 'night'], range(2)):
    packables.extend(as_species(data[enc_type][i]))

for key in ['rate_form0', 'rate_form1', 'rate_form2', 'rate_form3', 'rate_form4', 'unown_table']:
    packables.extend(u32(data[key]))

for version, i in itertools.product(['ruby', 'sapphire', 'emerald', 'firered', 'leafgreen'], range(2)):
    packables.extend(as_species(data[version][i]))

NO_SLOT_RATES = [0, 0, 0, 0, 0]

packables.extend(u32(data['surf_rate']))
packables.extend(convert_water_slot_rates(data.get('surf_slot_rates', NO_SLOT_RATES)))
packables.extend(convert_water(data['surf_encounters']))

# phmode: Rock Smash encounters. This used to be a padded-out "unused" water
# table; most maps still have neither key, so default to a rate of 0 (no
# encounter, ever) and 5 empty slots - the same all-zero bytes pad(44) wrote.
# Rock Smash rolls a flat uniform 20% per slot by default, same as the water/rod
# tables' hardcoded odds, unless 'rock_smash_slot_rates' overrides it (see
# GetRockSmashEncounterSlot).
NO_ROCK_SMASH_ENCOUNTERS = [{'level_min': 0, 'level_max': 0, 'species': 'SPECIES_NONE'}] * 5
packables.extend(u32(data.get('rock_smash_rate', 0)))
packables.extend(convert_water_slot_rates(data.get('rock_smash_slot_rates', NO_SLOT_RATES)))
packables.extend(convert_water(data.get('rock_smash_encounters', NO_ROCK_SMASH_ENCOUNTERS)))

for rod in ['old', 'good', 'super']:
    packables.extend(u32(data[f'{rod}_rod_rate']))
    packables.extend(convert_water_slot_rates(data.get(f'{rod}_rod_slot_rates', NO_SLOT_RATES)))
    packables.extend(convert_water(data[f'{rod}_rod_encounters']))

with open(output_path, 'wb') as output_file:
    output_file.write(packables)
