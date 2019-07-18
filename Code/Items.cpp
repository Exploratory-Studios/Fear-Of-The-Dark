#include "Items.h"

ItemBlockGrass::ItemBlockGrass(unsigned int quantity) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_GRASS;
    m_weight = 0.0f; /// TODO
    m_quantity = quantity;

    m_blockId = (unsigned int)Categories::BlockIDs::GRASS;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockDirt::ItemBlockDirt(unsigned int quantity) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_DIRT;
    m_weight = 0.0f; /// TODO
    m_quantity = quantity;

    m_blockId = (unsigned int)Categories::BlockIDs::DIRT;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockTorch::ItemBlockTorch(unsigned int quantity) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_TORCH;
    m_weight = 0.0f; /// TODO
    m_quantity = quantity;

    m_blockId = (unsigned int)Categories::BlockIDs::TORCH;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockWood::ItemBlockWood(unsigned int quantity) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_WOOD;
    m_weight = 0.0f; /// TODO
    m_quantity = quantity;

    m_blockId = (unsigned int)Categories::BlockIDs::WOOD;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}
