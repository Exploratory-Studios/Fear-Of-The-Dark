#pragma once

#include <glm/glm.hpp>

#include "Categories.h"
#include "Block.h"

/// Item class is basically abstract.
/// Create new classes for different items
/// The classes will inherit from sub-classes of this one (Block, Food, Potion, etc.) for specific functions (Place, Eat, Poison)

class Item
{
    friend class Inventory;

    public:
        Item() {}
        Item(unsigned int id, float weight, short unsigned int quantity);

        virtual void onLeftClick(Block* selectedBlock) {};
        virtual void onRightClick(Block* selectedBlock) {}; /// TODO variadic stuff for character pos, mouse pos, etc.

        bool isBlock() { return m_isBlock; }
        unsigned int getID() { return m_id; }
        // Make new classes for different types of items: blocks, weapons, food, etc. with each one having different default left & right click events

    protected:

        // There are 2 types of items: Blocks and non-blocks (food, quest items, etc.)
        bool m_isBlock; // Block or non-block?

        bool m_canPlace; // Blocks, etc.
        bool m_canConsume; // Potions, food, etc.

        unsigned int m_id; // Block/Non-block id

        float m_weight; // How much it weighs in the inventory (kgs)
        short unsigned int m_quantity; // How much you have

        // Metadata idea (from minecraft)
};
