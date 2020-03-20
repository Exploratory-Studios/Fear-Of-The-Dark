#include "Item.h"

#include "Blocks.h"
#include "World.h"

void Item::onLeftClick(Tile* selectedBlock, World* world) {
    Tile* b = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getLayer());
    //b->setAmbientLight(selectedBlock->getLight());
    world->setTile(b);
}
