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
        Item(unsigned int id, float weight, short unsigned int quantity) : m_id(id), m_weight(weight), m_quantity(quantity) {}
        virtual ~Item() {}

        virtual void onLeftClick(Block* selectedBlock) {}
        virtual void onRightClick(Block* selectedBlock) {} /// TODO variadic stuff for character pos, mouse pos, etc.  also: make virtua

        bool isBlock() { return m_isBlock; }
        int getID() { return m_id; } // -1 is equivalent to null
        short unsigned int getQuantity() { return m_quantity; }
        // Make new classes for different types of items: blocks, weapons, food, etc. with each one having different default left & right click events

    protected:

        // There are 2 types of items: Blocks and non-blocks (food, quest items, etc.)
        bool m_isBlock = false; // Block or non-block?

        bool m_canPlace = false; // Blocks, etc.
        bool m_canConsume = false; // Potions, food, etc.

        int m_id = -1; // Block/Non-block id

        float m_weight = 0.0f; // How much it weighs in the inventory (kgs)
        short unsigned int m_quantity = 0; // How much you have

        // Metadata idea (from minecraft)
};
