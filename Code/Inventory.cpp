#include "Inventory.h"

#include "PresetValues.h"

#include <string>

Inventory::Inventory() {

}

void Inventory::addItem(Item newItem) {
    m_weight += newItem.m_weight * newItem.m_quantity;

    for(unsigned int i = 0; i < m_items.size(); i++) {
        if(m_items[i].m_id == newItem.m_id) {
            m_items[i].m_quantity += newItem.m_quantity;
            return;
        }
    }
    m_items.push_back(newItem);
}

void Inventory::updateWeight() { // Only use if something needs verification or is bugged
    m_weight = 0;
    for(int i = 0; i < m_items.size(); i++) {
        m_weight += m_items[i].m_weight * m_items[i].m_quantity;
    }
}

float Inventory::getCurrentWeight() {
    return m_weight;
}

float Inventory::getSpeedMultiplier() {
    // y = -x^4 + 1; Gives us a nice exponential (downwards) curve (from 0,1 to 1,0), where x = m_weight / m_maxWeight and y = the multiplier
    float multiplier = -std::pow(m_weight / MAX_WEIGHT, 4) + 1.0f;

    if(multiplier < 0.0f) multiplier = 0.0f;

    return multiplier;
}

void Inventory::draw(float x, float y, GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {
    for(int i = 0; i < INVENTORY_BOX_NUM_Y; i++) {
        for(int j = 0; j < INVENTORY_BOX_NUM_X; j++) {

            glm::vec4 destRect(x + j * (INVENTORY_BOX_SIZE + INVENTORY_BOX_PADDING),
                               y + i * (INVENTORY_BOX_SIZE + INVENTORY_BOX_PADDING),
                               INVENTORY_BOX_SIZE,
                               INVENTORY_BOX_SIZE);

            float uvXSize = 1.0f / (float)INVENTORY_BOX_NUM_X;
            float uvYSize = 1.0f / (float)INVENTORY_BOX_NUM_Y;

            float uvX = uvXSize * (float)j;
            float uvY = uvYSize * (float)i;

            glm::vec4 uv(uvX, uvY, uvXSize, uvYSize); // xy beginDest, xy endDest

            int inventoryImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/Inventory.png").id;

            sb.begin();

            sb.draw(destRect, uv, inventoryImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));

            sb.end();
            sb.renderBatch();

            if(m_items.size() > i*INVENTORY_BOX_NUM_X+j) { // Only show boxes if they are filled
                if(m_items[i*INVENTORY_BOX_NUM_X+j].m_quantity != 0) { // If quantity is 0, it has not been initialized
                    glm::vec4 itemUV(0, 0, 1, 1);

                    int itemImgId = GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_items[i*INVENTORY_BOX_NUM_X+j].m_id].texturePath).id;

                    sb.begin();

                    sb.draw(destRect, itemUV, itemImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));

                    sb.end();
                    sb.renderBatch();

                    sb.begin();

                    sf.draw(sb, std::to_string(m_items[i*INVENTORY_BOX_NUM_X+j].m_quantity).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);

                    sb.end();
                    sb.renderBatch();

                }
            }

        }
    }
}
