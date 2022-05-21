#include "EntityPlayer.h"

#include <string>

#include <BAREErrors.hpp>

#include "World.h"
#include "Tile.h"

#include "Singletons.h"

#include "NPCInventory.h"
#include "ArmourInventory.h"
#include "WeaponInventory.h"
#include "EntityNPC.h"

#include "ItemBlock.h"

EntityPlayer::EntityPlayer(glm::vec2 pos, unsigned int layer) : EntityNPC(pos, layer, 0) {
	init();
}

EntityPlayer::~EntityPlayer() {
}

void EntityPlayer::init() {
	m_inventory = std::make_shared<NPCInventory>(15.0f, m_UUID);
	m_inventory->init(m_UUID + "_PLAYER_Inventory", false, nullptr);

	m_armourWeaponsInventory = std::make_shared<NPCInventoryWrapper>(m_UUID, m_inventory);
}

void EntityPlayer::init(SaveDataTypes::EntityPlayerData& data) {
	EntityNPC::init(data);

	init();

	m_position	 = data.position;
	m_layer		 = data.layer;
	m_velocity	 = data.velocity;
	m_id		 = data.id;
	m_health	 = data.health;
	m_sanity	 = data.sanity;
	m_hunger	 = data.hunger;
	m_exhaustion = data.exhaustion;
	m_stamina	 = data.stamina;
}

void EntityPlayer::initGUI() {
	m_armourWeaponsInventory->initGUI();
	m_inventory->initInventoryGUI();

	std::function<bool(const CEGUI::EventArgs&)> reskin = [=](const CEGUI::EventArgs& e) -> bool {
		this->reskinLimbs();
	};
	std::function<bool(const CEGUI::EventArgs&)> defaultSkin = [=](const CEGUI::EventArgs& e) -> bool {
		this->m_body.resetAnimations();
	};

	m_armourWeaponsInventory->m_armourGrid->subscribeEvent(CEGUI::Element::EventChildAdded,
														   CEGUI::Event::Subscriber(reskin));
	m_armourWeaponsInventory->m_armourGrid->subscribeEvent(CEGUI::Element::EventChildRemoved,
														   CEGUI::Event::Subscriber(defaultSkin));

	m_statusBoxFrame =
		static_cast<CEGUI::PopupMenu*>(Singletons::getGUI()->createWidget("FOTDSkin/StatusBox",
																		  glm::vec4(0.785f, 0.025f, 0.2f, 0.4f),
																		  glm::vec4(0.0f),
																		  nullptr,
																		  "PlayerGUI_StatusBox"));
	m_statusBoxFrame->openPopupMenu();

	m_statusBoxLabel =
		static_cast<CEGUI::DefaultWindow*>(Singletons::getGUI()->createWidget("FOTDSkin/Label",
																			  glm::vec4(0.075f, 0.025f, 0.925f, 1.0f),
																			  glm::vec4(0.0f),
																			  m_statusBoxFrame,
																			  "PlayerGUI_StatusBox_Label"));
	m_statusBoxLabel->setProperty("HorzFormatting", "LeftAligned");
	m_statusBoxLabel->setProperty("VertFormatting", "TopAligned");

	std::string labelText = "[padding='l:0 t:0 r:0 b:-1']Time of day and date\n";
	labelText +=
		"[padding='l:4 t:-7 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/SanityIcon']\n";
	labelText +=
		"[padding='l:4 t:-8 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/HealthIcon']\n";
	labelText +=
		"[padding='l:4 t:-7 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/ThirstIcon']\n";
	labelText +=
		"[padding='l:4 t:-5 r:0 b:2'][image-size='w:25 h:25'][vert-alignment='centre'][image='FOTDSkin/HungerIcon']\n";
	labelText +=
		"[padding='l:4 t:-8 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/ExhaustionIcon']\n";
	labelText +=
		"[padding='l:4 t:-6 r:0 b:2'][vert-alignment='centre'][image-size='w:25 h:25'][image='FOTDSkin/StaminaIcon']\n";

	m_statusBoxLabel->setText(labelText);

	m_sanityBar = static_cast<CEGUI::ProgressBar*>(
		Singletons::getGUI()->createWidget("FOTDSkin/SanityBar",
										   glm::vec4(0.25f, 0.10f + 0.066f, 0.65f, 0.1f),
										   glm::vec4(),
										   m_statusBoxFrame,
										   "PlayerGUI_StatusBox_Sanity"));
	m_healthBar = static_cast<CEGUI::ProgressBar*>(
		Singletons::getGUI()->createWidget("FOTDSkin/HealthBar",
										   glm::vec4(0.25f, 0.23f + 0.066f, 0.65f, 0.1f),
										   glm::vec4(),
										   m_statusBoxFrame,
										   "PlayerGUI_StatusBox_Health"));
	m_thirstBar = static_cast<CEGUI::ProgressBar*>(
		Singletons::getGUI()->createWidget("FOTDSkin/ThirstBar",
										   glm::vec4(0.25f, 0.36f + 0.066f, 0.65f, 0.1f),
										   glm::vec4(),
										   m_statusBoxFrame,
										   "PlayerGUI_StatusBox_Thirst"));
	m_hungerBar = static_cast<CEGUI::ProgressBar*>(
		Singletons::getGUI()->createWidget("FOTDSkin/HungerBar",
										   glm::vec4(0.25f, 0.50f + 0.066f, 0.65f, 0.1f),
										   glm::vec4(),
										   m_statusBoxFrame,
										   "PlayerGUI_StatusBox_Hunger"));
	m_exhaustionBar = static_cast<CEGUI::ProgressBar*>(
		Singletons::getGUI()->createWidget("FOTDSkin/ExhaustionBar",
										   glm::vec4(0.25f, 0.63f + 0.066f, 0.65f, 0.1f),
										   glm::vec4(),
										   m_statusBoxFrame,
										   "PlayerGUI_StatusBox_Exhaustion"));
	m_staminaBar = static_cast<CEGUI::ProgressBar*>(
		Singletons::getGUI()->createWidget("FOTDSkin/StaminaBar",
										   glm::vec4(0.25f, 0.76f + 0.066f, 0.65f, 0.1f),
										   glm::vec4(),
										   m_statusBoxFrame,
										   "PlayerGUI_StatusBox_Stamina"));

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

	m_buffBoxFrame =
		static_cast<CEGUI::PopupMenu*>(Singletons::getGUI()->createWidget("FOTDSkin/StatusBox",
																		  glm::vec4(0.725f, 0.025f, 0.055f, 0.4f),
																		  glm::vec4(0.0f),
																		  nullptr,
																		  "PlayerGUI_BuffBox"));
	m_buffBoxFrame->openPopupMenu();

	m_initedGUI = true;
}

