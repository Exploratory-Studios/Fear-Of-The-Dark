#pragma once

#include <glm/glm.hpp>
#include <GLTexture.h>
#include <ResourceManager.h>
#include <string>
#include <vector>

#include "ScriptQueue.h"

#include "SaveDataTypes.h"

class World;
class Tile;
namespace SaveDataTypes {
	class MetaData;
}

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
	Item(short unsigned int quantity, unsigned int id, bool loadTex);
	virtual ~Item() {
	}

	void init();

	virtual void onRightClick(Tile* selectedBlock);

	std::vector<ScriptingModule::Argument> generateLuaData() {
		std::vector<ScriptingModule::Argument> args = {{"itemID", std::to_string(m_id)},
													   {"itemQuantity", std::to_string(m_quantity)}};

		m_metaData.getLuaArguments(args);

		return args;
	}

	void addToQuantity(int amnt) {
		m_quantity += amnt;
	}

	unsigned int getID() const {
		return m_id; // (unsigned int)(-1) is equivalent to null
	}
	unsigned int getTextureId() const {
		return m_textureId;
	}
	short unsigned int getQuantity() const {
		return m_quantity;
	}
	std::string& getName() {
		return m_name;
	}
	SaveDataTypes::MetaData getMetaData() const {
		return m_metaData;
	}
	float getWeight() const {
		return m_weight;
	}

	void loadTexture() {
		m_textureId = GLEngine::ResourceManager::getTexture(m_texturePath).id;
	}

	SaveDataTypes::ItemData getItemSaveData();

  protected:
	unsigned int m_id = (unsigned int)-1; // Block/Non-block id

	float			   m_weight	  = 0.0f; // How much it weighs in the inventory (kgs)
	short unsigned int m_quantity = 0;	  // How much you have

	int m_useScriptId = -1;

	unsigned int m_textureId = (unsigned int)-1;
	std::string	 m_texturePath;

	std::string m_name = "UNDEFINED";

	SaveDataTypes::MetaData m_metaData;
};
