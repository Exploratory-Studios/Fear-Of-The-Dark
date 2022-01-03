#include "ItemWeapon.h"

#include <XMLDataManager.hpp>

#include "CustomXMLTypes.h"

ItemWeapon::ItemWeapon(short unsigned int quantity, unsigned int id) : Item(quantity, id) {
	init();
}

ItemWeapon::~ItemWeapon() {
}

void ItemWeapon::init() {
	// Inits XML
	XMLModule::ItemWeaponData d = getItemWeaponData(m_id);
	m_attackID					= d.attackID;
}

unsigned int ItemWeapon::getAttackID() {
	return m_attackID;
}
