#pragma once

#include "Item.h"
#include "Tile.h"

class ItemBlock : public Item
{
    public:
        ItemBlock(unsigned short int quantity, GLEngine::GLTexture texture, std::string name);

        void onRightClick(Tile* selectedBlock, World* world) override;

    protected:
        unsigned int m_blockId;
};
