#include "ItemBlock.h"

ItemBlock::ItemBlock() {

}

void ItemBlock::onLeftClick(Block* selectedBlock) {
    *selectedBlock = *createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getParentChunk());
}

void ItemBlock::onRightClick(Block* selectedBlock) {
    if((int)selectedBlock->getID() == (int)Categories::BlockIDs::AIR && m_quantity > 0) {
        *selectedBlock = *createBlock(m_blockId, selectedBlock->getPosition(), selectedBlock->getParentChunk());
        m_quantity--;
    }
}
