#pragma once

#include "Tile.h"

class TileLiquid : public Tile {
  public:
	TileLiquid(glm::vec2 pos, unsigned int layer, unsigned int id);
	~TileLiquid();

	float getLiquidLevel() {
		return m_level;
	}

	virtual void draw(BARE2D::BumpyRenderer* renderer,
					  BARE2D::FontRenderer*	 fontRenderer,
					  int&					 xOffset,
					  int&					 depthDifference) override;

  protected:
	virtual void onUpdate() override;

	float m_level = 1.0f; // The liquid's level (percentage) (0.0-1.0)
};
