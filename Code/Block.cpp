#include "Block.h"

Block::Block(glm::vec2 pos, unsigned int id, Chunk* parent) {

    m_parentChunk = parent;

    m_pos = pos;

    switchID(id);

}

void Block::switchID(unsigned int newID) {
    m_id = newID;

    m_transparent = false;
    m_solid = true;

    switch(m_id) {
        case (unsigned int)Categories::BlockIDs::DIRT:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Dirt.png");
            m_transparent = false;
            m_solid = true;
            break;
        case (unsigned int)Categories::BlockIDs::GRASS:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Grass.png");
            m_transparent = false;
            m_solid = true;
            break;
        case (unsigned int)Categories::BlockIDs::SAND:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Sand.png");
            m_transparent = false;
            m_solid = true;
            break;
        case (unsigned int)Categories::BlockIDs::STONE:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Stone.png");
            m_transparent = false;
            m_solid = true;
            break;
        case (unsigned int)Categories::BlockIDs::BUSH:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Bush.png");
            m_transparent = false;
            m_solid = true;
            break;
        case (unsigned int)Categories::BlockIDs::AIR:
            m_transparent = true;
            m_solid = false;
            break;
    }
}
