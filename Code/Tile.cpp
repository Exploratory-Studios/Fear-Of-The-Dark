#include "Tile.h"

#include <random>

Tile::Tile() {
    m_textureId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/UNDEFINED.png").id;
    m_parentChunk = nullptr;
}

float Tile::getLight() {
    if(m_ambientLight > m_sunLight && m_ambientLight > m_emittedLight) {
        return m_ambientLight;
    } else if(m_sunLight > m_ambientLight && m_sunLight > m_emittedLight) {
        return m_sunLight;
    } else if(m_emittedLight > m_sunLight && m_emittedLight > m_ambientLight) {
        return m_emittedLight;
    }
    return m_ambientLight;
}

float Tile::getSurroundingLight() {
    int x = (int)m_pos.x % CHUNK_SIZE;
    int y = (int)m_pos.y;

    float light = 0.0f;

    if(y-1 >= 0) {// 1 below
        if(m_parentChunk->tiles[y-1][x]->getLight() > light) {
            if(m_parentChunk->tiles[y-1][x]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->tiles[y-1][x]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->tiles[y-1][x]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    }
    if(y+1 < WORLD_HEIGHT) { // 1 above
        if(m_parentChunk->tiles[y+1][x]->getLight() > light) {
            if(m_parentChunk->tiles[y+1][x]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->tiles[y+1][x]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->tiles[y+1][x]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    }
    if(x-1 >= 0) { // to the left
        if(m_parentChunk->tiles[y][x-1]->getLight() > light) {
            if(m_parentChunk->tiles[y][x-1]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->tiles[y][x-1]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->tiles[y][x-1]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    } else if(x-1 == -1) {
        if(m_parentChunk->extraTiles[y][0]->getLight() > light) {
            if(m_parentChunk->extraTiles[y][0]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->extraTiles[y][0]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->extraTiles[y][0]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    }
    if(x+1 < CHUNK_SIZE) { // to the right
        if(m_parentChunk->tiles[y][x+1]->getLight() > light) {
            if(m_parentChunk->tiles[y][x+1]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->tiles[y][x+1]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->tiles[y][x+1]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    } else if(x+1 == CHUNK_SIZE) {
        if(m_parentChunk->extraTiles[y][1]->getLight() > light) {
            if(m_parentChunk->extraTiles[y][1]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->extraTiles[y][1]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->extraTiles[y][1]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    } else if(x+1 == CHUNK_SIZE+1) {
        if(m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->getLight() > light) {
            if(m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    } else if(x-1 == -2) {
        if(m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->getLight() > light) {
            if(m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    }

    return light;
}

float Tile::getSurroundingHeat() {
    int x = (int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex();
    int y = (int)m_pos.y;

    float temp = 0.0f;

    if(y-1 >= 0) {// 1 below
        if(!m_parentChunk->tiles[y-1][x]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->tiles[y-1][x]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->tiles[y-1][x]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    }
    if(y+1 < WORLD_HEIGHT) { // 1 above
        if(!m_parentChunk->tiles[y+1][x]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->tiles[y+1][x]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->tiles[y+1][x]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    }
    if(x-1 >= 0) { // to the left
        if(!m_parentChunk->tiles[y][x-1]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->tiles[y][x-1]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->tiles[y][x-1]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    } else if(x-1 == -1) {
        if(!m_parentChunk->extraTiles[y][0]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->extraTiles[y][0]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->extraTiles[y][0]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    }
    if(x+1 < CHUNK_SIZE) { // to the right
        if(!m_parentChunk->tiles[y][x+1]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->tiles[y][x+1]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->tiles[y][x+1]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    } else if(x+1 == CHUNK_SIZE) {
        if(!m_parentChunk->extraTiles[y][1]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->extraTiles[y][1]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->extraTiles[y][1]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    } else if(x+1 == CHUNK_SIZE+1) {
        if(!m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->getSurroundingChunks()[1]->tiles[y][0]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    } else if(x-1 == -2) {
        if(!m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->isSolid() || !isSolid()) {
            float newHeat = m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->getRawHeat() * TRANSPARENT_LIGHT_MULTIPLIER;
            temp += newHeat;
        } else {
            float newHeat = m_parentChunk->getSurroundingChunks()[0]->tiles[y][CHUNK_SIZE-1]->getRawHeat() * OPAQUE_LIGHT_MULTIPLIER;
            temp += newHeat;
        }
    }

    return temp / 4.0f;
}

float Tile::getHeat() {
    float baseHeat = Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].baseTemperature;
    float highHeat = Category_Data::placeData[(unsigned int)m_parentChunk->getPlace()].maxTemp;

    float heat = getRawHeat();

    heat += m_sunLight * (highHeat - baseHeat);
    heat += baseHeat;

    return heat;
}

float Tile::getRawHeat() {
    return (m_emittedHeat > m_temperature ? m_emittedHeat : m_temperature);
}

void Tile::update(float time) {
    float light = getSurroundingLight();
    setAmbientLight(light);
    m_lastLight = light;
    onUpdate(time);
}

void Tile::tick(float tickTime) {
    float heat = getSurroundingHeat();
    m_temperature = heat;

    setSunlight(tickTime);
    onTick(tickTime);
}

void Tile::draw(GLEngine::SpriteBatch& sb, int xOffset) {

    if(m_draw) {
        if(m_textureId == (GLuint)-1) {
            loadTexture();
        }

        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        int r = m_colour.r, g = m_colour.g, b = m_colour.b;
        sb.draw(glm::vec4(m_pos.x + xOffset * CHUNK_SIZE, m_pos.y, m_size.x, m_size.y),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_textureId,
                0.75f,
                GLEngine::ColourRGBA8(r, g, b, m_colour.a),
                glm::vec3(getLight()));
    }
}

void Tile::drawBackdrop(GLEngine::SpriteBatch& sb, int xOffset, int yOffset, float lightLevel) {
    if(m_draw && m_backdrop) {
        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        int r = m_colour.r * 0.2f, g = m_colour.g * 0.2f, b = m_colour.b * 0.2f;
        sb.draw(glm::vec4(m_pos.x + xOffset * CHUNK_SIZE, m_pos.y + yOffset, m_size.x, m_size.y),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_backdropTextureId,
                0.1f,
                GLEngine::ColourRGBA8(r, g, b, m_colour.a),
                glm::vec3(lightLevel));
    }
}

bool Tile::exposedToSun() {
    for(int i = WORLD_HEIGHT-1; i > m_pos.y; i--) {
        if(!m_parentChunk->tiles[i][(int)m_pos.x % CHUNK_SIZE]->isTransparent()) {
            return false;
        }
    }
    return true;
}
