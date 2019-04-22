#pragma once

namespace Categories {

    enum class Places {
        CANADA,
        ARCTIC,
        AUSTRALIA,
        USA,
        RUSSIA,
        NORTH_AFRICA,
        SOUTH_AFRICA,
        ASIA
    };

    enum class BlockIDs {
        AIR,
        DIRT,
        GRASS,
        SAND,
        STONE,
        BUSH,
        TORCH
    };

    enum class ItemIDs {
        WEAPON_SWORD,
        BLOCK_DIRT,
        BLOCK_TORCH,
        BLOCK_GRASS
    };

    enum class Faction {
        BENIGN,
        GOOD,
        NEUTRAL,
        BAD,
        EVIL
    };

    enum class Entity_Type {
        MOB,
        DIALOGUE
    };

    enum class AI_Type {
        WALKING,
        FLYING,
        SWIMMING
    };

    enum class Disability_Type {
        NONE,
        BLIND,
        LAME,
        DEAF
    };

    enum class Attack_Type {
        RANGED,
        MELEE_ONLY,
        MAGIC
    };

};
