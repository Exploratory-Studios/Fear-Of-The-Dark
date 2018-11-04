#pragma once

#include <ResourceManager.h>

#include "Categories.h"

/// World size variables
// How many tiles in a chunk?
#define CHUNK_SIZE 64
// How many pixels to a tile?
#define TILE_SIZE 8
// How many tiles high is the world?
#define WORLD_HEIGHT 150
// How many chunks does the world have?
#define WORLD_SIZE 16

// How many chunks can you see? (Odd)
#define VIEW_DIST 3

// What is the minimum scale of the camera?
#define MIN_ZOOM 0.25
// What is the maximum scale of the camera?
#define MAX_ZOOM 7.5

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

// How fast is TOO fast? (0.44 tiles/update)
#define MAX_SPEED TILE_SIZE / 2.25f

// How much can the player carry MAX
#define MAX_WEIGHT 100.0f

// How long a day is (frames at the moment)
#define DAY_LENGTH 2400  // Should be a minute per CYCLE (4 * 60)

// How much the light level is multiplied by when going to each block
#define LIGHT_MULTIPLIER 6.0f/7.0f

/// Category Datatypes
// Biomes (AKA Places)
class PlaceDatum {
    public:
        PlaceDatum() { }
        PlaceDatum(int baseH, float baseT, int maxHDiff, float flatnessP) : baseHeight(baseH), baseTemperature(baseT), maxHeightDiff(maxHDiff), flatness(flatnessP) {}

        int baseHeight;
        float baseTemperature;
        int maxHeightDiff;
        float flatness;
        ///std::vector<Mob> enemySpawns;
};
// Mobs (Of entity)
class MobDatum {
    public:
        MobDatum() { }
        MobDatum(unsigned int idP, float hpP, std::string texturePathP, glm::vec2 sizeP, Categories::Faction fac, float jumpHeightP, float speedP) : id(idP), hp(hpP), texturePath(texturePathP), size(sizeP), faction(fac), jumpHeight(std::sqrt((jumpHeightP*TILE_SIZE+3.0f/4.0f*TILE_SIZE)/5.88f)), speed(speedP) { }

        unsigned int id; /// This is used only for verification when copying from an array.
                         /// Ex.
                         /// Enemy enemy(mobData[x]);
                         /// if(enemy.id != x) return ERROR_CODE;
        float hp;
        std::string texturePath;
        glm::vec2 size;
        Categories::Faction faction;
        float jumpHeight;
        float speed;

};

class ItemDatum {
    public:
        ItemDatum() { }
        ItemDatum(unsigned int idP, std::string texturePathP, std::string nameP) : id(idP), texturePath(texturePathP), name(nameP) { }

        unsigned int id;
        std::string texturePath;
        std::string name;
};



/// Category Data
namespace Category_Data {

    const unsigned int TOTAL_PLACES = 7;
    const unsigned int TOTAL_MOBS = 3;
    const unsigned int TOTAL_ITEMS = 1;

    const PlaceDatum placeData[Category_Data::TOTAL_PLACES+1] = {
                                                                 PlaceDatum(20, -60, 10, 7.5),
                                                                 PlaceDatum(30, -45, 20, 4.3),
                                                                 PlaceDatum(55, -30, 45, 1.5),
                                                                 PlaceDatum(80, -15, 70, 6.2),
                                                                 PlaceDatum(110, 15, 100, 1.75),
                                                                 PlaceDatum(90, 30, 80, 8.5),
                                                                 PlaceDatum(70, 45, 60, 0.5),
                                                                 PlaceDatum(50, 60, 40, 7.6)
                                                                };

    const MobDatum mobData[Category_Data::TOTAL_MOBS] = {
                                                         MobDatum(0, 10.0f, "../Assets/Textures/Mobs/Mob0.png", glm::vec2(1.0f, 2.0f), Categories::Faction::BAD, 4.0f, 0.02f),
                                                         MobDatum(1, 20.0f, "../Assets/Textures/Mobs/Mob0.png", glm::vec2(1.0f, 2.0f), Categories::Faction::GOOD, 4.0f, 0.02f),
                                                         MobDatum(2, 30.0f, "../Assets/Textures/Mobs/Mob0.png", glm::vec2(1.0f, 1.0f), Categories::Faction::BAD, 4.0f, 0.02f)
                                                        };

    const ItemDatum itemData[Category_Data::TOTAL_ITEMS] = {
                                                            ItemDatum(0, "../Assets/Textures/Items/Sword0.png", "Sword")
                                                           };

    // Why was there a random const here? Block weights and types, etc.?

};
