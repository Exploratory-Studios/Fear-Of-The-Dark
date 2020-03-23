#pragma once

#include "Item.h"
#include "ItemBlock.h"
#include "World.h"

class ItemBlockGrass : public ItemBlock {
	public:
		ItemBlockGrass(unsigned int quantity);
};

class ItemBlockDirt : public ItemBlock {
	public:
		ItemBlockDirt(unsigned int quantity);
};

class ItemBlockTorch : public ItemBlock {
	public:
		ItemBlockTorch(unsigned int quantity);
};

class ItemBlockTorchBright : public ItemBlock {
    public:
        ItemBlockTorchBright(unsigned int quantity);
};

class ItemBlockTorchAnti : public ItemBlock {
    public:
        ItemBlockTorchAnti(unsigned int quantity);
};

class ItemBlockWood : public ItemBlock {
	public:
		ItemBlockWood(unsigned int quantity);
};

class ItemBlockWood_Pole : public ItemBlock {
    public:
        ItemBlockWood_Pole(unsigned int quantity);
};

class ItemBlockSign_Wood : public ItemBlock {
    public:
        ItemBlockSign_Wood(unsigned int quantity);
};

class ItemMiscBucket : public Item {
	public:
		ItemMiscBucket(unsigned int quantity);

		virtual void onLeftClick(Tile* selectedBlock, World* world) override {
			/*if(selectedBlock) {
				if(selectedBlock->getID() == (unsigned int)Categories::BlockIDs::WATER) {
					m_level += static_cast<BlockWater*>(selectedBlock)->getLevel();

					Tile* b = createBlock((unsigned int)Categories::BlockIDs::AIR, selectedBlock->getPosition(), selectedBlock->getLayer());
					b->setAmbientLight(selectedBlock->getLight());
					world->setTile(b);
				}
			}*/
		}
		virtual void onRightClick(Tile* selectedBlock, World* world) override {
			/*if(selectedBlock && m_level > 0.0f) {
				BlockWater* b = static_cast<BlockWater*>(createBlock((unsigned int)Categories::BlockIDs::WATER, selectedBlock->getPosition(), selectedBlock->getLayer()));
				b->setAmbientLight(selectedBlock->getLight());

				float addedLevel = m_level > 1.0f ? 1.0f : m_level;

				b->setLevel(addedLevel);
				m_level -= addedLevel;
				world->setTile(b);
			}*/
		}

	private:
		float m_level = 0.0f;
};

class ItemMiscMeat_Raw : public Item {
	public:
		ItemMiscMeat_Raw(unsigned int quantity);
};

class ItemMiscLeather : public Item {
	public:
		ItemMiscLeather(unsigned int quantity);
};

class ItemWeapon : public Item {
	public:
		float getDamage() const {
			return m_damage;
		}
		float getSpeed() const  {
			return m_speed;
		}
		float getReach() const  {
			return m_reach;
		}

	protected:
		ItemWeapon(unsigned int quantity, GLEngine::GLTexture texture, std::string name, float damage, float speed, float reach); // Abstract class, biotch

		float m_damage = 1.0f;
		float m_speed = 1.0f;
		float m_reach = 1.0f; // In metres
};

class ItemWeaponSword : public ItemWeapon {
	public:
		ItemWeaponSword(unsigned int quantity);
};

static Item* createItem(unsigned int id, unsigned int quantity) {
	Item* ret = nullptr;

	switch(id) {
        case (unsigned int)Categories::ItemIDs::BLOCK_GRASS:
            ret = new ItemBlockGrass(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_DIRT:
            ret = new ItemBlockDirt(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_TORCH:
            ret = new ItemBlockTorch(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_WOOD:
            ret = new ItemBlockWood(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_SIGN_WOOD:
            ret = new ItemBlockSign_Wood(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::MISC_BUCKET:
            ret = new ItemMiscBucket(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::MISC_MEAT_RAW:
            ret = new ItemMiscMeat_Raw(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::MISC_LEATHER:
            ret = new ItemMiscLeather(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::WEAPON_SWORD:
            ret = new ItemWeaponSword(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_WOOD_POLE:
            ret = new ItemBlockWood_Pole(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_TORCH_BRIGHT:
            ret = new ItemBlockTorchBright(quantity);
            break;
        case (unsigned int)Categories::ItemIDs::BLOCK_TORCH_ANTI:
            ret = new ItemBlockTorchAnti(quantity);
            break;
        default:
            ret = new Item(1, GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/UNDEFINED.png"), "UNDEFINED");
            Logger::getInstance()->log("UNKNOWN ITEM ID: " + std::to_string(id), true);
	}

	return ret;
}
