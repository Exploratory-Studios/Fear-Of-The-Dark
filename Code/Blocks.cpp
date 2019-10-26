#include "Blocks.h"

BlockAir::BlockAir(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::AIR;

    m_transparent = true; // Does light travel through it?
    m_draw = false; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

    //if(parent->getTile((int)pos.x, pos.y, 0))
        //m_ambientLight = parent->getTile(pos.x, (int)pos.y, 0)->getLight();
}

BlockDirt::BlockDirt(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::DIRT;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = true;

    m_natural = true;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

    m_walkEffect = SoundEffectIDs::WALK_DIRT;
}

void BlockDirt::onTick(float& tickTime) {
    if(m_sunLight >= 0.5f && m_exposedToSun) {
        int chance = std::rand() % 100;
        if(chance < 1) { // 1% chance every tick
            m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::GRASS, m_pos, m_layer, m_parentChunk, MetaData(), tickTime));
        }
    }
}

BlockGrass::BlockGrass(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::GRASS;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = true;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

    m_natural = true;
}

void BlockGrass::onTick(float& tickTime) {
    if(m_sunLight >= 0.5f && m_exposedToSun) {
        int chance = std::rand() % 1000;
        if(chance < 2 && m_parentChunk->getTile(m_pos.x, m_pos.y + 1, m_layer)->getID() == (unsigned int)Categories::BlockIDs::AIR) { // 0.2% chance every tick
            //m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::FOLIAGE, m_pos + glm::vec2(0.0f, 1.0f), m_parentChunk), m_pos.x, m_pos.y + 1);
        }
    }
}

BlockTorch::BlockTorch(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::TORCH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 1.5f; // Does it emit light?
    m_emittedHeat = 20.0f;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}

BlockTorchBright::BlockTorchBright(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture) : BlockTorch(pos, layer, parent, loadTexture) {
    m_emittedLight = 2.0f;
}

BlockTorchAnti::BlockTorchAnti(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture) : BlockTorch(pos, layer, parent, loadTexture) {
    m_emittedLight = -1.0f;
}

BlockBush::BlockBush(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::BUSH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}


BlockStone::BlockStone(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::STONE;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = true;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}

BlockFoliage::BlockFoliage(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::FOLIAGE;

    m_transparent = true;
    m_draw = true;
    m_solid = false;
    m_emittedLight = 0.0f;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

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
    if(m_parentChunk->getTile(m_pos.x, m_pos.y - 1, m_layer)->getID() != (unsigned int)Categories::BlockIDs::GRASS) {
        m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, m_pos, m_layer, m_parentChunk));
    }
}

BlockWood::BlockWood(glm::vec2 pos, unsigned int layer, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, MetaData(), loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::WOOD;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = true;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}

BlockWater::BlockWater(glm::vec2 pos, unsigned int layer, Chunk* parent, float level, MetaData metaData,/* = MetaData*/ bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, metaData, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::WATER;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = false;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

    if(!metaData.getAspect("level", m_waterLevel)) {
        m_waterLevel = level;
    }
}

