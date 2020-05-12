#pragma once

#include <glm/glm.hpp>
#include <GLTexture.h>
#include <ResourceManager.h>
#include <string>
#include <vector>

#include "Scripting/ScriptQueue.h"

class World;
class Tile;

enum class ItemIDs {
    WEAPON_SWORD,
    BLOCK_DIRT,
    BLOCK_TORCH,
    BLOCK_TORCH_BRIGHT,
    BLOCK_TORCH_ANTI,
    BLOCK_GRASS,
    BLOCK_WOOD,
    BLOCK_WOOD_POLE,
    BLOCK_SIGN_WOOD,
    MISC_BUCKET,
    MISC_MEAT_RAW,
    MISC_LEATHER
};

class Item
{
    friend class Inventory;

    public:
        //Item() {}
        Item(short unsigned int quantity, unsigned int id, bool loadTex);
        Item(short unsigned int quantity, ItemIDs id, bool loadTex);
        virtual ~Item() {}

        void onRightClick(Tile* selectedBlock);

        std::vector<Argument> generateLuaData() {
            std::vector<Argument> args = {
                { "itemID", m_id },
                { "itemQuantity", m_quantity }
            };

            /*std::string ret = "itemID,itemQuantity=";
            ret += std::to_string(m_id) + ",";
            ret += std::to_string(m_quantity);*/

            return args;
        }

        unsigned int getID() { return m_id; } // (unsigned int)(-1) is equivalent to null
        unsigned int getTextureId() { return m_textureId; }
        short unsigned int getQuantity() { return m_quantity; }
        std::string& getName() { return m_name; }
    protected:
        void loadTexture() { m_textureId = GLEngine::ResourceManager::getTexture(m_texturePath).id; }

        float m_weight = 0.0f; // How much it weighs in the inventory (kgs)
        short unsigned int m_quantity = 0; // How much you have

        unsigned int m_id = (unsigned int)-1; // Block/Non-block id

        int m_useScriptId = -1;

        unsigned int m_textureId = (unsigned int)-1;
        std::string m_texturePath;

        std::string m_name = "UNDEFINED";
};
