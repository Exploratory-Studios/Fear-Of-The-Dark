#pragma once

#include "Item.h"
#include "Block.h"
#include "Blocks.h"

class ItemBlock : public Item
{
    public:
        ItemBlock();

        void onLeftClick(Block* selectedBlock) override;
        void onRightClick(Block* selectedBlock) override;

    protected:
        unsigned int m_blockId;
};
