#include "Inventory.h"

#include <string>
#include <math.h>

Inventory::Inventory() {

}

bool Inventory::addItem(Item newItem, int x, int y) {
    m_weight += newItem.m_weight * newItem.m_quantity;

    if(x == -1 && y == -1) {
        for(unsigned int i = 0; i < INVENTORY_BOX_NUM_Y; i++) {
            for(unsigned int j = 0; j < INVENTORY_BOX_NUM_X; j++) {
                if(m_items[i][j].m_id == newItem.m_id) {
                    m_items[i][j].m_quantity += newItem.m_quantity;
                    return true;
                }
            }
        }
        for(unsigned int i = 0; i < INVENTORY_BOX_NUM_Y; i++) {
            for(unsigned int j = 0; j < INVENTORY_BOX_NUM_X; j++) {
                if(m_items[i][j].getID() == -1) {
                    m_items[i][j] = newItem;
                    return true;
                }
            }
        }
    }
    return false;
}

void Inventory::updateWeight() { // Only use if something needs verification or is bugged
    m_weight = 0;
    for(int i = 0; i < INVENTORY_BOX_NUM_Y; i++) {
        for(int j = 0; j < INVENTORY_BOX_NUM_X; j++) {
            if(m_items[i][j].getID() != -1) {
                m_weight += m_items[i][j].m_weight * m_items[i][j].m_quantity;
            }
        }
    }
}

float Inventory::getCurrentWeight() {
    return m_weight;
}

float Inventory::getSpeedMultiplier() {
    // y = -x^4 + 1; Gives us a nice exponential (downwards) curve (from 0,1 to 1,0), where x = m_weight / m_maxWeight and y = the multiplier
    if(m_weight > 0.0f) {
        float multiplier = -std::pow(m_weight / MAX_WEIGHT, 4) + 1.0f;

        if(multiplier < 0.0f) multiplier = 0.0f;
        if(std::isnan(multiplier)) multiplier = 1.0f;

        return multiplier;
    } else {
        return 1.0f;
    }
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

            if(m_items[i][j].m_quantity != 0 && m_items[i][j].m_id != -1) { // If quantity is 0, it has not been initialized
                glm::vec4 itemUV(0, 0, 1, 1);

                int itemImgId = GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_items[i][j].m_id].texturePath).id;

                sb.begin();

                sb.draw(destRect, itemUV, itemImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));

                sb.end();
                sb.renderBatch();

                sb.begin();

                sf.draw(sb, std::to_string(m_items[i][j].m_quantity).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);

                sb.end();
                sb.renderBatch();

            }


        }
    }
}
