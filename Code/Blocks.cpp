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

    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png").id;

    m_backdrop = true;
    m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png").id;

    m_natural = true;

    m_walkEffect = SoundEffectIDs::WALK_DIRT;
}

void BlockDirt::onTick(float& tickTime) {
    if(m_sunLight >= 0.5f) {
        int chance = std::rand() % 100;
        if(chance < 1) { // 1% chance every tick
            m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::GRASS, m_pos, m_parentChunk, tickTime), m_pos.x, m_pos.y);
        }
    }
}

BlockGrass::BlockGrass(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::GRASS;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Grass.png").id;

    m_backdrop = true;
    m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png").id;

    m_natural = true;
}

void BlockGrass::onTick(float& tickTime) {
    if(m_sunLight >= 0.5f) {
        int chance = std::rand() % 1000;
        if(chance < 2 && m_parentChunk->tiles[(unsigned int)m_pos.y + 1][(unsigned int)m_pos.x % CHUNK_SIZE]->getID() == (unsigned int)Categories::BlockIDs::AIR) { // 0.2% chance every tick
            m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::FOLIAGE, m_pos + glm::vec2(0.0f, 1.0f), m_parentChunk), m_pos.x, m_pos.y + 1);
        }
    }
}

BlockTorch::BlockTorch(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::TORCH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 1.5f; // Does it emit light?
    m_emittedHeat = 20.0f;

    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png").id;
}

BlockBush::BlockBush(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::BUSH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    int r = rand();
    if(r % 2 == 0) {
        m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WhiteFlower.png").id;
    } else {
        m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/RedFlower.png").id;
    }
}


BlockStone::BlockStone(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::STONE;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Stone.png").id;

    m_backdrop = true;
    m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Stone.png").id;
}

BlockFoliage::BlockFoliage(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::FOLIAGE;

    m_transparent = true;
    m_draw = true;
    m_solid = false;
    m_emittedLight = 0.0f;

    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/BushGreen.png").id;

    switch((unsigned int)parent->getPlace()) {
        case (unsigned int)Categories::Places::CANADA: {
            //m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/BushBlue.png");
            m_colour.b = 255;
            m_colour.r = 128;
            m_colour.g = 128;
            break;
        }
        case (unsigned int)Categories::Places::USA: {
            //m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/BushGreen.png");
            m_colour.b = 128;
            m_colour.r = 128;
            m_colour.g = 255;
            break;
        }
        default: {
            //m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/BushPink.png");
            m_colour.b = 128;
            m_colour.r = 255;
            m_colour.g = 128;
            break;
        }
    }
}

void BlockFoliage::onUpdate(float& time) {
    if(m_parentChunk->tiles[(unsigned int)m_pos.y-1][((unsigned int)(m_pos.x + CHUNK_SIZE) % CHUNK_SIZE)]->getID() != (unsigned int)Categories::BlockIDs::GRASS) {
        m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, m_pos, m_parentChunk), m_pos.x, m_pos.y);
    }
}

BlockWood::BlockWood(glm::vec2 pos, Chunk* parent) : Block(pos, parent) {
    m_id = (unsigned int)Categories::BlockIDs::WOOD;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png").id;

    m_backdrop = true;
    m_backdropTextureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png").id;
}
