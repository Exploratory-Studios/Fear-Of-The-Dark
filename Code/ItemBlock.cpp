#include "ItemBlock.h"

ItemBlock::ItemBlock() {

}

void ItemBlock::onLeftClick(Tile* selectedBlock) {
    selectedBlock->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getParentChunk()), selectedBlock->getPosition().x, selectedBlock->getPosition().y);
}

void ItemBlock::onRightClick(Tile* selectedBlock) {
    if((int)selectedBlock->getID() == (int)Categories::BlockIDs::AIR && m_quantity > 0) {
        selectedBlock->getParentChunk()->setTile(createBlock(m_blockId, selectedBlock->getPosition(), selectedBlock->getParentChunk()), selectedBlock->getPosition().x, selectedBlock->getPosition().y);
        m_quantity--;
    }
}
