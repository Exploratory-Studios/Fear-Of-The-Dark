#include "EntityPlayer.h"

#include <Errors.h>

#include "World.h"
#include "Tile.h"
#include "Inventory.h"

#include "Factory.h"

EntityPlayer::EntityPlayer(glm::vec2 pos, unsigned int layer, SaveDataTypes::MetaData data, bool loadTex) : EntityNPC(pos, layer, 0, data, loadTex) {
	m_lowVelAnimation.init(0);
}

EntityPlayer::~EntityPlayer() {
	//dtor
}

void EntityPlayer::initGUI(GLEngine::GUI* gui) {
	m_gui = gui;
	m_statusBoxFrame = static_cast<CEGUI::PopupMenu*>(m_gui->createWidget("FOTDSkin/StatusBox", glm::vec4(0.785f, 0.025f, 0.2f, 0.4f), glm::vec4(0.0f), "PlayerGUI_StatusBox"));
	m_statusBoxFrame->openPopupMenu();

	m_statusBoxLabel = static_cast<CEGUI::DefaultWindow*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/Label", glm::vec4(0.075f, 0.025f, 0.925f, 1.0f), glm::vec4(0.0f), "PlayerGUI_StatusBox_Label"));
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

	m_sanityBar     = static_cast<CEGUI::ProgressBar*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/SanityBar",     glm::vec4(0.25f, 0.10f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Sanity"));
	m_healthBar     = static_cast<CEGUI::ProgressBar*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/HealthBar",     glm::vec4(0.25f, 0.23f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Health"));
	m_thirstBar     = static_cast<CEGUI::ProgressBar*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/ThirstBar",     glm::vec4(0.25f, 0.36f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Thirst"));
	m_hungerBar     = static_cast<CEGUI::ProgressBar*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/HungerBar",     glm::vec4(0.25f, 0.50f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Hunger"));
	m_exhaustionBar = static_cast<CEGUI::ProgressBar*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/ExhaustionBar", glm::vec4(0.25f, 0.63f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Exhaustion"));
	m_staminaBar    = static_cast<CEGUI::ProgressBar*>(m_gui->createWidget(m_statusBoxFrame, "FOTDSkin/StaminaBar",    glm::vec4(0.25f, 0.76f + 0.066f, 0.65f, 0.1f), glm::vec4(), "PlayerGUI_StatusBox_Stamina"));

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


	m_buffBoxFrame = static_cast<CEGUI::PopupMenu*>(m_gui->createWidget("FOTDSkin/StatusBox", glm::vec4(0.725f, 0.025f, 0.055f, 0.4f), glm::vec4(0.0f), "PlayerGUI_BuffBox"));
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
	if(m_gui) {
		glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
		GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

		sb.begin();

		{
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
		} // Hotbar END


		{
			if(m_inventoryOpen) {
				m_inventory->draw(HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE * 1.5, sb, sf);
			}
		}

		sb.end();
		sb.renderBatch();

	} else {
		GLEngine::FatalError("Dumb Programmer Didn't Initialize PlayerGUI... ");
	}
}

#define cap(x) if(x > 1) x = 1; if(x < 0) x = 0;

