#include "EntityPlayer.h"

#include <Errors.h>

#include "World.h"
#include "Tile.h"
#include "NPCInventory.h"
#include "ArmourInventory.h"
#include "WeaponInventory.h"

#include "Factory.h"

ArmourAttackWrapper::ArmourAttackWrapper(std::string& UUID, std::shared_ptr<NPCInventory> inventory) {
	// Construct all of our inventories, as well as the GUI

	// Construct inventories:
	m_inventory = inventory; // Done

	std::string armourName = UUID + "PlayerArmourGrid";
	std::string attacksName = UUID + "PlayerAttacksGrid";
	std::string frameName = UUID + "PlayerInventoryFrame";

	Factory::getGUI()->setActiveContext(1);

	m_window = static_cast<CEGUI::FrameWindow*>(Factory::getGUI()->createWidget("FOTDSkin/FrameWindow", glm::vec4(0.0f, 0.0f, 0.65f, 1.0f), glm::vec4(0.0f), frameName));
	m_window->setCloseButtonEnabled(false);
	m_window->setDragMovingEnabled(true);
	m_window->setRollupEnabled(false);
	m_window->setSizingEnabled(false);
	m_window->getTitlebar()->setText("Armour & Attacks");

	Factory::getGUI()->setActiveContext(0);

	m_armourGrid = std::make_unique<ArmourInventory>(armourName, false, true, m_window);
	m_armourGrid->init();
	m_attacksGrid = std::make_unique<WeaponInventory>(attacksName, false, true, m_window);
	m_attacksGrid->init();

	m_window->setVisible(false);
	m_window->setEnabled(false);
	m_armourGrid->setToDraw(true);
	m_attacksGrid->setToDraw(true);

	m_armourGrid->setMovable(false);
	m_attacksGrid->setMovable(false);

	glm::vec4 armourPos(0.1f, 0.1f, INVENTORY_BOX_WIDTH, INVENTORY_BOX_HEIGHT * 3); // We need to offset by half dims. 0.0, 0.0 is the centre of the screen for some reason. I'm too lazy to fix it.
	glm::vec4 attacksPos(0.1f, 0.8f, INVENTORY_BOX_WIDTH * 3, INVENTORY_BOX_HEIGHT);

	m_armourGrid->setDestRect(armourPos);
	m_attacksGrid->setDestRect(attacksPos);

	m_armourGrid->setContentSize(1, 3); // vertical
	m_attacksGrid->setContentSize(3, 1); // horizontal
}

ArmourAttackWrapper::~ArmourAttackWrapper() {

}

void ArmourAttackWrapper::setToDraw(bool& setting) {
	if(setting == true) { // Make sure the inventory is in the right position before opening
		glm::vec4 inventoryPos(0.65f, 0.25f, 0.35f, 0.5f);
		m_inventory->setDestRect(inventoryPos);
		m_inventory->setMovable(false);
	} else {
		glm::vec4 inventoryPos(-0.2f, -0.2f, 0.35f, 0.5f);
		m_inventory->setDestRect(inventoryPos);
		m_inventory->setMovable(true);
	}

	m_window->setVisible(setting);
	m_window->setEnabled(setting);
	//m_armourGrid->setToDraw(setting);
	//m_attacksGrid->setToDraw(setting);
	m_inventory->setToDraw(setting);
}

void ArmourAttackWrapper::draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, float x, float y) {
	// This draws our armourGrid, attacksGrid, and inventory.
	m_inventory->draw(sb, sf, x, y, false);
	m_armourGrid->draw(sb, sf, x, y, false);
	m_attacksGrid->draw(sb, sf, x, y, false);
}

void ArmourAttackWrapper::update() {
	m_inventory->update();
	m_armourGrid->update();
	m_attacksGrid->update();
}


EntityPlayer::EntityPlayer(glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data, bool loadTex) : EntityNPC(pos, layer, 0, data, loadTex) {
	m_inventory = std::make_shared<NPCInventory>(15.0f, m_UUID, true); // This makes sure that the player has an inventory with GUI
	m_inventory->init();

	m_armourAttackWrapper = new ArmourAttackWrapper(m_UUID, m_inventory);
}


EntityPlayer::~EntityPlayer() {
	//dtor
}

