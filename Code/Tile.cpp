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
        m_sunLight = tickTime / 100.0f;
    }
    if((int)m_pos.y-m_parentChunk->getIndex() >= 0) {
        int x = (int)m_pos.x - CHUNK_SIZE*m_parentChunk->getIndex();
        int y = (int)m_pos.y;
        if(y-1 >= 0)
            if(m_transparent || m_parentChunk->tiles[y-1][x]->isTransparent())
                if(m_parentChunk->tiles[y-1][x]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[y-1][x]->setAmbientLight(getLight() * 9.0f/10.0f);
        if(y+1 < WORLD_HEIGHT)
            if(m_transparent || m_parentChunk->tiles[y+1][x]->isTransparent())
                if(m_parentChunk->tiles[y+1][x]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[y+1][x]->setAmbientLight(getLight() * 9.0f/10.0f);
        if(x-1 >= 0)
            if(m_transparent || m_parentChunk->tiles[y][x-1]->isTransparent())
                if(m_parentChunk->tiles[y][x-1]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[y][x-1]->setAmbientLight(getLight() * 9.0f/10.0f);
        if(x+1 < CHUNK_SIZE)
            if(m_transparent || m_parentChunk->tiles[y][x+1]->isTransparent())
                if(m_parentChunk->tiles[y][x+1]->getAmbientLight() < getLight())
                    m_parentChunk->tiles[y][x+1]->setAmbientLight(getLight() * 9.0f/10.0f);
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
