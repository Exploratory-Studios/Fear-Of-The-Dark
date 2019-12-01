#pragma once

#include "Item.h"
#include "Block.h"
#include "Blocks.h"

class ItemBlock : public Item
{
    public:
        ItemBlock(unsigned short int quantity, GLEngine::GLTexture texture, std::string name);

        void onLeftClick(Tile* selectedBlock, World* world) override;
        void onRightClick(Tile* selectedBlock, World* world) override;

    protected:
        unsigned int m_blockId;
};
