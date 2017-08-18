#include "Tile.h"

Tile::Tile() {

}

void Tile::update() {

}

void Tile::draw(GLEngine::SpriteBatch& sb) {
    sb.draw(glm::vec4(m_pos.x, m_pos.y, m_size.x, m_size.y), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), m_texture.id, 0.0f, m_colour);
}
