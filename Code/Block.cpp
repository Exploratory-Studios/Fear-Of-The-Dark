#include "Block.h"

Block::Block(glm::vec2 pos, unsigned int id) {

    m_pos = pos;
    m_id = id;

    m_solid = true; // Normally solid. Exceptions in id switch

    switch(m_id) {
        case (unsigned int)Categories::BlockIDs::AIR:
            m_solid = false;
            break;
        case (unsigned int)Categories::BlockIDs::DIRT:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Dirt.png");
            break;
        case (unsigned int)Categories::BlockIDs::SAND:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Sand.png");
            break;
        case (unsigned int)Categories::BlockIDs::STONE:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Stone.png");
            break;
        case (unsigned int)Categories::BlockIDs::BUSH:
            m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Blocks/Bush.png");
            break;
    }

}
