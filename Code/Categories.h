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

    const Place placeData[Categories::TOTAL_PLACES+1] = {Place(10, -60, 10, 7.5),
                                                         Place(20, -45, 20, 4.3),
                                                         Place(20, -30, 45, 1.5),
                                                         Place(20, -15, 70, 6.2),
                                                         Place(50, 15, 100, 1.75),
                                                         Place(20, 30, 80, 8.5),
                                                         Place(20, 45, 60, 0.5),
                                                         Place(20, 60, 40, 7.6)};

};
