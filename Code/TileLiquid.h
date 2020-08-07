#pragma once

#include "Tile.h"

class TileLiquid : public Tile {
	public:
		TileLiquid(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex);
		~TileLiquid();

		float getLiquidLevel() {
			return m_level;
		}

		virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int& xOffset, int& depthDifference) override;
		virtual void drawNormal(GLEngine::SpriteBatch& sb, int& xOffset, int& depthDifference) override;

	protected:
		virtual void onUpdate() override;

		float m_level = 1.0f; // The liquid's level (percentage) (0.0-1.0)

};
