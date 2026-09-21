#ifndef POKEPLATINUM_STRUCT_TRAINER_DATA_H
#define POKEPLATINUM_STRUCT_TRAINER_DATA_H

/*
 * Note: *most* source files should not include this header directly, and should
 * instead including `struct_defs/trainer.h`. This header is held separately for
 * use by data-packing routines.
 */

#include "constants/moves.h"

#define TRAINER_MON_FORM_SHIFT 10

#define MAX_TRAINER_ITEMS 4

enum TrainerDataType {
    TRDATATYPE_BASE = 0,
    TRDATATYPE_WITH_MOVES,
    TRDATATYPE_WITH_ITEM,
    TRDATATYPE_WITH_MOVES_AND_ITEM,
};

typedef struct TrainerHeader {
    u8 monDataType;
    u8 trainerType;
    u8 sprite;
    u8 partySize;
    u16 items[MAX_TRAINER_ITEMS];
    u32 aiMask;
    u32 battleType;
} TrainerHeader;

// phmode: trainer Pokemon always have perfect (31) IVs in every stat now, so the old
// ivScale field (which picked a single flat IV 0-31 for every stat, scaled from a
// 0-255 JSON value) was replaced in-place with a nature override instead - same slot,
// same u16 width, so the packed struct layout/size is unchanged. A JSON `"nature": null`
// currently resolves to a specific neutral nature (`NATURE_HARDY`) rather than a real
// "no override" sentinel - see the comment on `trainerproc.c`'s parsing of this field.
//
// `ability` is a new always-present field (like `nature`) added after it in the same
// uniform-u16 struct - it forces which of the species' two ability slots the Pokemon
// uses. `ABILITY_NONE` (0) means "no override", which resolves to the species' first
// ability slot by default; any other value has already been validated by `trainerproc.c`
// against that species' real ability list.
typedef struct TrainerMonBase {
    u16 nature;
    u16 ability;
    u16 level;
    u16 species;
    u16 cbSeal;
} TrainerMonBase;

typedef struct TrainerMonWithMoves {
    u16 nature;
    u16 ability;
    u16 level;
    u16 species;
    u16 moves[LEARNED_MOVES_MAX];
    u16 cbSeal;
} TrainerMonWithMoves;

typedef struct TrainerMonWithItem {
    u16 nature;
    u16 ability;
    u16 level;
    u16 species;
    u16 item;
    u16 cbSeal;
} TrainerMonWithItem;

typedef struct TrainerMonWithMovesAndItem {
    u16 nature;
    u16 ability;
    u16 level;
    u16 species;
    u16 item;
    u16 moves[LEARNED_MOVES_MAX];
    u16 cbSeal;
} TrainerMonWithMovesAndItem;

#endif // POKEPLATINUM_STRUCT_TRAINER_DATA_H