void BlockWater::draw(GLEngine::SpriteBatch& sb, int xOffset, int depthDifference) {
    if(m_textureId == (GLuint)-1) {
        loadTexture();
    }

    GLEngine::ColourRGBA8 colour = m_colour;
    if(depthDifference > 0) {
        colour.a = 0;
    }


    sb.draw(glm::vec4(m_pos.x + xOffset * CHUNK_SIZE, m_pos.y, m_size.x, m_waterLevel),
            glm::vec4(0.0f, 0.0f, 1.0f, m_waterLevel),
            m_textureId,
            2.0f,
            colour,
            glm::vec3(getLight()));
}
#include <iostream>
void BlockWater::onUpdate(float& time) {
    // 'Spread' water to other blocks, if they are passable blocks


    if(m_waterLevel > 0.0f) {
        Tile* left = nullptr;
        Tile* right = nullptr;
        Tile* down = nullptr;

        left = m_parentChunk->getTile(m_pos.x - 1, m_pos.y, m_layer); /// TODO: Cross-layer water movement

        right = m_parentChunk->getTile(m_pos.x + 1, m_pos.y, m_layer);

        down = m_parentChunk->getTile(m_pos.x, m_pos.y - 1, m_layer);

        // First, get levels of surrounding water blocks, set each to average
        {
            float averageLevel = 0.0f;
            unsigned int divisor = 0;

            bool fullySupported = true; // Does this water have full water on the right, left, and underneath?

            if(left->getID() == (unsigned int)Categories::BlockIDs::WATER) {
                if(reinterpret_cast<BlockWater*>(left)->getLevel() < m_waterLevel) {
                    divisor++;
                    averageLevel += reinterpret_cast<BlockWater*>(left)->getLevel();
                    fullySupported = false;
                }
            }
            if(right->getID() == (unsigned int)Categories::BlockIDs::WATER) {
                if(reinterpret_cast<BlockWater*>(right)->getLevel() < m_waterLevel) {
                    divisor++;
                    averageLevel += reinterpret_cast<BlockWater*>(right)->getLevel();
                    fullySupported = false;
                }
            }

            averageLevel += m_waterLevel;
            averageLevel /= (divisor + 1);

            if(down->getID() == (unsigned int)Categories::BlockIDs::WATER && reinterpret_cast<BlockWater*>(down)->getLevel() < 1.0f) {
                fullySupported = false;

                float newLevel = reinterpret_cast<BlockWater*>(down)->getLevel() + m_waterLevel;

                if(newLevel > 1.0f) {
                    float lev = 1.0f;
                    reinterpret_cast<BlockWater*>(down)->setLevel(lev);
                    newLevel -= 1.0f;
                    m_waterLevel = newLevel;
                } else {
                    reinterpret_cast<BlockWater*>(down)->setLevel(newLevel);
                    m_waterLevel = 0.0f;
                }

            } else {
                m_waterLevel = averageLevel;
                if(left->getID() == (unsigned int)Categories::BlockIDs::WATER) {
                    if(reinterpret_cast<BlockWater*>(left)->getLevel() < m_waterLevel) {
                        float newLevel = averageLevel;
                        reinterpret_cast<BlockWater*>(left)->setLevel(newLevel);
                    }
                }
                if(right->getID() == (unsigned int)Categories::BlockIDs::WATER) {
                    if(reinterpret_cast<BlockWater*>(right)->getLevel() < m_waterLevel) {
                        float newLevel = averageLevel;
                        reinterpret_cast<BlockWater*>(right)->setLevel(newLevel);
                    }
                }
            }

            if(fullySupported && m_waterLevel > 0.96f) m_waterLevel = 1.0f;
        }

        // Now, check to see if there's any air nearby to be filled with water
        {
            unsigned int divisor = 0;

            bool goLeft = false, goRight = false, goDown = false;

            if(left) {
                if(!left->isSolid() && left->getID() != (unsigned int)Categories::BlockIDs::WATER) {
                    divisor++;
                    goLeft = true;
                }
            }
            if(right) {
                if(!right->isSolid() && right->getID() != (unsigned int)Categories::BlockIDs::WATER) {
                    divisor++;
                    goRight = true;
                }
            }
            if(down) {
                if(!down->isSolid() && down->getID() != (unsigned int)Categories::BlockIDs::WATER) {
                    //divisor++; // Only go down if available
                    goDown = true;
                    //goLeft = false;
                    //goRight = false;
                }
            }

            float newLevel = m_waterLevel / (divisor + 1);
            m_waterLevel = newLevel;
            if(goDown) {
                glm::vec2 pos = glm::vec2(m_pos.x, m_pos.y - 1);
                down->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::WATER, pos, m_layer, down->getParentChunk())); /// TODO: Cross-layer water movement

                reinterpret_cast<BlockWater*>(down)->setLevel(m_waterLevel); // All of the water should fall
                m_waterLevel = 0.0f;
            } else {
                if(goLeft) {
                    glm::vec2 pos = glm::vec2(m_pos.x - 1, m_pos.y);
                    left->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::WATER, pos, m_layer, left->getParentChunk())); /// TODO: Cross-layer water movement

                    left = m_parentChunk->getTile(m_pos.x - 1, m_pos.y, m_layer);

                    reinterpret_cast<BlockWater*>(left)->setLevel(newLevel);
                }
                if(goRight) {
                    glm::vec2 pos = glm::vec2(m_pos.x + 1, m_pos.y);
                    right->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::WATER, pos, m_layer, right->getParentChunk())); /// TODO: Cross-layer water movement

                    right = m_parentChunk->getTile(m_pos.x + 1, m_pos.y, m_layer);

                    reinterpret_cast<BlockWater*>(right)->setLevel(newLevel);
                }
            }
        }
    }

    if(m_waterLevel <= 0.0f) {
        //m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, m_pos, m_parentChunk), (int)m_pos.x % CHUNK_SIZE, m_pos.y);
    }
}

BlockPole::BlockPole(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData/* = MetaData()*/, bool loadTexture/* = true*/) : GenericBlock(pos, layer, parent, metaData, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::WOOD_POLE;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = false;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}
