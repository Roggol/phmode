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
// same u16 width, so the packed struct layout/size is unchanged. NATURE_COUNT (one past
// the last real nature) means "no specific nature requested", matching how `item`/`moves`
// use `null` for "no override" elsewhere in this same format.
typedef struct TrainerMonBase {
    u16 nature;
    u16 level;
    u16 species;
    u16 cbSeal;
} TrainerMonBase;

typedef struct TrainerMonWithMoves {
    u16 nature;
    u16 level;
    u16 species;
    u16 moves[LEARNED_MOVES_MAX];
    u16 cbSeal;
} TrainerMonWithMoves;

typedef struct TrainerMonWithItem {
    u16 nature;
    u16 level;
    u16 species;
    u16 item;
    u16 cbSeal;
} TrainerMonWithItem;

typedef struct TrainerMonWithMovesAndItem {
    u16 nature;
    u16 level;
    u16 species;
    u16 item;
    u16 moves[LEARNED_MOVES_MAX];
    u16 cbSeal;
} TrainerMonWithMovesAndItem;

#endif // POKEPLATINUM_STRUCT_TRAINER_DATA_H
