#pragma once

#include "Tile.h"

class TileInventory;

class TileContainer : public Tile {
  public:
	TileContainer(glm::vec2 pos, unsigned int layer, unsigned int id);
	~TileContainer();

	virtual void drawGUI(BARE2D::BasicRenderer* renderer, BARE2D::FontRenderer* fontRenderer, int& xOffset) override;
	virtual void destroy() override;

  protected:
	virtual void setToDraw(bool toDraw);

	virtual void onUpdate() override;

	TileInventory* m_inventory = nullptr;

	bool m_opened = false;

	bool m_initedGUI = false;

  private:
	void init();
};
