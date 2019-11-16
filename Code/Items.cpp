#include "Items.h"

ItemBlockGrass::ItemBlockGrass(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Grass.png"), "Grass Block") {
	m_id = (unsigned int)Categories::ItemIDs::BLOCK_GRASS;
	m_type = ItemType::BLOCK;
	m_weight = 0.0f; /// TODO

	m_blockId = (unsigned int)Categories::BlockIDs::GRASS;

	m_canConsume = false;
	m_canPlace = true;
}

ItemBlockDirt::ItemBlockDirt(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Dirt.png"), "Dirt Block") {
	m_id = (unsigned int)Categories::ItemIDs::BLOCK_DIRT;
	m_type = ItemType::BLOCK;
	m_weight = 0.0f; /// TODO

	m_blockId = (unsigned int)Categories::BlockIDs::DIRT;

	m_canConsume = false;
	m_canPlace = true;
}

ItemBlockTorch::ItemBlockTorch(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png"), "Torch") {
	m_id = (unsigned int)Categories::ItemIDs::BLOCK_TORCH;
	m_type = ItemType::BLOCK;
	m_weight = 0.0f; /// TODO

	m_blockId = (unsigned int)Categories::BlockIDs::TORCH;

	m_canConsume = false;
	m_canPlace = true;
}

ItemBlockTorchBright::ItemBlockTorchBright(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png"), "Super Torch") {
	m_id = (unsigned int)Categories::ItemIDs::BLOCK_TORCH_BRIGHT;
	m_type = ItemType::BLOCK;
	m_weight = 0.0f; /// TODO

	m_blockId = (unsigned int)Categories::BlockIDs::TORCH_BRIGHT;

	m_canConsume = false;
	m_canPlace = true;
}

ItemBlockTorchAnti::ItemBlockTorchAnti(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Torch.png"), "Anti-Torch") {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_TORCH_ANTI;
	m_type = ItemType::BLOCK;
	m_weight = 0.0f; /// TODO

	m_blockId = (unsigned int)Categories::BlockIDs::TORCH_ANTI;

	m_canConsume = false;
	m_canPlace = true;
}

ItemBlockWood::ItemBlockWood(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/WoodBlock.png"), "Wood Block") {
	m_id = (unsigned int)Categories::ItemIDs::BLOCK_WOOD;
	m_type = ItemType::BLOCK;
	m_weight = 0.0f; /// TODO

	m_blockId = (unsigned int)Categories::BlockIDs::WOOD;

	m_canConsume = false;
	m_canPlace = true;
}

ItemBlockWood_Pole::ItemBlockWood_Pole(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Wood_Pole.png"), "Wood Pole") {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_WOOD_POLE;
    m_type = ItemType::BLOCK;
    m_weight = 0.0f;

    m_blockId = (unsigned int)Categories::BlockIDs::WOOD_POLE;

    m_canConsume = false;
    m_canPlace = true;
}

ItemBlockSign_Wood::ItemBlockSign_Wood(unsigned int quantity) : ItemBlock(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/Wood_Sign.png"), "Wood Sign") {
    m_id = (unsigned int)Categories::ItemIDs::BLOCK_SIGN_WOOD;
    m_type = ItemType::BLOCK;
    m_weight = 0.0f;

    m_blockId = (unsigned int)Categories::BlockIDs::SIGN_WOOD;

    m_canConsume = false;
    m_canPlace = true;
}

ItemMiscBucket::ItemMiscBucket(unsigned int quantity) : Item(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Items/Bucket.png"), "Bucket") {
	m_id = (unsigned int)Categories::ItemIDs::MISC_BUCKET;
	m_type = ItemType::MISC;
	m_weight = 0.0f;

	m_canConsume = false;
	m_canPlace = true;
}

ItemMiscMeat_Raw::ItemMiscMeat_Raw(unsigned int quantity) : Item(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Blocks/LogPile.png"), "Raw Meat") {
	m_id = (unsigned int)Categories::ItemIDs::MISC_MEAT_RAW;
	m_type = ItemType::MISC;
	m_weight = 0.0f; /// TODO

	m_canConsume = true;
	m_canPlace = false;
}

ItemMiscLeather::ItemMiscLeather(unsigned int quantity) : Item(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Items/Map.png"), "Leather") {
	m_id = (unsigned int)Categories::ItemIDs::MISC_LEATHER;
	m_type = ItemType::MISC;
	m_weight = 0.0f; /// TODO

	m_canConsume = false;
	m_canPlace = false;
}

ItemWeapon::ItemWeapon(unsigned int quantity, GLEngine::GLTexture texture, std::string name, float damage, float speed, float reach) : Item(quantity, texture, name) {
	m_type = ItemType::WEAPON;
	m_canConsume = false;
	m_canPlace = false;

	m_damage = damage;
	m_speed = speed;
	m_reach = reach;
}

ItemWeaponSword::ItemWeaponSword(unsigned int quantity) : ItemWeapon(quantity, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "Textures/Items/Sword0.png"), "Sword", 2.0f, 1.0f, 0.4f) {
	m_id = (unsigned int)Categories::ItemIDs::WEAPON_SWORD;
	m_weight = 0.0f; /// TODO
}
