#include "ItemWeapon.h"

ItemWeapon::ItemWeapon(short unsigned int quantity, unsigned int id, bool loadTex) : Item(quantity, id, loadTex) {
	init();
}

ItemWeapon::~ItemWeapon() {
}

void ItemWeapon::init() {
	// Inits XML
	XMLModule::ItemWeaponData d = XMLModule::XMLData::getItemWeaponData(m_id);
	m_attackID = d.attackID;
}

unsigned int ItemWeapon::getAttackID() {
	return m_attackID;
}
