#pragma once

#include <ResourceManager.h>

#include <glm/glm.hpp>
#include <vector>
#include <random>

#include "Categories.h"
#include "Logging.h"

//namespace Settings {
    /// World size variables
    // How many tiles in a chunk?
    #define CHUNK_SIZE 32 // only used in world creation.
    // How many tiles high is the world?
    #define WORLD_HEIGHT 300
    // How many tiles does the world have horizontal?
    #define WORLD_SIZE (10*32)
    #define WORLD_DEPTH 4

    // How high does water spawn naturally?
    #define WATER_LEVEL 50

    // How many chunks can you see? (Odd) (TOTAL CHUNKS DRAWN AT ONCE)
    #define VIEW_DIST 5

    // How many ticks per second?
    #define TICK_RATE 4

    #define INITIAL_ZOOM 25.0f

    #ifndef DEV_CONTROLS
    // What is the minimum scale of the camera?
    #define MIN_ZOOM 20.0f//0.001f // 15
    // What is the maximum scale of the camera?
    #define MAX_ZOOM 20.0f//15.0*12.5
    #else
    #define MIN_ZOOM 1.0f
    #define MAX_ZOOM 100.0f
    #endif // DEV_CONTROLS

    // How many boxes are in the hotbar?
    #define HOTBAR_BOX_NUM 10
    // How big is each box? (Pixels)
    #define HOTBAR_BOX_SIZE 64
    // How much space is in between each box?
    #define HOTBAR_BOX_PADDING 4

    // How many boxes are in the inventory?
    #define INVENTORY_BOX_NUM_X 10
    #define INVENTORY_BOX_NUM_Y 3
    // How big is each box?
    #define INVENTORY_BOX_SIZE 64
    // How much space is in between each box?
    #define INVENTORY_BOX_PADDING 4

    // How fast is TOO fast? (2.0 tiles/update, 120.0 tiles/sec.)
    #define MAX_SPEED 2.0f

    // How much can the player carry MAX
    #define MAX_WEIGHT 100.0f

    // How long a day is (frames at the moment)
    #define DAY_LENGTH 2400  // Should be 10 minutes per CYCLE (Full day and night) 10 minutes * 60 seconds/min. * 4 ticks/sec.
    // How much the light level is multiplied by when going through transparent tiles
    #define TRANSPARENT_LIGHT_MULTIPLIER 0.75f
    // How much the light level is multiplied by when going through blocks
    #define OPAQUE_LIGHT_MULTIPLIER 0.25f
    #define SUNLIGHT_MINUS 0.3
    /// Both those multipliers double as multipliers for heat, as well

    #define TRANSPARENT_LIGHT_MINUS 0.1
    #define OPAQUE_LIGHT_MINUS 0.4 // For heat

    // How many chunks can we see in either direction (multiply by two and add 1 for total chunks)
    #define MAX_VIEW_DIST 4

    // How many entities are we going to attempt to spawn per tick?
    #define ENTITY_SPAWN_ROLLS 50
    // How far away from the player should entities spawn?
    #define ENTITY_SPAWN_RANGE 50


    // The path to the assets folder
    #define STRX(x) std::string(#x)
    #define STR(x) STRX(x)

    #define HOME_PATH UNDEFINED_HOME_ERROR

    #ifdef WINDOWS
    #undef HOME_PATH
    #define HOME_PATH std::string((std::string)getenv("APPDATA") + "/Roaming/Exploratory Studios/Fear Of The Dark")
    #endif // WINDOWS
    #ifdef LINUX
    #undef HOME_PATH
    #define HOME_PATH std::string((std::string)getenv("HOME") + "/.exploratory/Fear Of The Dark")
    #endif // LINUX

    #define SAVES_PATH HOME_PATH + std::string("/Saves/")
    #define ASSETS_FOLDER_PATH HOME_PATH + std::string("/Assets/")

    // How long it takes (in ms) to fade in and out music
    #define FADE_TIME 30 * 1000

    //How far we can fall (in tiles) until we take damage (base)
    #define SAFE_FALL_DIST 10

    // How far away can you hear the loudest sound? (Tiles, using pythagorean theorem for dist)
    #define MAX_DIST_HEARD 30

    // How many entities can there be within a single chunk?
    #define MAX_CHUNK_ENTITIES 15
    // How often should entities spawn in each chunk? x/1000 ticks
    #define SPAWN_RATE 10

    #define MAX_LIGHTS_RENDERED 30

    #define MAX_TYPE_PARTICLES 300 // How many particles are there of a single type have at any given time?

//}

/// Category Datatypes
// Biomes (AKA Places)
class PlaceDatum {
    public:
        PlaceDatum() { }
        PlaceDatum(int baseH, float baseT, float highT, int maxHDiff, float flatnessP, std::initializer_list<unsigned int> spawnIds) : baseHeight(baseH), baseTemperature(baseT), maxTemp(highT), maxHeightDiff(maxHDiff), flatness(flatnessP)
        {
            for(auto &e : spawnIds) {
                mobSpawnIds.push_back(e);
            }
        }