void EntityPlayer::updateStats(World* world, float timeStep) {
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

void EntityPlayer::updateMouse(World* world, glm::vec2 mouseCoords) {

	if(m_canInteract) {
		m_selectedBlock = nullptr;

		if(world->getTile((int)mouseCoords.x, mouseCoords.y, m_layer)) {
			m_selectedBlock = world->getTile((int)mouseCoords.x, mouseCoords.y, m_layer);
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

void EntityPlayer::updateInput(GLEngine::InputManager* input, World* world) {
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
		moveDownLayer(world);
	}
	if(input->isKeyPressed(SDLK_q)) {
		moveUpLayer(world);
	}
	m_layer = (m_layer < 0) ? 0 : (m_layer >= WORLD_DEPTH) ? WORLD_DEPTH - 1 : m_layer;

	if(input->isKeyDown(SDLK_d) && (m_stamina > 0.0f)) {
		if(m_velocity.x < 0.0f) m_velocity.x /= 5.0f;
		if(m_velocity.x < MAX_SPEED) {
			m_velocity.x += m_runSpeed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
		}
		m_stamina *= 0.999f;
	} else if(input->isKeyDown(SDLK_a) && (m_stamina > 0.0f)) {
		if(m_velocity.x > 0.0f) m_velocity.x /= 5.0f;
		if(m_velocity.x > -MAX_SPEED)
			m_velocity.x -= m_runSpeed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
		m_stamina *= 0.999f;
	} else {
		if(m_onGround)
			m_velocity.x *= 0.9f;
	}

	if(m_velocity.x < 0.001f && m_velocity.x > -0.001f && m_onGround && m_stamina * 1.003f <= 1.000000f) m_stamina *= 1.003f;

	if(m_canInteract) {
		if(input->isKeyPressed(SDLK_r)) {
			if(m_selectedEntity) { // must hover mouse over entity and press 'e'
				if(m_selectedEntity->getType() == EntityTypes::NPC) {
					dynamic_cast<EntityNPC*>(m_selectedEntity)->onTalk();
				}
			}
		} else if(input->isKeyPressed(SDLK_t)) {
			if(m_selectedEntity) {
				if(m_selectedEntity->getType() == EntityTypes::NPC) {
					dynamic_cast<EntityNPC*>(m_selectedEntity)->onTrade();
				}
			}
		}

		if(input->isKeyDown(SDLK_p)) {
			m_inventory->addItem(new Item(1, ItemIDs::BLOCK_WOOD, true));
		}

		if(input->isKeyPressed(SDL_BUTTON_LEFT) && m_selectedBlock) {
			//if(m_favouriteItems[m_selectedHotbox]) m_favouriteItems[m_selectedHotbox]->onLeftClick(m_selectedBlock, world);
			if(!m_favouriteItems[m_selectedHotbox]) {
				Tile* t = new Tile(m_selectedBlock->getPosition(), m_selectedBlock->getLayer(), 0, SaveDataTypes::MetaData(), false);
				world->setTile(t);
			}
			if(m_inventory) m_inventory->updateWeight();
		}
		if(input->isKeyPressed(SDL_BUTTON_RIGHT)) {
			if(m_selectedBlock) {
				if(m_favouriteItems[m_selectedHotbox]) {
					m_favouriteItems[m_selectedHotbox]->onRightClick(m_selectedBlock);
				}
				m_inventory->updateWeight();
				if(m_selectedBlock->getID() != (unsigned int)TileIDs::AIR) {
					m_selectedBlock->onInteract_RightClicked();
				}
			}
		}
		if(input->isKeyPressed(SDLK_r)) {
			Item* newItem = new Item(1, ItemIDs::BLOCK_DIRT, true);
			m_inventory->addItem(newItem);
		}
		if(input->isKeyPressed(SDLK_t)) {
			if(input->isKeyDown(SDLK_LSHIFT)) {
				Item* newItem = new Item(1, ItemIDs::BLOCK_TORCH_BRIGHT, true);

				m_inventory->addItem(newItem);
			} else {
				Item* newItem = new Item(1, ItemIDs::BLOCK_TORCH, true);
				m_inventory->addItem(newItem);
			}
		}
		if(input->isKeyPressed(SDLK_i)) {
			m_inventoryOpen = !m_inventoryOpen;
		}
	}

	if(input->isKeyPressed(SDLK_1)) {
		m_selectedHotbox = 0;
		m_favouriteItems[0] = m_inventory->getItem(0);
	} else if(input->isKeyPressed(SDLK_2)) {
		m_selectedHotbox = 1;
		m_favouriteItems[1] = m_inventory->getItem(1);
	} else if(input->isKeyPressed(SDLK_3)) {
		m_selectedHotbox = 2;
	} else if(input->isKeyPressed(SDLK_4)) {
		m_selectedHotbox = 3;
	} else if(input->isKeyPressed(SDLK_5)) {
		m_selectedHotbox = 4;
	} else if(input->isKeyPressed(SDLK_6)) {
		m_selectedHotbox = 5;
	} else if(input->isKeyPressed(SDLK_7)) {
		m_selectedHotbox = 6;
	} else if(input->isKeyPressed(SDLK_8)) {
		m_selectedHotbox = 7;
	} else if(input->isKeyPressed(SDLK_9)) {
		m_selectedHotbox = 8;
	} else if(input->isKeyPressed(SDLK_0)) {
		m_selectedHotbox = 9;
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
