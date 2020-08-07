#pragma once

class Entity;
class Tile;
class Item;

class Factory {
	public:
		static Entity* createEntity();
		static Tile* createTile();
		static Item* createItem();

};
