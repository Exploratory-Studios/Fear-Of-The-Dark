#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include <Texture.hpp>
#include <ResourceManager.hpp>
#include <LuaScriptQueue.hpp>

#include "SaveDataTypes.h"

class World;
class Tile;

enum class ItemIDs { /// DEPRECATED, but still used (dumb, it'll get fixed eventually.)
	WEAPON_SWORD,
	BLOCK_DIRT,
	BLOCK_TORCH,
	BLOCK_TORCH_BRIGHT,
	BLOCK_TORCH_ANTI,
	BLOCK_GRASS,
	BLOCK_WOOD,
	BLOCK_WOOD_POLE,
	BLOCK_SIGN_WOOD,
	MISC_BUCKET,
	MISC_MEAT_RAW,
	MISC_LEATHER
};

class Item {
  public:
	Item(short unsigned int quantity, unsigned int id);
	virtual ~Item() {
	}

	void init();

	virtual void onRightClick(Tile* selectedBlock);

	void addToQuantity(int amnt) {
		m_quantity += amnt;
	}

	unsigned int getID() const {
		return m_id; // (unsigned int)(-1) is equivalent to null
	}
	short unsigned int getQuantity() const {
		return m_quantity;
	}
	std::string& getName() {
		return m_name;
	}
	float getWeight() const {
		return m_weight;
	}

	unsigned int getTextureID() {
		return m_texture.id;
	}

	SaveDataTypes::ItemData getItemSaveData();

  protected:
	unsigned int m_id = (unsigned int)-1; // Block/Non-block id

	float			   m_weight	  = 0.0f; // How much it weighs in the inventory (kgs)
	short unsigned int m_quantity = 0;	  // How much you have

	BARE2D::LuaScript m_useScript;

	BARE2D::Texture m_texture;

	std::string m_name = "UNDEFINED";
};
