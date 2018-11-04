#include "Tile.h"

Tile::Tile() {

}

float Tile::getLight() {
    if(m_ambientLight > m_sunLight && m_ambientLight > m_emittedLight) {
        return m_ambientLight;
    } else if(m_sunLight > m_ambientLight && m_sunLight > m_emittedLight) {
        return m_sunLight;
    } else if(m_emittedLight > m_sunLight && m_emittedLight > m_ambientLight) {
        return m_emittedLight;
    } else {
        return m_ambientLight; // They're all the same, so ambientLight is correct, but so is any other variable.
    }
}

float Tile::getSurroundingLight() {
    int x = (int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex();
    int y = (int)m_pos.y;

    float light = 0.0f;

    if(y-1 >= 0)
        if(m_parentChunk->tiles[y-1][x]->getLight() > light) {
            light = m_parentChunk->tiles[y-1][x]->getLight();
        }
    if(y+1 < WORLD_HEIGHT)
        if(m_parentChunk->tiles[y+1][x]->getLight() > light) {
            light = m_parentChunk->tiles[y+1][x]->getLight();
        }
    if(x-1 >= 0) {
        if(m_parentChunk->tiles[y][x-1]->getLight() > light) {
            light = m_parentChunk->tiles[y][x-1]->getLight();
        }
    } else if(x-1 == -1) {
        if(m_parentChunk->extraTiles[y][0]->getLight() > light) {
            light = m_parentChunk->extraTiles[y][0]->getLight();
        }
    }
    if(x+1 < CHUNK_SIZE) {
        if(m_parentChunk->tiles[y][x+1]->getLight() > light) {
            light = m_parentChunk->tiles[y][x+1]->getLight();
        }
    } else if(x+1 == CHUNK_SIZE) {
        if(m_parentChunk->extraTiles[y][1]->getLight() > light) {
            light = m_parentChunk->extraTiles[y][1]->getLight();
        }
    }

    return light;
}

void Tile::update(float time) {
    if((getLight() != m_lastLight || getSurroundingLight() != getAmbientLight()) && (int)m_pos.y-m_parentChunk->getIndex() >= 0) {
        m_lastLight = getLight();
        int x = (int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex();
        int y = (int)m_pos.y;
        if(y-1 >= 0)
            if(m_transparent) {
                if(m_parentChunk->tiles[y-1][x]->getAmbientLight() < getLight()) {
                    m_parentChunk->tiles[y-1][x]->setAmbientLight(getLight() * LIGHT_MULTIPLIER);
                } else {
                    if(m_parentChunk->tiles[y-1][x]->isTransparent())
                        setAmbientLight(m_parentChunk->tiles[y-1][x]->getAmbientLight() * LIGHT_MULTIPLIER);
                }
            }
        if(y+1 < WORLD_HEIGHT) {
            if(m_transparent) {
                if(m_parentChunk->tiles[y+1][x]->getAmbientLight() < getLight()) {
                    m_parentChunk->tiles[y+1][x]->setAmbientLight(getLight() * LIGHT_MULTIPLIER);
                } else {
                    if(m_parentChunk->tiles[y+1][x]->isTransparent())
                        setAmbientLight(m_parentChunk->tiles[y+1][x]->getAmbientLight() * LIGHT_MULTIPLIER);
                }
            }
        }
        if(x-1 >= 0) {
            if(m_transparent) {
                if(m_parentChunk->tiles[y][x-1]->getAmbientLight() < getLight()) {
                    m_parentChunk->tiles[y][x-1]->setAmbientLight(getLight() * LIGHT_MULTIPLIER);
                } else {
                    if(m_parentChunk->tiles[y][x-1]->isTransparent())
                        setAmbientLight(m_parentChunk->tiles[y][x-1]->getAmbientLight() * LIGHT_MULTIPLIER);
                }
            }
        } else if(x-1 == -1) {
            if(m_transparent) {
                if(m_parentChunk->extraTiles[y][0]->getAmbientLight() < getLight()) {
                    m_parentChunk->extraTiles[y][0]->setAmbientLight(getLight() * LIGHT_MULTIPLIER);
                } else {
                    if(m_parentChunk->tiles[y][0]->isTransparent())
                        setAmbientLight(m_parentChunk->extraTiles[y][0]->getAmbientLight() * LIGHT_MULTIPLIER);
                }
            }
        }
        if(x+1 < CHUNK_SIZE) {
            if(m_transparent) {
                if(m_parentChunk->tiles[y][x+1]->getAmbientLight() < getLight()) {
                    m_parentChunk->tiles[y][x+1]->setAmbientLight(getLight() * LIGHT_MULTIPLIER);
                } else {
                    if(m_parentChunk->tiles[y][x+1]->isTransparent())
                        setAmbientLight(m_parentChunk->tiles[y][x+1]->getAmbientLight() * LIGHT_MULTIPLIER);
                }
            }
        } else if(x+1 == CHUNK_SIZE) {
            if(m_transparent) {
                if(m_parentChunk->extraTiles[y][1]->getAmbientLight() < getLight()) {
                    m_parentChunk->extraTiles[y][1]->setAmbientLight(getLight() * LIGHT_MULTIPLIER);
                } else {
                    if(m_parentChunk->tiles[y][1]->isTransparent())
                        setAmbientLight(m_parentChunk->extraTiles[y][1]->getAmbientLight() * LIGHT_MULTIPLIER);
                }
            }
        }
    }
}

void Tile::tick(int tickTime) {
    if(exposedToSun()) { // check if this block is exposed to sunlight
        m_sunLight = cos((float)tickTime / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
    }
}

void Tile::draw(GLEngine::SpriteBatch& sb, int xOffset) {

    if(!m_transparent) {
        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        if(getLight() < 1.0f) sb.draw(glm::vec4(m_pos.x * TILE_SIZE + xOffset * CHUNK_SIZE * TILE_SIZE, m_pos.y * TILE_SIZE, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, GLEngine::ColourRGBA8(m_colour.r * getLight(), m_colour.g * getLight(), m_colour.b * getLight(), m_colour.a));
        if(getLight() >= 1.0f) {
            int r = m_colour.r, g = m_colour.g, b = m_colour.b;
            sb.draw(glm::vec4(m_pos.x * TILE_SIZE + xOffset * CHUNK_SIZE * TILE_SIZE, m_pos.y * TILE_SIZE, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, GLEngine::ColourRGBA8(r, g, b, m_colour.a), glm::vec3(getLight()));
        }
    }
}

bool Tile::exposedToSun() {
    for(int i = WORLD_HEIGHT-1; i > m_pos.y; i--) {
        if(m_parentChunk->tiles[i][(int)m_pos.x % CHUNK_SIZE]->isSolid()) {
            return false;
        }
    }
    return true;
}
