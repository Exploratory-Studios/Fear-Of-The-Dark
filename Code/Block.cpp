#include "Block.h"

#include <random>
#include <time.h>

Block::Block(glm::vec2 pos, Chunk* parent) {

    m_parentChunk = parent;
    m_pos = pos;

    m_ambientLight = 0.0f; // These should all start at 0, no matter what the block
    m_sunLight = 0.0f;
    m_lastLight = 0.0f;
}
