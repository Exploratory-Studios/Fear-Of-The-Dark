#include "ScripterMain.h"

#include <Camera2D.h>

void Scripter::init(EntityManager& entityManager, WorldIOManager& worldIOManager, GLEngine::Camera2D& camera,
                         bool scriptInitFlags /*Modify all and inherit all by default*/) {

    m_scriptInitFlags = scriptInitFlags;

    if((m_scriptInitFlags & SCRIPT_INIT_FLAG_INHERITENTITIES) == SCRIPT_INIT_FLAG_INHERITENTITIES)
        m_entities = entityManager.getEntities();
    if((m_scriptInitFlags & SCRIPT_INIT_FLAG_INHERITBLOCKS) == SCRIPT_INIT_FLAG_INHERITBLOCKS)
        m_chunks = worldIOManager.getWorld()->chunks;
    m_camera = &camera;

}

void Scripter::addBlock(Block* newBlock) {
    int chunk = std::floor(newBlock->getPosition().x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)newBlock->getPosition().x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->setTile(newBlock, chunkX, newBlock->getPosition().y); // Set the block, of course
}

void Scripter::removeBlock(int x, int y) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->tiles[y][chunkX]->m_solid = false;      // Set the block as non-solid
    m_chunks[chunk]->tiles[y][chunkX]->m_transparent = true; // Set the block as transparent
}

void Scripter::showBlock(int x, int y) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->tiles[y][chunkX]->m_transparent = false; // Make it not-transparent
}

void Scripter::hideBlock(int x, int y) {
    int chunk = std::floor(x / CHUNK_SIZE); // What chunk index it belongs to
    int chunkX = (int)x % CHUNK_SIZE;            // What x in the chunk is

    m_chunks[chunk]->tiles[y][chunkX]->m_transparent = true; // Make it transparent
}

unsigned int Scripter::addEntity(Entity newEntity) {
    m_entities.push_back(&newEntity);
}

void Scripter::removeEntity(unsigned int index) {
    m_entities[index] = nullptr;
}

void Scripter::showEntity(unsigned int index) {
    m_entities[index]->m_transparent = false;
}

void Scripter::hideEntity(unsigned int index) {
    m_entities[index]->m_transparent = true;
}

void Scripter::updateWorld() {

}

/// PRIVATE FUNCTIONS

void Scripter::draw() {
    for(int i = 0; i < m_entities.size(); i++) {
        //m_entities[i]->draw(sb);
    }

}

void Scripter::update() {

}

