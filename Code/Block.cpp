#include "Block.h"

#include <random>
#include <time.h>

Block::Block(glm::vec2 pos, unsigned int layer, MetaData metaData) : Tile() {
    m_pos = pos;
    m_layer = layer;
    m_metaData = metaData;
}
