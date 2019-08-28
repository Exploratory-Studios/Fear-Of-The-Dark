#include "ItemBlock.h"

ItemBlock::ItemBlock(unsigned short int quantity, GLEngine::GLTexture texture, std::string name) : Item(quantity, texture, name) {

}

void ItemBlock::onLeftClick(Tile* selectedBlock) {
    selectedBlock->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getLayer(), selectedBlock->getParentChunk()));
}

void ItemBlock::onRightClick(Tile* selectedBlock) {
    if((int)selectedBlock->getID() == (int)Categories::BlockIDs::AIR && m_quantity > 0) {
        selectedBlock->getParentChunk()->setTile(createBlock(m_blockId, selectedBlock->getPosition(), selectedBlock->getLayer(), selectedBlock->getParentChunk()));
        m_quantity--;
    }
}