void EntityPlayer::onDraw(BARE2D::BumpyRenderer* renderer, float time, int layerDifference, float xOffset) {
}

void EntityPlayer::drawGUI(BARE2D::BasicRenderer* renderer, BARE2D::FontRenderer* fontRenderer) {
	if(!m_initedGUI)
		initGUI();

	glm::vec4	   fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	BARE2D::Colour fullColour(255, 255, 255, 255);

	renderer->begin();

	if(m_selectedEntity) {
		glm::vec4	fullUV(0.0f, 0.0f, 1.0f, 1.0f);
		std::string cursorPath	= "GUI/Player/Cursor.png";
		int			cursorImgId = BARE2D::ResourceManager::loadTexture(cursorPath).id;

		glm::vec4 cursorDestRect(m_selectedEntity->getPosition().x,
								 m_selectedEntity->getPosition().y,
								 m_selectedEntity->getSize().x,
								 m_selectedEntity->getSize().y);
		renderer->draw(cursorDestRect,
					   fullUV,
					   cursorImgId,
					   1.5f * (WORLD_DEPTH - m_layer),
					   BARE2D::Colour(255, 255, 255, 255));
	} else if(m_selectedBlock) { // Cursor box selection
		glm::vec4	fullUV(0.0f, 0.0f, 1.0f, 1.0f);
		std::string cursorPath	= "GUI/Player/Cursor.png";
		int			cursorImgId = BARE2D::ResourceManager::loadTexture(cursorPath).id;

		glm::vec4 cursorDestRect(m_selectedBlock->getPosition().x,
								 m_selectedBlock->getPosition().y,
								 m_selectedBlock->getSize().x,
								 m_selectedBlock->getSize().y);
		renderer->draw(cursorDestRect,
					   fullUV,
					   cursorImgId,
					   1.5f * (WORLD_DEPTH - m_layer),
					   BARE2D::Colour(255, 255, 255, 255));
	}

	/*{
		// Hotbar
		int hotbarImgId = BARE2D::ResourceManager::loadTexture(ASSETS_FOLDER_PATH + "GUI/Player/Hotbar.png").id;
		int hotbarSelectImgId = BARE2D::ResourceManager::loadTexture(ASSETS_FOLDER_PATH + "GUI/Player/HotbarSelection.png").id;

		for(int i = 0; i < HOTBAR_BOX_NUM; i++) {

			glm::vec4 uv(i * (1.0 / HOTBAR_BOX_NUM), 0.0f, (1.0 / HOTBAR_BOX_NUM), 1.0f);
			glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
			renderer->draw(destRect, uv, hotbarImgId, 0.0f, fullColour);

			{
				if(m_favouriteItems[i]) {
					glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
					                   HOTBAR_BOX_SIZE / 4,
					                   HOTBAR_BOX_SIZE,
					                   HOTBAR_BOX_SIZE);
					glm::vec4 itemUV(0, 0, 1, 1);
					int itemImgId = m_favouriteItems[i]->getTextureId();//BARE2D::ResourceManager::loadTexture(Category_Data::itemData[m_favouriteItems[i]->getID()].texturePath).id;

					renderer->draw(destRect, itemUV, itemImgId, 0.4f, BARE2D::Colour(255, 255, 255, 255));
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
				int itemImgId = m_favouriteItems[i]->getTextureId();//BARE2D::ResourceManager::loadTexture(Category_Data::itemData[m_favouriteItems[i]->getID()].texturePath).id;

				renderer->draw(destRect, itemUV, itemImgId, 0.4f, BARE2D::Colour(255, 255, 255, 255));

				fontRenderer.draw(renderer, std::to_string(m_favouriteItems[i]->getQuantity()).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE * 9 / 10, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 1.0f, BARE2D::Colour(255, 255, 255, 255), BARE2D::Justification::RIGHT);
			}
		}

		glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * m_selectedHotbox, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
		renderer->draw(destRect, fullUV, hotbarSelectImgId, 1.1f, fullColour);
	} // Hotbar END*/

	if(m_bagOpen) {
		glm::vec2 size = Singletons::getGameCamera()->getViewspaceCoord(glm::vec2(3.0f, 3.0f));
		glm::vec4 destRect(0.0f, 0.0f, size.x, size.y);
		m_inventory->setDestRect(destRect);
		m_inventory->draw(renderer, fontRenderer, m_position.x - m_size.x / 2.0f, m_position.y + m_size.y * 3.0f / 4.0f);
	} else if(m_inventoryOpen) {
		m_armourWeaponsInventory->draw(renderer, fontRenderer, m_position.x, m_position.y);
	}

	renderer->end();
	renderer->render();
}