        int baseHeight;
        float baseTemperature; // The **lowest** the temperature will be (noon, not including sun's effect)
        float maxTemp; // The **highest** the temperature will be (midnight)
        int maxHeightDiff;
        float flatness;
        std::vector<unsigned int> mobSpawnIds; // The ids of the mobs that will be spawned in this biome
};
// Mobs (Of entity)

enum class DayPortion {
    MORNING,
    DAY,
    AFTERNOON,
    NIGHT,
    NIGHTMARE_HOUR
};

enum class WorldEra {
    NEOLITHIC_ERA,
    COMMON_ERA,
    FUTURE_ERA
};

/*

class DropDatum {
    friend class DropTable;
    public:
        DropDatum(unsigned int level, ItemIDs itemId, unsigned int rowId, unsigned int parentId, float rarity) {
            m_level = level;
            m_itemId = itemId;
            m_rowId = rowId;
            m_rarity = rarity;
            m_parentId = parentId;
        }

        ItemIDs             getItemId() const { return m_itemId; }
        float               getRarity() const { return m_rarity; }

        ItemIDs roll()      const {
            if(m_level == 0) { // We're at the top level, quit
                return m_itemId;
            }

            float chance = (float)(std::rand() % 1000) / 1000.0f; // Gives us a chance between 0.0 and 1.0
            if(chance > m_rarity || m_rarity == 0.0f) {
                return m_parent->roll();
            } else {
                return m_itemId;
            }
        }
    private:
        unsigned int        m_level    = 0;
        ItemIDs m_itemId   = (ItemIDs)0;
        unsigned int        m_rowId    = 0;
        float               m_rarity   = 0.0f; // Inverse chance to move up in tree (0-1)
        unsigned int        m_parentId = 0;

        DropDatum*          m_parent   = nullptr;
};

class DropTable {
    public:
        DropTable(std::initializer_list<DropDatum> nodes, Categories::LootTableIds tableId) {
            std::vector<DropDatum> data;
            unsigned int deepestLevel = 0;
            for(auto& n : nodes) {
                data.push_back(n);
                if(n.m_level > deepestLevel) deepestLevel = n.m_level;
            }

            m_nodes.resize(deepestLevel+1);
            for(unsigned int i = 0; i < data.size(); i++) {
                if(data[i].m_rowId + 1 > m_nodes[data[i].m_level].size()) m_nodes[data[i].m_level].resize(data[i].m_rowId + 1, DropDatum(-1, (ItemIDs)-1, -1, -1, -1.0f));
                m_nodes[data[i].m_level][data[i].m_rowId] = data[i];
            }

            for(unsigned int i = 1; i < m_nodes.size(); i++) { // Don't want to give the base level nodes parents, since they're simply nullptrs
                for(unsigned int j = 0; j < m_nodes[i].size(); j++) {
                    m_nodes[i][j].m_parent = &m_nodes[i-1][m_nodes[i][j].m_parentId];
                }
            }

            m_tableId = tableId;
        }

        DropDatum getNode(unsigned int level, unsigned int index) {
            if(m_nodes.size() > level && m_nodes[level].size() > index) {
                DropDatum d = m_nodes[level][index];
                return d;
            } else {
                Logger::getInstance()->log("ERROR: Could not get drop in table: " + std::to_string((unsigned int)m_tableId) + " at " + std::to_string(level) + ":" + std::to_string(index), true);
            }
        }
    private:
        std::vector<std::vector<DropDatum>> m_nodes;
        Categories::LootTableIds m_tableId;
};
*/ /// TODO: Move this somewhere else. Honestly just burn anything but #DEFINEs in PresetValues.h. Please.


/// Category Data
namespace Category_Data {

    static const unsigned int TOTAL_PLACES = 8;
    static const unsigned int TOTAL_LOOT_TABLES = 1;

    static const PlaceDatum placeData[Category_Data::TOTAL_PLACES] = {
                                                                 PlaceDatum(20, -14, 25, 10, 7.5, {2}), // temperature averages in late fall-ish, as that's the kind of feel the game should have. Fall-ish...
                                                                 PlaceDatum(30, -45, -10, 15, 4.3, {2}),
                                                                 PlaceDatum(55, 10, 35, 25, 1.5, {2}),
                                                                 PlaceDatum(80, 10, 20, 30, 6.2, {2}),
                                                                 PlaceDatum(110, -15, 10, 50, 1.75, {2}),
                                                                 PlaceDatum(90, 10, 30, 40, 8.5, {2}),
                                                                 PlaceDatum(70, 10, 40, 30, 0.5, {2}),
                                                                 PlaceDatum(40, 10, 25, 2, 7.6, {2})
                                                                };

    //static DropTable lootTables[Category_Data::TOTAL_LOOT_TABLES] = {
      //                                                      DropTable({ DropDatum(0, ItemIDs::MISC_MEAT_RAW, 0, 0, 1.0f), DropDatum(1, ItemIDs::MISC_LEATHER, 0, 0, 0.5f) }, Categories::LootTableIds::ANIMAL)
      //                                                     };


};
