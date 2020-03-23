#include "ItemBlock.h"

#include "World.h"

ItemBlock::ItemBlock(unsigned short int quantity, GLEngine::GLTexture texture, std::string name) : Item(quantity, texture, name) {

}

void ItemBlock::onLeftClick(Tile* selectedBlock, World* world) {
    //world->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getLayer()));
}
 /// TODO: Re-enable breaking and placing blocks
void ItemBlock::onRightClick(Tile* selectedBlock, World* world) {
    if((int)selectedBlock->getID() == (int)TileIDs::AIR && m_quantity > 0) {
        //world->setTile(createBlock(m_blockId, selectedBlock->getPosition(), selectedBlock->getLayer()));
        m_quantity--;
    }
}
