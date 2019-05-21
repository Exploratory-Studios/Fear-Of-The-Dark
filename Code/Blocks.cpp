#include "Blocks.h"

BlockAir::BlockAir(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::AIR;

    m_transparent = true; // Does light travel through it?
    m_draw = false; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    if(parent->tiles[(unsigned int)pos.y][(unsigned int)pos.x % CHUNK_SIZE])
        m_ambientLight = parent->tiles[(unsigned int)pos.y][(unsigned int)pos.x % CHUNK_SIZE]->getLight();
}

BlockDirt::BlockDirt(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::DIRT;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png");
}

BlockGrass::BlockGrass(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::GRASS;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Grass.png");
}

BlockTorch::BlockTorch(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::TORCH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 1.5f; // Does it emit light?

    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png");
}

BlockBush::BlockBush(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::BUSH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    int r = rand();
    if(r % 2 == 0) {
        m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WhiteFlower.png");
    } else {
        m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/RedFlower.png");
    }
}


BlockStone::BlockStone(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::STONE;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Stone.png");
}
