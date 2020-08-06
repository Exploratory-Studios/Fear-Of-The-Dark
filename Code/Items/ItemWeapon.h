#pragma once

#include "Item.h"

class ItemWeapon : public Item {
		/*
		 * Class Responsiblities:
		 *  - Hold an attack ID
		 *  - Have a getter for the attack ID
		 */
	public:
		ItemWeapon(short unsigned int quantity, unsigned int id, bool loadTex);
		virtual ~ItemWeapon();

		void init(); // Inits XML

		unsigned int getAttackID();

	protected:
		unsigned int m_attackID;

};
