#pragma once

#include <ResourceManager.h>

#include <glm/glm.hpp>
#include <vector>
#include <random>

#include "Categories.h"

/// TODO: Change some of these to static so that they can at least be changed in settings

/// World size variables
// How many tiles in a chunk?
#define CHUNK_SIZE 32 // 24
// How many tiles high is the world?
#define WORLD_HEIGHT 400
// How many chunks does the world have?
#define WORLD_SIZE 32
#define WORLD_DEPTH 3

// How high does water spawn naturally?
#define WATER_LEVEL 50

// How many chunks can you see? (Odd) (TOTAL CHUNKS DRAWN AT ONCE)
#define VIEW_DIST 5

// How many ticks per second?
#define TICK_RATE 4

#define INITIAL_ZOOM 20.0f

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
#define DAY_LENGTH 240  // Should be 10 minutes per CYCLE (Full day and night) 600*4
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


// The path to the assets folder
#define STRX(x) std::string(#x)
#define STR(x) STRX(x)

#define ASSETS_FOLDER_PATH std::string("../Assets/")
#ifdef CUSTOM_ASSETS_FOLDER_PATH
    #undef ASSETS_FOLDER_PATH
    #define ASSETS_FOLDER_PATH STR(CUSTOM_ASSETS_FOLDER_PATH) + "/"
#endif // CUSTOM_ASSETS_FOLDER_PATH

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

#include "Logging.h"

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

class MobDatum {
    public:
        MobDatum() { }
        MobDatum(unsigned int idP,
                 float hpP,
                 std::string texturePathP,
                 glm::vec2 sizeP,
                 Categories::Faction fac,
                 float jumpHeightP,
                 float speedP,
                 Categories::AI_Type aiType,
                 Categories::Disability_Type disType,
                 Categories::Attack_Type attType,
                 std::initializer_list<std::initializer_list<float>> limbData, /* Format for this is as follows: { {x (0.0f-1.0f), y (0.0f-1.0f), xSize (tiles are units), ySize (tiles are units)} } */
                 std::initializer_list<DayPortion> spawnTimes,
                 std::initializer_list<WorldEra> spawnEras) :
            id(idP),
            hp(hpP),
            texturePath(texturePathP),
            size(sizeP),
            faction(fac),
            jumpHeight(jumpHeightP),
            speed(speedP),
            aitype(aiType),
            disabilityType(disType),
            attackType(attType)
            {
                for(auto& e : spawnTimes) {
                    spawnTime.push_back(e);
                }
                for(auto& e : spawnEras) {
                    spawnEra.push_back(e);
                }
            }

        unsigned int id; /// This is used only for verification when copying from an array. (Kind of unused to be completely honest with ya)
                         /// Ex.
                         /// Enemy enemy(mobData[x]);
                         /// if(enemy.id != x) return ERROR_CODE;
        float hp;
        std::string texturePath;
        glm::vec2 size;
        Categories::Faction faction;
        float jumpHeight;
        float speed;
        Categories::AI_Type aitype;
        Categories::Disability_Type disabilityType;
        Categories::Attack_Type attackType;
        std::vector<DayPortion> spawnTime;
        std::vector<WorldEra> spawnEra;
};

class DialogueMobDatum {
    public:
        DialogueMobDatum(unsigned int idP, unsigned int mobIdP /* The id of the previously defined mob that this follows */, unsigned int dialogueIdP) : id(idP), mobId(mobIdP), dialogueId(dialogueIdP) {}

        unsigned int id;
        unsigned int mobId;
        unsigned int dialogueId;
};

class ItemDatum {
    public:
        ItemDatum() { }
        ItemDatum(unsigned int idP, std::string texturePathP, std::string nameP) : id(idP), texturePath(texturePathP), name(nameP) { }

        unsigned int id;
        std::string texturePath;
        std::string name;
};

class DropDatum {
    friend class DropTable;
    public:
        DropDatum(unsigned int level/*Higher numbers lower in the tree*/, Categories::ItemIDs itemId, unsigned int rowId/*Unique to this level*/, unsigned int parentId, float rarity) {
            m_level = level;
            m_itemId = itemId;
            m_rowId = rowId;
            m_rarity = rarity;
            m_parentId = parentId;
        }

        Categories::ItemIDs getItemId() const { return m_itemId; }
        float               getRarity() const { return m_rarity; }

