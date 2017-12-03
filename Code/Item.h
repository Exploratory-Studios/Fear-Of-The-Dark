#pragma once

#include <glm/glm.hpp>

#include "Categories.h"

/// Item class is basically abstract.
/// Create new classes for different items
/// The classes will inherit from sub-classes of this one (Block, Food, Potion, etc.) for specific functions (Place, Eat, Poison)

class Item
{
    friend class Inventory;

    public:
        Item(bool block, bool canPlace, bool canConsume, unsigned int id, short unsigned int weight, short unsigned int quantity) :
            m_isBlock(block), m_canPlace(canPlace), m_canConsume(canConsume), m_id(id), m_weight(weight), m_quantity(quantity)
        { }

        virtual void onLeftClick() = 0;
        virtual void onRightClick() = 0;

    private:

        // There are 2 types of items: Blocks and non-blocks (food, quest items, etc.)
        bool m_isBlock; // Block or non-block?

        bool m_canPlace; // Blocks, etc.
        bool m_canConsume; // Potions, food, etc.

        unsigned int m_id; // Block/Non-block id

        short unsigned int m_weight; // How much it weighs in the inventory (kgs)
        short unsigned int m_quantity; // How much you have
};
