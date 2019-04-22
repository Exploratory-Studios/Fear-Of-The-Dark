#pragma once

#include <ResourceManager.h>

#include <glm/glm.hpp>

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

// How many chunks can you see? (Odd) (TOTAL CHUNKS DRAWN AT ONCE)
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
//#define DAY_LENGTH 2400  // Should be 10 minutes per CYCLE 600*4
#define DAY_LENGTH 240
// How much the light level is multiplied by when going through transparent tiles
#define TRANSPARENT_LIGHT_MULTIPLIER 5.0f/7.0f
// How much the light level is multiplied by when going through blocks
#define OPAQUE_LIGHT_MULTIPLIER 1.0f/3.0f

// The path to the assets folder
#define ASSETS_FOLDER_PATH std::string("../Assets/")

// How long it takes (in ms) to fade in and out music
#define FADE_TIME 30 * 1000

//How far we can fall (in tiles) until we take damage (base)
#define SAFE_FALL_DIST 10

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
                 std::initializer_list<std::initializer_list<float>> limbData /* Format for this is as follows: { {x (0.0f-1.0f), y (0.0f-1.0f), xSize (tiles are units), ySize (tiles are units)} } */) :
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
            { }

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
};

class DialogueMobDatum {
    public:
        DialogueMobDatum(unsigned int idP, unsigned int mobIdP, unsigned int dialogueIdP) : id(idP), mobId(mobIdP), dialogueId(dialogueIdP) {}

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



/// Category Data
namespace Category_Data {

    const unsigned int TOTAL_PLACES = 7;
    const unsigned int TOTAL_MOBS = 3;
    const unsigned int TOTAL_DIALOGUE_MOBS = 1;
    const unsigned int TOTAL_ITEMS = 3;

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
                                                         MobDatum(0, 10.0f, ASSETS_FOLDER_PATH + "Textures/Mobs/Mob0.png", glm::vec2(1.0f, 2.0f), Categories::Faction::BAD, 2.608f, 0.02f, Categories::AI_Type::WALKING, Categories::Disability_Type::NONE, Categories::Attack_Type::MELEE_ONLY, { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} }),
                                                         MobDatum(1, 20.0f, ASSETS_FOLDER_PATH + "Textures/Mobs/Mob0.png", glm::vec2(1.0f, 2.0f), Categories::Faction::GOOD, 2.608f, 0.02f, Categories::AI_Type::WALKING, Categories::Disability_Type::NONE, Categories::Attack_Type::MELEE_ONLY, { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} }),
                                                         MobDatum(2, 30.0f, ASSETS_FOLDER_PATH + "Textures/Mobs/Mob0.png", glm::vec2(1.0f, 1.0f), Categories::Faction::BAD, 2.608f, 0.02f, Categories::AI_Type::SWIMMING, Categories::Disability_Type::NONE, Categories::Attack_Type::MELEE_ONLY, { {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f} })
                                                        };

    const DialogueMobDatum dialogueMobData[Category_Data::TOTAL_DIALOGUE_MOBS] = {
                                                                                  DialogueMobDatum(0, 1, 0)
                                                                                 };

    const ItemDatum itemData[Category_Data::TOTAL_ITEMS] = {
                                                            ItemDatum(0, ASSETS_FOLDER_PATH + "Textures/Items/Sword0.png", "Sword"),
                                                            ItemDatum(1, ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png", "Dirt"),
                                                            ItemDatum(2, ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png", "Torch")
                                                           };


};
