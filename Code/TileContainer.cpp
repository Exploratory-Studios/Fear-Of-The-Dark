#include "TileContainer.h"

TileContainer::TileContainer(glm::vec2 pos, unsigned int layer, unsigned int id, SaveDataTypes::MetaData data, bool loadTex) : Tile(pos, layer, id, data, loadTex) {
	init(); // The parent constructor sets m_id
}

TileContainer::~TileContainer() {
}

void TileContainer::init() {
	XMLModule::TileContainerData d = XMLModule::XMLData::getTileContainerData(m_id);

	m_maxWeight = d.maxWeight;
}
