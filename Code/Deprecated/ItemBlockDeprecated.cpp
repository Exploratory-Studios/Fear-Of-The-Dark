#include "ItemBlock.h"

#include "Tile.h"
#include "World.h"

ItemBlock::ItemBlock(unsigned short int quantity, GLEngine::GLTexture texture, std::string name) : Item(quantity, texture, name) {

}

 /// TODO: Re-enable breaking and placing blocks
void ItemBlock::onRightClick(Tile* selectedBlock, World* world) {
    if((int)selectedBlock->getID() == (int)TileIDs::AIR && m_quantity > 0) {
        Tile* tile = new Tile(selectedBlock->getPosition(), selectedBlock->getLayer(), m_blockId, MetaData(), true);
        world->setTile(tile);
        m_quantity--;
    }
}
