#pragma once

#include "Item.h"
#include "ItemBlock.h"

class ItemBlockGrass : public ItemBlock {
    public:
        ItemBlockGrass(unsigned int quantity);
};

class ItemBlockDirt : public ItemBlock {
    public:
        ItemBlockDirt(unsigned int quantity);
};

class ItemBlockTorch : public ItemBlock {
    public:
        ItemBlockTorch(unsigned int quantity);
};

class ItemBlockWood : public ItemBlock {
    public:
        ItemBlockWood(unsigned int quantity);
};

static Item* createItem(unsigned int id, unsigned int quantity) {
    Item* ret = nullptr;

    switch(id) {
        case (unsigned int)Categories::ItemIDs::WEAPON_SWORD: break;
        case (unsigned int)Categories::ItemIDs::BLOCK_GRASS: ret = new ItemBlockGrass(quantity); break;
        case (unsigned int)Categories::ItemIDs::BLOCK_DIRT: ret = new ItemBlockDirt(quantity); break;
        case (unsigned int)Categories::ItemIDs::BLOCK_TORCH: ret = new ItemBlockTorch(quantity); break;
        case (unsigned int)Categories::ItemIDs::BLOCK_WOOD: ret = new ItemBlockWood(quantity); break;
    }

    return ret;
}
