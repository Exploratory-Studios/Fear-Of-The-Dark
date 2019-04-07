#pragma once

#include "Item.h"
#include "Block.h"
#include "Blocks.h"

class ItemBlock : public Item
{
    public:
        ItemBlock(unsigned int id, float weight, short unsigned int quantity, unsigned int blockId);

        void onLeftClick(Block* selectedBlock);
        void onRightClick(Block* selectedBlock);

    private:
        unsigned int m_blockId;
};
