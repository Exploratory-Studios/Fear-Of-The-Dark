#pragma once

#include <glm/glm.hpp>

#include "Block.h"

/// Item class is basically abstract.
/// Create new classes for different items
/// The classes will inherit from sub-classes of this one (Block, Food, Potion, etc.) for specific functions (Place, Eat, Poison)

enum class ItemType {
    WEAPON,
    BLOCK,
    MISC
};

class Item
{
    friend class Inventory;

    public:
        //Item() {}
        Item(short unsigned int quantity, GLEngine::GLTexture texture) : m_quantity(quantity), m_texture(texture) {}
        virtual ~Item() {}

        virtual void onLeftClick(Tile* selectedBlock) {
            Block* b = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getLayer(), selectedBlock->getParentChunk());
            b->setAmbientLight(selectedBlock->getLight());
            selectedBlock->getParentChunk()->setTile(b);
        }
        virtual void onRightClick(Tile* selectedBlock) {} /// TODO variadic stuff for character pos, mouse pos, etc.  also: make virtual

        bool operator==(Item* other) {
            if(other->getID() == m_id) {
                return true;
            }
            return false;
        }

        bool isBlock() { return (m_type == ItemType::BLOCK); }
        bool isWeapon() { return (m_type == ItemType::WEAPON); }
        unsigned int getID() { return m_id; } // (unsigned int)(-1) is equivalent to null
        short unsigned int getQuantity() { return m_quantity; }
        GLEngine::GLTexture getTexture() { return m_texture; }
        // Make new classes for different types of items: blocks, weapons, food, etc. with each one having different default left & right click events

    protected:

        // There are 3 types of items: Blocks, weapons, and more to come
        ItemType m_type;

        bool m_canPlace = false; // Blocks, etc.
        bool m_canConsume = false; // Potions, food, etc.

        unsigned int m_id = (unsigned int)(-1); // Block/Non-block id
        GLEngine::GLTexture m_texture; // So we can have animated items!

        float m_weight = 0.0f; // How much it weighs in the inventory (kgs)
        short unsigned int m_quantity = 0; // How much you have

        // Metadata idea (from minecraft)
};
