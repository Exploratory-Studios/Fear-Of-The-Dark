#pragma once

#include "Item.h"
#include "Block.h"
#include "Blocks.h"

class ItemBlock : public Item
{
    public:
        ItemBlock(unsigned short int quantity, GLEngine::GLTexture texture);

        void onLeftClick(Tile* selectedBlock) override;
        void onRightClick(Tile* selectedBlock) override;

    protected:
        unsigned int m_blockId;
};
