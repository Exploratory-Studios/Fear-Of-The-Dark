#pragma once

#include "Tile.h"

class TileInventory;

class TileContainer : public Tile {
	public:
		TileContainer(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
		~TileContainer();

		virtual void onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth);

	protected:
		TileInventory* m_inventory = nullptr;

		bool m_opened = false;

	private:
		void init();

};
