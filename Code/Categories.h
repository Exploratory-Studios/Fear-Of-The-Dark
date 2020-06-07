#pragma once

namespace Categories {

    enum class Faction {
        BENIGN,
        GOOD,
        NEUTRAL,
        BAD,
        EVIL
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
