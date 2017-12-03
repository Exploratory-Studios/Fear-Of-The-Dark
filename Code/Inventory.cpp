#include "Inventory.h"

#include "PresetValues.h"

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

void Inventory::updateWeight() {
    m_weight = 0;
    for(int i = 0; i < m_items.size(); i++) {
        m_weight += m_items[i].m_weight * m_items[i].m_quantity;
    }
}

short unsigned int Inventory::getCurrentWeight() {
    return m_weight;
}

float Inventory::getSpeedMultiplier() {
    // y = -x^3 + 1; Gives us a nice exponential (downwards) curve (from 0,1 to 1,0), where x = m_weight / m_maxWeight and y = the multiplier
    float multiplier = std::pow(-(m_weight / m_maxWeight), WEIGHT_EFFECT) + 1.0f;
    return multiplier;
}