void EntityPlayer::initGUI() {
	m_statusBoxFrame = static_cast<CEGUI::PopupMenu*>(Factory::getGUI()->createWidget("FOTDSkin/StatusBox", glm::vec4(0.785f, 0.025f, 0.2f, 0.4f), glm::vec4(0.0f), "PlayerGUI_StatusBox"));
	m_statusBoxFrame->openPopupMenu();

	m_statusBoxLabel = static_cast<CEGUI::DefaultWindow*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/Label", glm::vec4(0.075f, 0.025f, 0.925f, 1.0f), glm::vec4(0.0f), "PlayerGUI_StatusBox_Label"));
	m_statusBoxLabel->setProperty("HorzFormatting", "LeftAligned");
	m_statusBoxLabel->setProperty("VertFormatting", "TopAligned");

	std::string labelText = "[padding='l:0 t:0 r:0 b:-1']Time of day and date\n";
	labelText +=            "[padding='l:4 t:-7 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/SanityIcon']\n";
	labelText +=            "[padding='l:4 t:-8 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/HealthIcon']\n";
	labelText +=            "[padding='l:4 t:-7 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/ThirstIcon']\n";
	labelText +=            "[padding='l:4 t:-5 r:0 b:2'][image-size='w:25 h:25'][vert-alignment='centre'][image='FOTDSkin/HungerIcon']\n";
	labelText +=            "[padding='l:4 t:-8 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/ExhaustionIcon']\n";
	labelText +=            "[padding='l:4 t:-6 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/StaminaIcon']\n";

	m_statusBoxLabel->setText(labelText);

	m_sanityBar     = static_cast<CEGUI::ProgressBar*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/SanityBar",     glm::vec4(0.25f, 0.10f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Sanity"));
	m_healthBar     = static_cast<CEGUI::ProgressBar*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/HealthBar",     glm::vec4(0.25f, 0.23f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Health"));
	m_thirstBar     = static_cast<CEGUI::ProgressBar*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/ThirstBar",     glm::vec4(0.25f, 0.36f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Thirst"));
	m_hungerBar     = static_cast<CEGUI::ProgressBar*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/HungerBar",     glm::vec4(0.25f, 0.50f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Hunger"));
	m_exhaustionBar = static_cast<CEGUI::ProgressBar*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/ExhaustionBar", glm::vec4(0.25f, 0.63f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Exhaustion"));
	m_staminaBar    = static_cast<CEGUI::ProgressBar*>(Factory::getGUI()->createWidget(m_statusBoxFrame, "FOTDSkin/StaminaBar",    glm::vec4(0.25f, 0.76f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Stamina"));

	m_sanityBar->setProgress(1.0f);
	m_healthBar->setProgress(1.0f);
	m_thirstBar->setProgress(1.0f);
	m_hungerBar->setProgress(1.0f);
	m_exhaustionBar->setProgress(1.0f);
	m_staminaBar->setProgress(1.0f);

	m_sanityBar->setStepSize(0.0000005d);
	m_healthBar->setStepSize(0.0000005d);
	m_thirstBar->setStepSize(0.0000005d);
	m_hungerBar->setStepSize(0.0000005d);
	m_exhaustionBar->setStepSize(0.0000005d);
	m_staminaBar->setStepSize(0.0000005d);


	m_buffBoxFrame = static_cast<CEGUI::PopupMenu*>(Factory::getGUI()->createWidget("FOTDSkin/StatusBox", glm::vec4(0.725f, 0.025f, 0.055f, 0.4f), glm::vec4(0.0f), "PlayerGUI_BuffBox"));
	m_buffBoxFrame->openPopupMenu();
}

void EntityPlayer::onDraw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {
	if(m_selectedEntity) {
		glm::vec4 fullUV(0.0f, 0.0f, 1.0f, 1.0f);
		int cursorImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Cursor.png").id;

		glm::vec4 cursorDestRect(m_selectedEntity->getPosition().x + xOffset, m_selectedEntity->getPosition().y, m_selectedEntity->getSize().x, m_selectedEntity->getSize().y);
		sb.draw(cursorDestRect, fullUV, cursorImgId, 1.5f * (WORLD_DEPTH - m_layer), GLEngine::ColourRGBA8(255, 255, 255, 255));
	} else if(m_selectedBlock) { // Cursor box selection
		glm::vec4 fullUV(0.0f, 0.0f, 1.0f, 1.0f);
		int cursorImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Cursor.png").id;

		glm::vec4 cursorDestRect(m_selectedBlock->getPosition().x + xOffset, m_selectedBlock->getPosition().y, m_selectedBlock->getSize().x, m_selectedBlock->getSize().y);
		sb.draw(cursorDestRect, fullUV, cursorImgId, 1.5f * (WORLD_DEPTH - m_layer), GLEngine::ColourRGBA8(255, 255, 255, 255));
	}
}

void EntityPlayer::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {
	glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

	sb.begin();

	/*{
		// Hotbar
		int hotbarImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Hotbar.png").id;
		int hotbarSelectImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/HotbarSelection.png").id;

		for(int i = 0; i < HOTBAR_BOX_NUM; i++) {

			glm::vec4 uv(i * (1.0 / HOTBAR_BOX_NUM), 0.0f, (1.0 / HOTBAR_BOX_NUM), 1.0f);
			glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
			sb.draw(destRect, uv, hotbarImgId, 0.0f, fullColour);

			{
				if(m_favouriteItems[i]) {
					glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
					                   HOTBAR_BOX_SIZE / 4,
					                   HOTBAR_BOX_SIZE,
					                   HOTBAR_BOX_SIZE);
					glm::vec4 itemUV(0, 0, 1, 1);
					int itemImgId = m_favouriteItems[i]->getTextureId();//GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_favouriteItems[i]->getID()].texturePath).id;

					sb.draw(destRect, itemUV, itemImgId, 0.4f, GLEngine::ColourRGBA8(255, 255, 255, 255));
				}
			}
		}

		for(int i = 0; i < HOTBAR_BOX_NUM; i++) {
			if(m_favouriteItems[i]) {
				glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
				                   HOTBAR_BOX_SIZE / 4,
				                   HOTBAR_BOX_SIZE,
				                   HOTBAR_BOX_SIZE);
				glm::vec4 itemUV(0, 0, 1, 1);
				int itemImgId = m_favouriteItems[i]->getTextureId();//GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_favouriteItems[i]->getID()].texturePath).id;

				sb.draw(destRect, itemUV, itemImgId, 0.4f, GLEngine::ColourRGBA8(255, 255, 255, 255));

				sf.draw(sb, std::to_string(m_favouriteItems[i]->getQuantity()).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE * 9 / 10, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 1.0f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);
			}
		}

		glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * m_selectedHotbox, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
		sb.draw(destRect, fullUV, hotbarSelectImgId, 1.1f, fullColour);
	} // Hotbar END*/


	if(m_bagOpen) {
		m_inventory->draw(sb, sf, m_position.x, m_position.y);
	} else if(m_inventoryOpen) {
		m_armourAttackWrapper->draw(sb, sf, m_position.x, m_position.y);
	}

	sb.end();
	sb.renderBatch();
}

#define cap(x) if(x > 1) x = 1; if(x < 0) x = 0;

void EntityPlayer::updateStats(float timeStep) {
	m_sanityBar->setProgress(m_sanity);
	m_healthBar->setProgress(m_health);
	m_thirstBar->setProgress(m_thirst);
	m_hungerBar->setProgress(m_hunger);
	m_exhaustionBar->setProgress(m_exhaustion);
	m_staminaBar->setProgress(m_stamina);

	cap(m_sanity);
	cap(m_health);
	cap(m_thirst);
	cap(m_hunger);
	cap(m_exhaustion);
	cap(m_stamina);

	m_thirst -= 0.00006f * timeStep;
	m_hunger -= 0.00003f * timeStep;
	m_exhaustion -= 0.0001f * timeStep;

	if(m_stamina < 0.8f) {
		m_exhaustion -= 0.0002f * timeStep;
	}

	if(m_exhaustion < 0.25f) {
		m_sanity -= 0.0001f * timeStep;
	}
	/*if((m_cornerLight.x + m_cornerLight.y + m_cornerLight.z + m_cornerLight.w) / 4.0f < 0.25f) {
	    if(m_exposedToSun) {
	        Buff_Increase_Sanity_StarLight* potentialBuff = new Buff_Increase_Sanity_StarLight(&m_sanity);
	        if(std::find(m_buffs.begin(), m_buffs.end(), potentialBuff) == m_buffs.end() || m_buffs.size() == 0) {
	            m_buffs.push_back(potentialBuff);
	        } else {
	            delete potentialBuff;
	        }
	    }
	}
	if((m_cornerLight.x + m_cornerLight.y + m_cornerLight.z + m_cornerLight.w) / 4.0f < 0.25f) {
	    Buff_Decrease_Sanity_LowLight* potentialBuff = new Buff_Decrease_Sanity_LowLight(&m_sanity);
	    if(std::find(m_buffs.begin(), m_buffs.end(), potentialBuff) == m_buffs.end() || m_buffs.size() == 0) {
	        m_buffs.push_back(potentialBuff);
	    } else {
	        delete potentialBuff;
	    }
	}

	for(unsigned int i = 0; i < m_buffs.size(); i++) {
	    m_buffs[i]->update();
	}

	for(unsigned int i = 0; i < m_buffs.size(); i++) {
	    if(!m_buffs[i]->isActive()) {
	        for(int j = i; j < m_buffs.size()-1; j++) {
	            m_buffs[j] = m_buffs[j+1];
	        }
	        m_buffs.pop_back();
	    }
	}*/
}

void EntityPlayer::updateMouse(glm::vec2 mouseCoords) {

	m_aimingDirection = glm::normalize(mouseCoords - (m_position + m_size / glm::vec2(2.0f)));

	if(m_canInteract) {
		m_selectedBlock = nullptr;

		if((int)mouseCoords.x > -WORLD_SIZE) {
			if(world->getTile((int)mouseCoords.x, mouseCoords.y, m_layer)) {
				m_selectedBlock = world->getTile((int)mouseCoords.x, mouseCoords.y, m_layer);
			}
		}

		//Logger::getInstance()->log("Clicked: " + std::to_string(m_selectedBlock->getPosition().x));

		m_selectedEntity = nullptr;

		for(unsigned int i = 0; i < Factory::getEntityManager()->getEntities().size() && !m_selectedEntity; i++) {
			float sizeX = (Factory::getEntityManager()->getEntities()[i]->getSize().x) / 2.0f;
			float midX = Factory::getEntityManager()->getEntities()[i]->getPosition().x + sizeX;

			float sizeY = (Factory::getEntityManager()->getEntities()[i]->getSize().y) / 2.0f;
			float midY = Factory::getEntityManager()->getEntities()[i]->getPosition().y + sizeY;

			if(std::abs(midX - mouseCoords.x) <= sizeX / 2.0f) {
				if(std::abs(midY - mouseCoords.y) <= sizeY / 2.0f) {
					m_selectedEntity = Factory::getEntityManager()->getEntities()[i];
				}
			}
		}
	}

}

void EntityPlayer::updateInput(GLEngine::InputManager* input) {

	/// THESE ARE PURELY FOR DEVELOPMENT
	if(input->isKeyPressed(SDLK_t)) {
		m_inventory->addItem(new Item(1, (unsigned int)ItemIDs::BLOCK_TORCH, true));
	}

	if(input->isKeyDown(SDLK_w) && m_stamina > 0.0f) {
		if(m_onGround) {
			m_velocity.y = m_jumpHeight; // y=(jumpHeight*TILE_SIZE+3/4*TILE_SIZE+-5.88*x^2)  initial jump power is the absolute of the x when y=0. jumpheight is in eights of tiles and you must add 4
			m_onGround = false;
			m_stamina -= 0.005f;
		}
	}
	if(input->isKeyDown(SDLK_s)) {
		m_velocity.y -= 0.1f;
	}

	if(input->isKeyPressed(SDLK_e)) {
		moveDownLayer();
	}
	if(input->isKeyPressed(SDLK_q)) {
		moveUpLayer();
	}
	m_layer = (m_layer < 0) ? 0 : (m_layer >= WORLD_DEPTH) ? WORLD_DEPTH - 1 : m_layer;

	if(input->isKeyDown(SDLK_d) && (m_stamina > 0.0f)) {
		if(m_velocity.x < 0.0f) m_velocity.x /= 5.0f;
		if(m_velocity.x < MAX_SPEED) {
			m_velocity.x += m_runSpeed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
		}
		if(m_state != MovementState::LOW_VEL) {
			m_state = MovementState::LOW_VEL;
			m_body.changeAnimation(m_lowVelAnimation);
		}
		m_stamina *= 0.999f;
	} else if(input->isKeyDown(SDLK_a) && (m_stamina > 0.0f)) {
		if(m_velocity.x > 0.0f) m_velocity.x /= 5.0f;
		if(m_velocity.x > -MAX_SPEED)
			m_velocity.x -= m_runSpeed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
		if(m_state != MovementState::LOW_VEL) {
			m_state = MovementState::LOW_VEL;
			m_body.changeAnimation(m_lowVelAnimation);
		}
		m_stamina *= 0.999f;
	}

	if(m_velocity.x < 0.001f && m_velocity.x > -0.001f && m_onGround && m_stamina * 1.003f <= 1.000000f) {
		m_stamina *= 1.003f;
		if(m_state != MovementState::IDLE) {
			m_state = MovementState::IDLE;
			m_body.changeAnimation(m_idleAnimation);
		}
	}

	if(canInteract()) {
		if(input->isKeyPressed(SDLK_e)) {
			if(m_selectedEntity) { // must hover mouse over entity and press 'e'
				if(m_selectedEntity->getType() == XMLModule::EntityType::NPC) {
					dynamic_cast<EntityNPC*>(m_selectedEntity)->onTalk();
				}
			}
		} else if(input->isKeyPressed(SDLK_t)) {
			if(m_selectedEntity) {
				if(m_selectedEntity->getType() == XMLModule::EntityType::NPC) {
					dynamic_cast<EntityNPC*>(m_selectedEntity)->onTrade();
				}
			}
		}

		if(input->isKeyPressed(SDL_BUTTON_LEFT) && m_selectedBlock) {
			//if(m_favouriteItems[m_selectedHotbox]) m_favouriteItems[m_selectedHotbox]->onLeftClick(m_selectedBlock, world);
			/*if(!m_favouriteItems[m_selectedHotbox]) {
				Tile* t = new Tile(m_selectedBlock->getPosition(), m_selectedBlock->getLayer(), 0, SaveDataTypes::MetaData(), false);
				world->setTile(t);
			}*/
			//if(m_inventory) m_inventory->updateWeight();


			activateAttack(0);
		} else if(input->isKeyPressed(SDL_BUTTON_RIGHT)) {
			if(m_selectedBlock) {
				/*if(m_favouriteItems[m_selectedHotbox]) {
					m_favouriteItems[m_selectedHotbox]->onRightClick(m_selectedBlock);
				}*/
				m_inventory->updateWeight();
				if(m_selectedBlock->getID() != (unsigned int)TileIDs::AIR) {
					m_selectedBlock->onInteract_RightClicked();
				}
			}
		}
	}

	// The inventory and bag should not be opened at the same time. One or the other.
	if(input->isKeyPressed(SDLK_b) && !m_inventoryOpen) { // Open bag
		m_bagOpen = !m_bagOpen;
		m_inventory->setToDraw(m_bagOpen);
	}
	if(input->isKeyPressed(SDLK_i) && !m_bagOpen) { // open armour/attack menu
		m_statusBoxFrame->setVisible(m_inventoryOpen);
		m_statusBoxFrame->setEnabled(m_inventoryOpen);
		m_buffBoxFrame->setVisible(m_inventoryOpen);
		m_buffBoxFrame->setEnabled(m_inventoryOpen);
		m_inventoryOpen = !m_inventoryOpen;
		m_armourAttackWrapper->setToDraw(m_inventoryOpen);
	}

	for(unsigned int i = 1; i < 9; i++) { // Just 1-9
		if(input->isKeyPressed(SDLK_0 + i)) {
			m_selectedHotbox = i - 1;
			//m_favouriteItems[i-1] = m_inventory->getItem(i-1);
		}
	}
}

SaveDataTypes::EntityPlayerData EntityPlayer::getPlayerSaveData() {
	SaveDataTypes::EntityPlayerData ret;
	ret.id = m_id;
	ret.velocity = m_velocity;
	ret.position = m_position;
	ret.layer = m_layer;
	ret.md = m_metaData;

	ret.health = m_health;
	ret.inventory = m_inventory->getInventorySaveData();

	ret.sanity = m_sanity;
	ret.thirst = m_thirst;
	ret.hunger = m_hunger;
	ret.exhaustion = m_exhaustion;
	ret.stamina = m_stamina;

	return ret;
}