#define cap(x) \
	if(x > 1)  \
		x = 1; \
	if(x < 0)  \
		x = 0;

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

		if((int)mouseCoords.x > -(int)((int)Singletons::getWorld()->getSize() * (int)CHUNK_SIZE)) {
			m_selectedBlock = Singletons::getWorld()->getTile((int)mouseCoords.x, mouseCoords.y, m_layer);
		}

		m_selectedEntity = nullptr;

		for(unsigned int i = 0; i < Singletons::getEntityManager()->getEntities().size() && !m_selectedEntity; i++) {
			float sizeX = (Singletons::getEntityManager()->getEntities()[i]->getSize().x) / 2.0f;
			float midX	= Singletons::getEntityManager()->getEntities()[i]->getPosition().x + sizeX;

			float sizeY = (Singletons::getEntityManager()->getEntities()[i]->getSize().y) / 2.0f;
			float midY	= Singletons::getEntityManager()->getEntities()[i]->getPosition().y + sizeY;

			if(std::abs(midX - mouseCoords.x) <= sizeX / 2.0f) {
				if(std::abs(midY - mouseCoords.y) <= sizeY / 2.0f) {
					m_selectedEntity = Singletons::getEntityManager()->getEntities()[i];
				}
			}
		}
	}
}

void EntityPlayer::updateInput(BARE2D::InputManager* input) {
	if(input->isKeyDown(SDLK_w) && m_stamina > 0.0f) {
		if(m_onGround) {
			m_velocity.y =
				m_jumpHeight; // y=(jumpHeight*TILE_SIZE+3/4*TILE_SIZE+-5.88*x^2)  initial jump power is the absolute of the x when y=0. jumpheight is in eights of tiles and you must add 4
			m_onGround = false;
			m_stamina -= 0.005f;
		}
	}
	if(input->isKeyDown(SDLK_s)) {
		m_velocity.y -= 0.1f;
	}

	if(input->isKeyPressed(SDLK_e)) { /// TODO: Implement back and forward doors.
		moveDownLayer();
	}
	if(input->isKeyPressed(SDLK_q)) {
		moveUpLayer();
	}
	m_layer = (m_layer < 0) ? 0 : (m_layer >= WORLD_DEPTH) ? WORLD_DEPTH - 1 : m_layer;

	if(input->isKeyDown(SDLK_d) && (m_stamina > 0.0f)) {
		if(m_velocity.x < 0.0f)
			m_velocity.x /= 5.0f;
		if(m_velocity.x < MAX_SPEED) {
			m_velocity.x += m_runSpeed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
		}
		if(m_state != MovementState::LOW_VEL && m_lowVelAnimation) {
			m_state = MovementState::LOW_VEL;
			m_body.changeAnimation(m_lowVelAnimation);
			m_flippedTexture = false;
		}
		m_stamina *= 0.999f;
	} else if(input->isKeyDown(SDLK_a) && (m_stamina > 0.0f)) {
		if(m_velocity.x > 0.0f)
			m_velocity.x /= 5.0f;
		if(m_velocity.x > -MAX_SPEED) {
			m_velocity.x -= m_runSpeed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
		}
		if(m_state != MovementState::LOW_VEL && m_lowVelAnimation) {
			m_state = MovementState::LOW_VEL;
			m_body.changeAnimation(m_lowVelAnimation);
			m_flippedTexture = true;
		}
		m_stamina *= 0.999f;
	}

	if(std::abs(m_velocity.x) < 0.001f && m_onGround && m_stamina * 1.003f <= 1.000000f) {
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
			/// TODO
			BARE2D::throwError(BARE2D::BAREError::UNINITIALIZED_FUNCTION,
							   "input->isKeyPressed(SDL_BUTTON_LEFT) && m_selectedBlock  block is not initialized.");
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
		m_armourWeaponsInventory->setToDraw(m_inventoryOpen);
	}

	for(unsigned int i = 1; i < 9; i++) { // Just 1-9
		if(input->isKeyPressed(SDLK_0 + i)) {
			m_selectedHotbox = i - 1;
			//m_favouriteItems[i-1] = m_inventory->getItem(i-1);
		}
	}

	if(input->isKeyPressed(SDLK_F9)) {
		m_inventory->addItem(new ItemArmour(1, 9));
	}
	/// THESE ARE PURELY FOR DEVELOPMENT
	if(input->isKeyPressed(SDLK_t)) {
		m_inventory->addItem(new ItemBlock(1, (unsigned int)ItemIDs::BLOCK_TORCH));
	}
}

bool EntityPlayer::event_reskin(const CEGUI::EventArgs& e) {
	reskinLimbs();
}

SaveDataTypes::EntityPlayerData EntityPlayer::getPlayerSaveData() {
	SaveDataTypes::EntityPlayerData ret;
	ret.id		 = m_id;
	ret.velocity = m_velocity;
	ret.position = m_position;
	ret.layer	 = m_layer;

	ret.health	  = m_health;
	ret.inventory = m_inventory->getInventorySaveData();

	ret.sanity	   = m_sanity;
	ret.thirst	   = m_thirst;
	ret.hunger	   = m_hunger;
	ret.exhaustion = m_exhaustion;
	ret.stamina	   = m_stamina;

	return ret;
}
