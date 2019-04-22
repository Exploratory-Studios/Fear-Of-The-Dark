#pragma once

#include "../Item.h"
#include "../Block.h"
#include "../Blocks.h"

class BlockItem : public Item {
    public:
        BlockItem(unsigned int id, float weight, short unsigned int quantity);

        void onLeftClick(Block* selectedBlock) override;
        void onRightClick(Block* selectedBlock) override;

    private:
};
