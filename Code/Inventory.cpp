#include "Inventory.h"

#include <string>
#include <math.h>

Inventory::Inventory() {
	
}

bool Inventory::addItem(Item* newItem) {
	m_weight += newItem->m_weight * newItem->m_quantity;

	for(unsigned int i = 0; i < m_items.size(); i++) {
		if(m_items[i]->m_id == newItem->m_id) {
			m_items[i]->m_quantity += newItem->m_quantity;
			return true;
		}
	}

	m_items.push_back(newItem);
	return false;
}

void Inventory::subtractItem(Item* item) {
	getItem(getItemIndex(item))->m_quantity -= item->getQuantity();
}

void Inventory::updateWeight() { // Only use if something needs verification or is bugged
	m_weight = 0;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		m_weight += m_items[i]->m_weight * m_items[i]->m_quantity;
	}
}

void Inventory::update() {
	for(unsigned int i = 0; i < m_items.size(); i++) {
		if(m_items[i]->getQuantity() <= 0) {
			for(unsigned int j = i + 1; j < m_items.size(); j++) {
				m_items[i] = m_items[j];
			}
			m_items.pop_back();
		}
	}
}

float Inventory::getCurrentWeight() {
	return m_weight;
}

float Inventory::getSpeedMultiplier() {
	// y = -x^4 + 1; Gives us a nice exponential curve (from 0,1 to 1,0), where x = m_weight / m_maxWeight and y = the multiplier
	if(m_weight > 0.0f) {
		float multiplier = -std::pow(m_weight / MAX_WEIGHT, 4) + 1.0f;

		if(multiplier < 0.0f) multiplier = 0.0f;
		if(std::isnan(multiplier)) multiplier = 1.0f;

		return multiplier;
	}
	return 1.0f;
}

void Inventory::draw(float x, float y, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {
	for(int i = 0; i < INVENTORY_BOX_NUM_Y * INVENTORY_BOX_NUM_X + (int)(m_items.size() / INVENTORY_BOX_NUM_X) * INVENTORY_BOX_NUM_X; i++) {

		glm::vec4 destRect(x + (i % INVENTORY_BOX_NUM_X) * (INVENTORY_BOX_SIZE + INVENTORY_BOX_PADDING),
		                   y + (int)(i / INVENTORY_BOX_NUM_X) * (INVENTORY_BOX_SIZE + INVENTORY_BOX_PADDING),
		                   INVENTORY_BOX_SIZE,
		                   INVENTORY_BOX_SIZE);

		float uvXSize = 1.0f / (float)INVENTORY_BOX_NUM_X;
		float uvYSize = 1.0f / (float)INVENTORY_BOX_NUM_Y;

		float uvX = uvXSize * (int)(i % INVENTORY_BOX_NUM_X);
		float uvY = uvYSize * (int)(i / INVENTORY_BOX_NUM_X);

		glm::vec4 uv(uvX, uvY, uvXSize, uvYSize); // xy beginDest, xy endDest

		int inventoryImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Inventory.png").id;
		int inventoryOverlayImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/InventoryOverlay.png").id;

		sb.draw(destRect, uv, inventoryImgId, 1.1f, GLEngine::ColourRGBA8(255, 255, 255, 255));
		sb.draw(destRect, uv, inventoryOverlayImgId, 0.95f, GLEngine::ColourRGBA8(255, 255, 255, 255));

		if(m_items.size() > (unsigned int)i) {
			if(m_items[i]) {
				glm::vec4 itemUV(0, 0, 1, 1);
				GLuint itemImgId = m_items[i]->getTextureId();
				sb.draw(destRect, itemUV, itemImgId, 1.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));

				sf.draw(sb, std::to_string(m_items[i]->m_quantity).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 1.05f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);
			}
		}
	}
}

SaveDataTypes::InventoryData Inventory::getInventorySaveData() {
	SaveDataTypes::InventoryData ret;
	for(unsigned int i = 0; i < m_items.size(); i++) {
		ret.itemData.push_back(m_items[i]->getItemSaveData());
	}
	return ret;
}
