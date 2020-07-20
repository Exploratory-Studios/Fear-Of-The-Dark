#pragma once

#include "Tile.h"

class TileContainer : public Tile {
	public:
		TileContainer(unsigned int id);
		~TileContainer();

		virtual void onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth);

	protected:
		Inventory* m_inventory = nullptr;
		float m_maxWeight;

		bool m_opened = false;

	private:
		void init();

};
