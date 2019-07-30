#include "Items.h"

ItemBlockGrass::ItemBlockGrass(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Grass.png")) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_GRASS;
    m_weight = 0.0f; /// TODO

    m_blockId = (unsigned int)Categories::BlockIDs::GRASS;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockDirt::ItemBlockDirt(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png")) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_DIRT;
    m_weight = 0.0f; /// TODO

    m_blockId = (unsigned int)Categories::BlockIDs::DIRT;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockTorch::ItemBlockTorch(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png")) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_TORCH;
    m_weight = 0.0f; /// TODO

    m_blockId = (unsigned int)Categories::BlockIDs::TORCH;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockWood::ItemBlockWood(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png")) {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_WOOD;
    m_weight = 0.0f; /// TODO

    m_blockId = (unsigned int)Categories::BlockIDs::WOOD;

    m_isBlock = true;
    m_canConsume = false;
    m_canPlace = true;
}

ItemMiscBucket::ItemMiscBucket(unsigned int quantity) : Item(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Items/Bucket.png")) {
    m_id = (unsigned int)Categories::ItemIDs::MISC_BUCKET;
    m_weight = 0.0f;

    m_isBlock = false;
    m_canConsume = false;
    m_canPlace = true;
}

ItemMiscMeat_Raw::ItemMiscMeat_Raw(unsigned int quantity) : Item(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/LogPile.png")) {
    m_id = (unsigned int)Categories::ItemIDs::MISC_MEAT_RAW;
    m_weight = 0.0f; /// TODO

    m_isBlock = false;
    m_canConsume = true;
    m_canPlace = false;
}

ItemMiscLeather::ItemMiscLeather(unsigned int quantity) : Item(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Items/Map.png")) {
    m_id = (unsigned int)Categories::ItemIDs::MISC_LEATHER;
    m_weight = 0.0f; /// TODO

    m_isBlock = false;
    m_canConsume = false;
    m_canPlace = false;
}
