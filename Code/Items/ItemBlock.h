#pragma once

#include "Item.h"

class ItemBlock : public Item {
	/* Responsibilities:
		 *  - Hold a block ID
		 *  - Have a 'placeBlock()' function, called from the default 'onRightClick()' function
		 */

  public:
	ItemBlock(short unsigned int quantity, unsigned int id, bool loadTex);
	~ItemBlock(); // destroy m_block

	void init(); // IMPORTANT: Also creates a new Tile pointer (m_block)

	virtual void onRightClick(Tile* selectedBlock) override;

	void setTileToPlace(Tile& tile); // just copies the data into a Tile pointer.

  private:
	void placeBlock(Tile* selected);

	Tile* m_block = nullptr; // Block to be placed
							 /* Why not just an ID? You may ask.
		 *
		 * Well.
		 *
		 * If there's a block that needs certain properties set (see: MetaData, durability, inventory, etc.),
		 * then just simply storing an ID won't cut it because that'll just create an all new tile. Instead,
		 * we copy the actual data into a new Tile pointer, and recreate it when placed.
		 */
};
