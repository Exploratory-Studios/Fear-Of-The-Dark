#pragma once

#include <glm/glm.hpp>

#include "Block.h"



/// Item class is basically abstract.
/// Create new classes for different items
/// The classes will inherit from sub-classes of this one (Block, Food, Potion, etc.) for specific functions (Place, Eat, Poison)

class Item
{
    friend class Inventory;

    public:
        Item() {}
        Item(short unsigned int quantity) : m_quantity(quantity) {}
        virtual ~Item() {}

        virtual void onLeftClick(Tile* selectedBlock) {
            Block* b = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getParentChunk());
            b->setAmbientLight(selectedBlock->getLight());
            selectedBlock->getParentChunk()->setTile(b, selectedBlock->getPosition().x, selectedBlock->getPosition().y);
        }
        virtual void onRightClick(Tile* selectedBlock) {} /// TODO variadic stuff for character pos, mouse pos, etc.  also: make virtual

        bool isBlock() { return m_isBlock; }
        unsigned int getID() { return m_id; } // (unsigned int)(-1) is equivalent to null
        short unsigned int getQuantity() { return m_quantity; }
        // Make new classes for different types of items: blocks, weapons, food, etc. with each one having different default left & right click events

    protected:

        // There are 2 types of items: Blocks and non-blocks (food, quest items, etc.)
        bool m_isBlock = false; // Block or non-block?

        bool m_canPlace = false; // Blocks, etc.
        bool m_canConsume = false; // Potions, food, etc.

        unsigned int m_id = (unsigned int)(-1); // Block/Non-block id

        float m_weight = 0.0f; // How much it weighs in the inventory (kgs)
        short unsigned int m_quantity = 0; // How much you have

        // Metadata idea (from minecraft)
};
