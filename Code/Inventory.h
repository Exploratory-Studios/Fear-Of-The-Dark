#pragma once

#include <vector>

#include "Item.h"

class Inventory
{
    public:
        Inventory();

        void addItem(Item newItem);

        void updateWeight();

        short unsigned int getCurrentWeight();
        float getSpeedMultiplier();

    private:
        std::vector<Item> m_items;
        short unsigned int m_weight; // The average person can carry about 40-45lbs of weight and still walk normally
        short unsigned int m_maxWeight; // The max weight at which you can still walk the normal speed
        short unsigned int m_absMaxWeight; // The weight at which you can't walk anymore (around 70-75lbs). 2x the max weight at normal walking speed
};
