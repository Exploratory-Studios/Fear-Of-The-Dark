#pragma once

#include "PresetValues.h"

/*
    The place class should hold all data for all 'places' (comparable to biomes)

    It should also have a function to 'build' the chunk on world creation, and a function to modify the chunk (after gravity/degradation) when eras change.
*/

/// TODO: Handle travelling back in time to good/bad future choices. Ex. below
/*
    A -> B0 -> C
      -> B1 -> C

    Travelling back from C is difficult because we don't know if the player came from B0 or B1, as they both lead to C
*/

class Chunk;

class Place
{
    public:
        Place(int baseH, float baseT, float highT, int maxHDiff, float flatnessP, std::initializer_list<unsigned int> spawnIds) :
            m_baseHeight(baseH),
            m_baseTemperature(baseT),
            m_maxTemp(highT),
            m_maxHeightDiff(maxHDiff),
            m_flatness(flatnessP)
        {
            for(auto &e : spawnIds) {
                m_mobSpawnIds.push_back(e);
            }
        }

        virtual void formChunk(Chunk* parent, int blockHeights[CHUNK_SIZE]);
        virtual void ageChunk(Chunk* parent);

    protected:
        int m_baseHeight;
        float m_baseTemperature; // The **lowest** the temperature will be (noon, not including sun's effect)
        float m_maxTemp; // The **highest** the temperature will be (midnight)
        int m_maxHeightDiff;
        float m_flatness;
        std::vector<unsigned int> m_mobSpawnIds; // The ids of the mobs that will be spawned in this biome
};
