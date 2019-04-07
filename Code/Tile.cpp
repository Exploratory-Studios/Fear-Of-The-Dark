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
    } else if(m_emittedLight == m_sunLight && m_emittedLight == m_ambientLight) {
        return m_emittedLight;
    }
    return m_ambientLight;
}

float Tile::getSurroundingLight() {
    int x = (int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex();
    int y = (int)m_pos.y;

    float light = 0.0f;

    if(y-1 >= 0)
        if(m_parentChunk->tiles[y-1][x]->getLight() > light) {
            if(m_parentChunk->tiles[y-1][x]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->tiles[y-1][x]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->tiles[y-1][x]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    if(y+1 < WORLD_HEIGHT)
        if(m_parentChunk->tiles[y+1][x]->getLight() > light) {
            if(m_parentChunk->tiles[y+1][x]->isTransparent() || isTransparent()) {
                float newLight = m_parentChunk->tiles[y+1][x]->getLight() * TRANSPARENT_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            } else {
                float newLight = m_parentChunk->tiles[y+1][x]->getLight() * OPAQUE_LIGHT_MULTIPLIER;
                if(newLight > light) light = newLight;
            }
        }
    if(x-1 >= 0) {
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
    if(x+1 < CHUNK_SIZE) {
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
#include <iostream>
void Tile::update(float time) {
    float light = getSurroundingLight();
    setAmbientLight(light);
    m_lastLight = light;
}

void Tile::tick(float* tickTime) {
    if(exposedToSun()) { // check if this block is exposed to sunlight
        m_sunLight = cos((float)*tickTime / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
    } else {
        m_sunLight = 0.0f;
    }
}

void Tile::draw(GLEngine::SpriteBatch& sb, int xOffset) {

    if(m_draw) {
        //GLint lightColourUniform = program.getUniformLocation("lightColour");
        //glUniform4fv(lightColourUniform, 3, &glm::vec3(0.0f, 1.0f, 0.0f)[0]);

        int r = m_colour.r, g = m_colour.g, b = m_colour.b;
        sb.draw(glm::vec4(m_pos.x * TILE_SIZE + xOffset * CHUNK_SIZE * TILE_SIZE, m_pos.y * TILE_SIZE, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE),
                glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
                m_texture.id,
                0.0f,
                GLEngine::ColourRGBA8(r, g, b, m_colour.a),
                glm::vec3(getLight()));
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
