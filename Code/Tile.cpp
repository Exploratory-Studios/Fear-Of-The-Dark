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
    }
}

void Tile::update(float time) {

}

void Tile::tick(int tickTime) {
    if(exposedToSun()) { // check if this block is exposed to sunlight
        m_sunLight = (std::cos((float)tickTime / (float)DAY_LENGTH) + 1.0f) / 2.0f;
    }
    if(!m_transparent) {
        if((int)m_pos.y-m_parentChunk->getIndex() >= 0) {
            if((int)m_pos.y-1-m_parentChunk->getIndex() >= 0)
                if(m_parentChunk->tiles[(int)m_pos.y-1][(int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex()]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[(int)m_pos.y-1][(int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex()]->setAmbientLight(getLight() * 7.0f/10.0f);
            if((int)m_pos.y+1-m_parentChunk->getIndex() < WORLD_HEIGHT)
                if(m_parentChunk->tiles[(int)m_pos.y+1][(int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex()]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[(int)m_pos.y+1][(int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex()]->setAmbientLight(getLight() * 7.0f/10.0f);
            if((int)m_pos.x-1 - CHUNK_SIZE*m_parentChunk->getIndex() >= 0)
                if(m_parentChunk->tiles[(int)m_pos.y][(int)m_pos.x-1 - CHUNK_SIZE*m_parentChunk->getIndex()]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[(int)m_pos.y][(int)m_pos.x-1 - CHUNK_SIZE*m_parentChunk->getIndex()]->setAmbientLight(getLight() * 7.0f/10.0f);
            if((int)m_pos.x+1 - CHUNK_SIZE*m_parentChunk->getIndex() < CHUNK_SIZE)
                if(m_parentChunk->tiles[(int)m_pos.y][(int)m_pos.x+1 - CHUNK_SIZE*m_parentChunk->getIndex()]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[(int)m_pos.y][(int)m_pos.x+1 - CHUNK_SIZE*m_parentChunk->getIndex()]->setAmbientLight(getLight() * 7.0f/10.0f);
        }
    }
}

void Tile::draw(GLEngine::SpriteBatch& sb) {
    if(!m_transparent) sb.draw(glm::vec4(m_pos.x * TILE_SIZE, m_pos.y * TILE_SIZE, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, GLEngine::ColourRGBA8(m_colour.r * getLight(), m_colour.g * getLight(), m_colour.b * getLight(), m_colour.a));
}

bool Tile::exposedToSun() {
    for(int i = WORLD_HEIGHT-1; i > m_pos.y; i--) {
        if(m_parentChunk->tiles[i][(int)m_pos.x % CHUNK_SIZE]->isSolid()) {
            return false;
        }
    }
    return true;
}
