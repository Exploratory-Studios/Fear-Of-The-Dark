#include "ItemBlock.h"

ItemBlock::ItemBlock(unsigned int id, float weight, short unsigned int quantity, unsigned int blockId) {
    m_id = id;
    m_weight = weight;
    m_quantity = quantity;

    m_blockId = blockId;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

void ItemBlock::onLeftClick(Block* selectedBlock) {
    //*selectedBlock = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getParentChunk());
}

void ItemBlock::onRightClick(Block* selectedBlock) {
    if((int)selectedBlock->getID() == (int)Categories::BlockIDs::AIR) {
        *selectedBlock = *createBlock(m_blockId, selectedBlock->getPosition(), selectedBlock->getParentChunk());
        m_quantity--;
    }
}
