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
        TORCH,
        TORCH_BRIGHT,
        FOLIAGE,
        WOOD,
        WATER,
        WOOD_POLE
    };

    enum class ItemIDs {
        WEAPON_SWORD,
        BLOCK_DIRT,
        BLOCK_TORCH,
        BLOCK_TORCH_BRIGHT,
        BLOCK_GRASS,
        BLOCK_WOOD,
        BLOCK_WOOD_POLE,
        MISC_BUCKET,
        MISC_MEAT_RAW,
        MISC_LEATHER
    };

    enum class EntityIDs { // Should match up with presetvalues.h entities' data
        MOB_PLAYER,
        MOB_NEUTRAL_QUESTGIVER_A,
        MOB_NEUTRAL_COMPANIONCUBE,
        ITEM
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
        DIALOGUE,
        PROJECTILE,
        ITEM,
        PLAYER
    };

    enum class AI_Type {
        WALKING,
        FLYING,
        SWIMMING,
        NONE
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
        MAGIC,
        CONTACT,
        NONE
    };

    enum class LootTableIds {
        ANIMAL,
        DEMON,
        HUMANOID,
        UNIQUE,
        NONE
    };

};
