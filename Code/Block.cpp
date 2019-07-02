#include "Block.h"

#include <random>
#include <time.h>

Block::Block(glm::vec2 pos, Chunk* parent) {

    m_parentChunk = parent;
    m_pos = pos;
}
