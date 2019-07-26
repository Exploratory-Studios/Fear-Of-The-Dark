#include "Blocks.h"

BlockAir::BlockAir(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::AIR;

    m_transparent = true; // Does light travel through it?
    m_draw = false; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

    if(parent->tiles[(unsigned int)pos.y][(unsigned int)pos.x % CHUNK_SIZE])
        m_ambientLight = parent->tiles[(unsigned int)pos.y][(unsigned int)pos.x % CHUNK_SIZE]->getLight();
}

BlockDirt::BlockDirt(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
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
    if(m_sunLight >= 0.5f) {
        int chance = std::rand() % 100;
        if(chance < 1) { // 1% chance every tick
            m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::GRASS, m_pos, m_parentChunk, tickTime), m_pos.x, m_pos.y);
        }
    }
}

BlockGrass::BlockGrass(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
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
    if(m_sunLight >= 0.5f) {
        int chance = std::rand() % 1000;
        if(chance < 2 && m_parentChunk->tiles[(unsigned int)m_pos.y + 1][(unsigned int)m_pos.x % CHUNK_SIZE]->getID() == (unsigned int)Categories::BlockIDs::AIR) { // 0.2% chance every tick
            //m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::FOLIAGE, m_pos + glm::vec2(0.0f, 1.0f), m_parentChunk), m_pos.x, m_pos.y + 1);
        }
    }
}

BlockTorch::BlockTorch(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::TORCH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 1.5f; // Does it emit light?
    m_emittedHeat = 20.0f;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}

BlockBush::BlockBush(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::BUSH;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}


BlockStone::BlockStone(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::STONE;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = true;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}

BlockFoliage::BlockFoliage(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
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
    if(m_parentChunk->tiles[(unsigned int)m_pos.y-1][((unsigned int)(m_pos.x + CHUNK_SIZE) % CHUNK_SIZE)]->getID() != (unsigned int)Categories::BlockIDs::GRASS) {
        m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, m_pos, m_parentChunk), m_pos.x, m_pos.y);
    }
}

BlockWood::BlockWood(glm::vec2 pos, Chunk* parent, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::WOOD;

    m_transparent = false; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = true; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = true;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }
}

BlockWater::BlockWater(glm::vec2 pos, Chunk* parent, float level, bool loadTexture/* = true*/) : GenericBlock(pos, parent, loadTexture) {
    m_id = (unsigned int)Categories::BlockIDs::WATER;

    m_transparent = true; // Does light travel through it?
    m_draw = true; // Is it drawn?
    m_solid = false; // Can we walk through it?
    m_emittedLight = 0.0f; // Does it emit light?

    m_backdrop = false;

    if(loadTexture) { this->loadTexture(); } else { m_textureId = (GLuint)-1; }

    m_waterLevel = level;
}

void BlockWater::draw(GLEngine::SpriteBatch& sb, int xOffset) {
    if(m_textureId == (GLuint)-1) {
        loadTexture();
    }

    int r = m_colour.r, g = m_colour.g, b = m_colour.b;
    sb.draw(glm::vec4(m_pos.x * TILE_SIZE + xOffset * CHUNK_SIZE * TILE_SIZE, m_pos.y * TILE_SIZE, m_size.x * TILE_SIZE, m_waterLevel * TILE_SIZE),
            glm::vec4(0.0f, 0.0f, 1.0f, m_waterLevel),
            m_textureId,
            2.0f,
            GLEngine::ColourRGBA8(r, g, b, m_colour.a),
            glm::vec3(getLight()));
}
#include <iostream>
void BlockWater::onUpdate(float& time) {
    // 'Spread' water to other blocks, if they are passable blocks


    if(m_waterLevel > 0.0f) {
        Tile* left = nullptr;
        Tile* right = nullptr;
        Tile* down = nullptr;

        if((int)(m_pos.x) % CHUNK_SIZE > 0) {
            left = m_parentChunk->tiles[(int)m_pos.y][(int)(m_pos.x - 1) % CHUNK_SIZE];
        } else {
            left = m_parentChunk->extraTiles[(int)m_pos.y][0];
        }

        if((int)(m_pos.x) % CHUNK_SIZE < CHUNK_SIZE-1) {
            right = m_parentChunk->tiles[(int)m_pos.y][(int)(m_pos.x + 1) % CHUNK_SIZE];
        } else {
            right = m_parentChunk->extraTiles[(int)m_pos.y][1];
        }

        if((int)(m_pos.y) > 0) {
            down = m_parentChunk->tiles[(int)m_pos.y-1][(int)m_pos.x % CHUNK_SIZE];
        }

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
                down->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::WATER, pos, down->getParentChunk()), (int)pos.x, (int)pos.y);

                reinterpret_cast<BlockWater*>(down)->setLevel(m_waterLevel); // All of the water should fall
                m_waterLevel = 0.0f;
            } else {
                if(goLeft) {
                    glm::vec2 pos = glm::vec2(m_pos.x - 1, m_pos.y);
                    left->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::WATER, pos, left->getParentChunk()), (int)pos.x % CHUNK_SIZE, (int)pos.y);

                    if((int)(m_pos.x) % CHUNK_SIZE > 0) {
                        left = m_parentChunk->tiles[(int)m_pos.y][(int)(m_pos.x - 1) % CHUNK_SIZE];
                    } else {
                        left = m_parentChunk->extraTiles[(int)m_pos.y][0];
                    }

                    reinterpret_cast<BlockWater*>(left)->setLevel(newLevel);
                }
                if(goRight) {
                    glm::vec2 pos = glm::vec2(m_pos.x + 1, m_pos.y);
                    right->getParentChunk()->setTile(createBlock((unsigned int)Categories::BlockIDs::WATER, pos, right->getParentChunk()), (int)pos.x % CHUNK_SIZE, (int)pos.y);

                    if((int)(m_pos.x) % CHUNK_SIZE < CHUNK_SIZE-1) {
                        right = m_parentChunk->tiles[(int)m_pos.y][(int)(m_pos.x + 1) % CHUNK_SIZE];
                    } else {
                        right = m_parentChunk->extraTiles[(int)m_pos.y][1];
                    }

                    reinterpret_cast<BlockWater*>(right)->setLevel(newLevel);
                }
            }
        }
    }

    if(m_waterLevel <= 0.0f) {
        //m_parentChunk->setTile(createBlock((unsigned int)Categories::BlockIDs::AIR, m_pos, m_parentChunk), (int)m_pos.x % CHUNK_SIZE, m_pos.y);
    }
}