        Categories::ItemIDs roll()      const {
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
        Categories::ItemIDs m_itemId   = (Categories::ItemIDs)0;
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
            for(int i = 0; i < data.size(); i++) {
                if(data[i].m_rowId + 1 > m_nodes[data[i].m_level].size()) m_nodes[data[i].m_level].resize(data[i].m_rowId + 1, DropDatum(-1, (Categories::ItemIDs)-1, -1, -1, -1.0f));
                m_nodes[data[i].m_level][data[i].m_rowId] = data[i];
            }

            for(int i = 1; i < m_nodes.size(); i++) { // Don't want to give the base level nodes parents, since there ARE NONE
                for(int j = 0; j < m_nodes[i].size(); j++) {
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



/// Category Data
namespace Category_Data {

    static const unsigned int TOTAL_PLACES = 8;
    static const unsigned int TOTAL_MOBS = 3;
    static const unsigned int TOTAL_DIALOGUE_MOBS = 1;
    static const unsigned int TOTAL_ITEMS = 9;
    static const unsigned int TOTAL_LOOT_TABLES = 1;

    static const PlaceDatum placeData[Category_Data::TOTAL_PLACES] = {
                                                                 PlaceDatum(20, -14, 25, 10, 7.5, {2}), // temperature averages in late fall-ish, as that's the kind of feel the game should have. Fall-ish...
                                                                 PlaceDatum(30, -45, -10, 20, 4.3, {2}),
                                                                 PlaceDatum(55, 10, 35, 45, 1.5, {2}),
                                                                 PlaceDatum(80, 10, 20, 70, 6.2, {2}),
                                                                 PlaceDatum(110, -15, 10, 100, 1.75, {2}),
                                                                 PlaceDatum(90, 10, 30, 80, 8.5, {2}),
                                                                 PlaceDatum(70, 10, 40, 60, 0.5, {2}),
                                                                 PlaceDatum(50, 10, 25, 40, 7.6, {2})
                                                                };
                                                              /*CANADA,
                                                                ARCTIC,
                                                                AUSTRALIA,
                                                                USA,
                                                                RUSSIA,
                                                                NORTH_AFRICA,
                                                                SOUTH_AFRICA,
                                                                ASIA*/

    static const MobDatum mobData[Category_Data::TOTAL_MOBS] = {
                                                         //MobDatum(0, 10.0f, ASSETS_FOLDER_PATH + "Textures/Mobs/Mob0.png", glm::vec2(1.0f, 2.0f), Categories::Faction::BAD, 2.608f, 0.02f, Categories::AI_Type::WALKING, Categories::Disability_Type::NONE, Categories::Attack_Type::MELEE_ONLY, { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} }, { }, { }),
                                                         //MobDatum(1, 20.0f, ASSETS_FOLDER_PATH + "Textures/Mobs/Mob0.png", glm::vec2(1.0f, 2.0f), Categories::Faction::GOOD, 2.608f, 0.02f, Categories::AI_Type::WALKING, Categories::Disability_Type::NONE, Categories::Attack_Type::MELEE_ONLY, { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} }, { }, { }),
                                                         //MobDatum(2, 30.0f, ASSETS_FOLDER_PATH + "Textures/Mobs/Mob0.png", glm::vec2(1.0f, 1.0f), Categories::Faction::BAD, 2.608f, 0.02f, Categories::AI_Type::WALKING, Categories::Disability_Type::NONE, Categories::Attack_Type::MELEE_ONLY, { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} }, { DayPortion::DAY }, { WorldEra::NEOLITHIC_ERA })
                                                        };

    static const DialogueMobDatum dialogueMobData[Category_Data::TOTAL_DIALOGUE_MOBS] = {
                                                                                  DialogueMobDatum(0, 1, 0)
                                                                                 };

    static const ItemDatum itemData[Category_Data::TOTAL_ITEMS] = {
                                                            ItemDatum((unsigned int)Categories::ItemIDs::WEAPON_SWORD, ASSETS_FOLDER_PATH + "Textures/Items/Sword0.png", "Sword"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::BLOCK_DIRT, ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png", "Dirt"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::BLOCK_TORCH, ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png", "Torch"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::BLOCK_GRASS, ASSETS_FOLDER_PATH + "Textures/Blocks/Grass.png", "Grass"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::BLOCK_WOOD, ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png", "Wood"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::BLOCK_WOOD_POLE, ASSETS_FOLDER_PATH + "Textures/Blocks/Wood_Pole.png", "Wood Pole"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::MISC_BUCKET, ASSETS_FOLDER_PATH + "Textures/Items/Bucket.png", "Bucket"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::MISC_MEAT_RAW, ASSETS_FOLDER_PATH + "Textures/Items/MeatRaw.png", "Raw Meat"),
                                                            ItemDatum((unsigned int)Categories::ItemIDs::MISC_LEATHER, ASSETS_FOLDER_PATH + "Textures/Items/Leather.png", "Leather")
                                                           };

    static DropTable lootTables[Category_Data::TOTAL_LOOT_TABLES] = {
                                                            DropTable({ DropDatum(0, Categories::ItemIDs::MISC_MEAT_RAW, 0, 0, 1.0f), DropDatum(1, Categories::ItemIDs::MISC_LEATHER, 0, 0, 0.5f) }, Categories::LootTableIds::ANIMAL)
                                                           };


};
