#include "Tile.h"

Tile::Tile() {

}

void Tile::update() {

}

void Tile::draw(GLEngine::SpriteBatch& sb) {
    sb.draw(glm::vec4(m_pos.x * TILE_SIZE, m_pos.y * TILE_SIZE, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, m_colour);
}
