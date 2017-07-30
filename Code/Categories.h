#pragma once

namespace Categories {

    const int TOTAL_PLACES = 7;

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
        DIRT,
        SAND,
        STONE,
        BUSH
    };

};

class Place {
    public:
        Place() { }
        Place(int baseH, float baseT, int maxHDiff, float flatnessP) : baseHeight(baseH), baseTemperature(baseT), maxHeightDiff(maxHDiff), flatness(flatnessP) {}
        int baseHeight;
        float baseTemperature;
        int maxHeightDiff;
        float flatness;
        ///std::vector<EnemyTypes> enemySpawns;
};

namespace Category_Data {

    const Place placeData[Categories::TOTAL_PLACES+1] = {Place(10, 22, 50, 6.5),   // CANADA
                                                         Place(20, 22, 50, 6.5),   // ARCTIC
                                                         Place(20, 22, 50, 3.5),   // AUSTRALIA
                                                         Place(20, 22, 50, 3.5),   // USA
                                                         Place(50, 22, 50, 6.5),   // RUSSIA
                                                         Place(20, 22, 50, 6.5),   // N_AFRCA
                                                         Place(20, 22, 50, 3.5),   // S_AFRCA
                                                         Place(20, 22, 50, 3.5)};  // ASIA

};
