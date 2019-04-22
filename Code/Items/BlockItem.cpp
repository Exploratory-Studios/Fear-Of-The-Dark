#include "BlockItem.h"

BlockItem::BlockItem(unsigned int id, float weight, short unsigned int quantity) {
    m_isBlock = true;
    m_canPlace = true;
    m_canConsume = false;

    m_id = id;
    m_weight = weight;
    m_quantity = quantity;

}

void BlockItem::onLeftClick(Block* selectedBlock) {
    selectedBlock = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getParentChunk());
}

void BlockItem::onRightClick(Block* selectedBlock) {
    if(selectedBlock->getID() == (unsigned int)Categories::BlockIDs::AIR)
        selectedBlock = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getParentChunk());
}
