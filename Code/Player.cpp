#include "Player.h"

#include "PresetValues.h"
#include "ItemBlock.h"
#include "BuffData.h"

#include "Entities.h"

#include <Errors.h>

Player::Player(glm::vec2 position, Chunk* parent, GLEngine::InputManager* input, ScriptQueue* sq, AudioManager* audio) : Entity(position, audio, sq, 12.0f/60.0f, Categories::LootTableIds::NONE, 0, 0), m_input(input)
{
    m_inventory = new Inventory();

    m_texture = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "/Textures/Mobs/Mob0.png");
    m_size = glm::vec2(1.0f, 2.0f);
    m_faction = Categories::Faction::GOOD;
    m_jumpHeight = 0.4f;
    m_speed = 2.5f/60.0f;
    //m_ai = Categories::AI_Type::;
    //m_disabilities = Categories::Disability_Type::NONE;
    //m_attackType = Categories::Attack_Type::;

    m_parentChunk = parent;

    Script s;
    s.commands.push_back("changeBlock relative near relative player 0 0 0 0 2");
    s.commands.push_back("changeBlock relative near relative player 0 0 0 1 6"); // changeblock [position] [id]
    s.commands.push_back("teleport near relative player 0 0 relative near relative player 0 0 0 3"); // teleport [entity] [position] -> [entity] is near [relative player 0 0], [position] is relative [near [relative player 0 0]] 0 3
    s.commands.push_back("time set 120");

    std::string path = "Script_player_makeHouse.txt";

    m_scriptID_dayTime = m_sq->addScript(s);
    m_scriptID_makeHouse = m_sq->addScript(path);

    Leg* leg0 = new Leg(this, m_texture.id, glm::vec2(-1.0f, 0.0f), glm::vec2(0.4f, 1.0f), 0.0f, 1.57f); // 3.14 = 2(1.57
    m_limbs.push_back(leg0);

    m_type = Categories::Entity_Type::PLAYER;
}

Player::~Player()
{
    delete m_inventory;
    delete m_limbs[0];
}

void Player::initGUI(GLEngine::GUI* gui) {
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

void Player::draw(GLEngine::SpriteBatch& sb, float time, float xOffset) {
    glm::vec4 destRect = glm::vec4(m_position.x + xOffset * CHUNK_SIZE, m_position.y, m_size.x, m_size.y);

    float x, y;
    if(m_velocity.x > m_speed) {
        x = (int)time*abs((int)m_velocity.x+1) % 3;
        y = 1;
        m_flippedTexture = false;
    } else if(m_velocity.x < -m_speed) {
        x = (int)time*abs((int)m_velocity.x+1) % 3;
        y = 1;
        m_flippedTexture = true;
    } else {
        x = 0;
        y = 1;
    }
    if(m_velocity.y > 0.0f) {
        x = (int)time % 3;
        y = 0;
    }

    float finalX = (x / ((float)m_texture.width / (m_size.x * 32)));
    float finalY = (y / ((float)m_texture.height / (m_size.y * 32)));

    glm::vec4 uvRect;

    if(!m_flippedTexture) {
        uvRect = glm::vec4(finalX,
                           finalY,
                           1.0f / ((float)m_texture.width / (m_size.x * 32)),
                           1.0f / ((float)m_texture.height / (m_size.y * 32)));
    } else if(m_flippedTexture) {
        uvRect = glm::vec4(finalX + 1.0f / ((float)m_texture.width / (m_size.x * 32)),
                           finalY,
                           1.0f / -((float)m_texture.width / (m_size.x * 32)),
                           1.0f / ((float)m_texture.height / (m_size.y * 32)));
    }

    GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.8f, colour, glm::vec3(m_light));

    for(int i = 0; i < m_limbs.size(); i++) {
        m_limbs[i]->draw(sb);
    }

    if(m_selectedEntity) {
        glm::vec4 fullUV(0.0f, 0.0f, 1.0f, 1.0f);
        int cursorImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Cursor.png").id;

        glm::vec4 cursorDestRect(m_selectedEntity->getPosition().x + xOffset * CHUNK_SIZE, m_selectedEntity->getPosition().y, m_selectedEntity->getSize().x, m_selectedEntity->getSize().y);
        sb.draw(cursorDestRect, fullUV, cursorImgId, 0.9f, GLEngine::ColourRGBA8(255, 255, 255, 255));
    } else if(m_selectedBlock) { // Cursor box selection
        glm::vec4 fullUV(0.0f, 0.0f, 1.0f, 1.0f);
        int cursorImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Cursor.png").id;

        int chunkIndex = (int)floor(m_mousePos.x / CHUNK_SIZE);
        int x = (int)(m_mousePos.x + CHUNK_SIZE) % CHUNK_SIZE;

        glm::vec4 cursorDestRect(x + chunkIndex * CHUNK_SIZE + xOffset * CHUNK_SIZE, m_selectedBlock->getPosition().y, m_selectedBlock->getSize().x, m_selectedBlock->getSize().y);
        sb.draw(cursorDestRect, fullUV, cursorImgId, 0.9f, GLEngine::ColourRGBA8(255, 255, 255, 255));
    }
}

void Player::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {
    if(m_gui) {
        glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

        sb.begin(GLEngine::GlyphSortType::FRONT_TO_BACK); // lower numbers in back

        { // Hotbar
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
                        int itemImgId = m_favouriteItems[i]->getTexture().id;//GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_favouriteItems[i]->getID()].texturePath).id;

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
                    int itemImgId = m_favouriteItems[i]->getTexture().id;//GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_favouriteItems[i]->getID()].texturePath).id;

                    sb.draw(destRect, itemUV, itemImgId, 0.4f, GLEngine::ColourRGBA8(255, 255, 255, 255));

                    sf.draw(sb, std::to_string(m_favouriteItems[i]->getQuantity()).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE * 9/10, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 1.0f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);
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

void Player::update(float timeStep, Chunk* worldChunks[WORLD_SIZE]) {
    Logger::getInstance()->log("Parent Chunk id: " + std::to_string(m_parentChunk->getIndex()));

    updateLightLevel();

    updateInput();
    updateLimbs();
    updateStats(timeStep, worldChunks);
    updateSounds();
    if(!m_godMode) {
        move(timeStep);
    } else {
        godMove();
    }
    m_inventory->update();

    if(m_velocity.x > MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f) && !m_godMode) {
        m_velocity.x = MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
    } else if(m_velocity.x < -MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f) && !m_godMode) {
        m_velocity.x = -MAX_SPEED * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
    }
    setParentChunk(worldChunks);

    for(int i = 0; i < m_parentChunk->getEntities().size(); i++) {
        Entity* e = m_parentChunk->getEntities()[i];
        if(e->getType() == Categories::Entity_Type::ITEM) {
            float xDist = std::abs(e->getPosition().x - m_position.x);
            float yDist = std::abs(e->getPosition().y - m_position.y);
            float dist = std::sqrt(xDist * xDist + yDist * yDist);

            if(dist <= 3.0f) {
                m_inventory->addItem((static_cast<EntityNeutralItem*>(e)->getItem()));
                delete e;
                m_parentChunk->removeEntity(i);
            }
        }
    }
}

#define cap(x) if(x > 1) x = 1; if(x < 0) x = 0;

void Player::updateStats(float timeStep, Chunk* worldChunks[WORLD_SIZE]) {
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

    m_thirst -= 0.00006f;
    m_hunger -= 0.00003f;
    m_exhaustion -= 0.0001f;

    if(m_stamina < 0.8f) {
        m_exhaustion -= 0.0002f;
    }

    if(m_exhaustion < 0.25f) {
        m_sanity -= 0.0001f;
    }
    if(m_light < 0.25f) {
        if(m_exposedToSun) {
            Buff_Increase_Sanity_StarLight* potentialBuff = new Buff_Increase_Sanity_StarLight(&m_sanity);
            if(std::find(m_buffs.begin(), m_buffs.end(), potentialBuff) == m_buffs.end() || m_buffs.size() == 0) {
                m_buffs.push_back(potentialBuff);
            } else {
                delete potentialBuff;
            }
        }
    }
    if(m_light < 0.25f) {
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
    }
}

void Player::updateMouse(GLEngine::Camera2D* worldCamera) {
    glm::vec2 mousePos = worldCamera->convertScreenToWorld(m_input->getMouseCoords());

    mousePos = glm::vec2(floor(mousePos.x), floor(mousePos.y)); // Changes it to grid-space

    m_mousePos = mousePos;

    if(m_canInteract) {

        int chunkIndex = std::floor(mousePos.x / CHUNK_SIZE) - getChunkIndex();

        if(/*mousePos.x >= 0 &&
           mousePos.x + (chunkIndex * CHUNK_SIZE) < WORLD_SIZE * CHUNK_SIZE &&*/
           mousePos.y >= 0 &&
           mousePos.y < WORLD_HEIGHT) {
                Chunk* chunk = nullptr;

                if(chunkIndex < 0) {
                    chunk = m_parentChunk->getSurroundingChunks()[0];
                } else
                if(chunkIndex > 0) {
                    chunk = m_parentChunk->getSurroundingChunks()[1];
                } else
                if(chunkIndex == 0) {
                    chunk = m_parentChunk;
                }

                m_selectedBlock = nullptr;

                Logger::getInstance()->log(std::to_string(mousePos.x));

                if(chunk->getTile((int)mousePos.x, mousePos.y, m_layer)->getParentChunk()) m_selectedBlock = chunk->getTile((int)mousePos.x, mousePos.y, m_layer);
                //Logger::getInstance()->log("Clicked: " + std::to_string(m_selectedBlock->getPosition().x));

                m_selectedEntity = nullptr;

                for(int i = 0; i < chunk->getEntities().size(); i++) {
                    float sizeX = (chunk->getEntities()[i]->getSize().x) / 4;
                    float midX = chunk->getEntities()[i]->getPosition().x + sizeX;

                    float sizeY = (chunk->getEntities()[i]->getSize().y) / 4;
                    float midY = chunk->getEntities()[i]->getPosition().y + sizeY;

                    if(std::abs(midX - mousePos.x) <= sizeX) {
                        if(std::abs(midY - mousePos.y) <= sizeY) {
                            m_selectedEntity = chunk->getEntities()[i];
                        }
                    }
                }

            }
    }

}

void Player::updateInput() {
    if(m_input->isKeyDown(SDLK_w) && m_stamina > 0.0f) {
        if(m_onGround || m_godMode) {
            m_velocity.y = m_jumpHeight; // y=(jumpHeight*TILE_SIZE+3/4*TILE_SIZE+-5.88*x^2)  initial jump power is the absolute of the x when y=0. jumpheight is in eights of tiles and you must add 4
            m_onGround = false;
            m_stamina -= 0.005f;
        }
    }
    if(m_input->isKeyDown(SDLK_s) && m_godMode) {
        m_velocity.y -= 0.1f;
    }

    if(m_input->isKeyDown(SDLK_d) && ((m_stamina > 0.0f && m_onGround) || m_godMode)) {
        if(m_velocity.x < 0.0f) m_velocity.x /= 5.0f;
        if(m_velocity.x < m_maxSpeed) {
            m_velocity.x += m_speed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
        }
        m_stamina *= 0.999f;
    } else if(m_input->isKeyDown(SDLK_a) && ((m_stamina > 0.0f && m_onGround) || m_godMode)) {
        if(m_velocity.x > 0.0f) m_velocity.x /= 5.0f;
        if(m_velocity.x > -m_maxSpeed)
            m_velocity.x -= m_speed * m_inventory->getSpeedMultiplier() * std::pow(m_stamina, 0.4f);
        m_stamina *= 0.999f;
    } else {
        if(m_onGround)
            m_velocity.x *= 0.9f;
    }

    if(m_velocity.x < 0.001f && m_velocity.x > -0.001f && m_onGround && m_stamina * 1.003f <= 1.000000f) m_stamina *= 1.003f;

    if(m_canInteract) {
        if(m_input->isKeyPressed(SDLK_e)) {
            if(m_selectedEntity) { // must hover mouse over entity and press 'e'
                m_selectedEntity->onTalk(m_sq);
            }
        } else if(m_input->isKeyPressed(SDLK_t)) {
            if(m_selectedEntity) {
                m_selectedEntity->onTrade(m_sq);
            }
        }

        if(m_input->isKeyDown(SDLK_p)) {
            m_inventory->addItem(createItem((unsigned int)Categories::ItemIDs::BLOCK_WOOD, 1));
        }
        if(m_input->isKeyPressed(SDLK_o)) {
            m_inventory->addItem(createItem((unsigned int)Categories::ItemIDs::MISC_BUCKET, 1));
        }

        if(m_input->isKeyDown(SDL_BUTTON_LEFT) && m_selectedBlock) {
            if(m_favouriteItems[m_selectedHotbox]) m_favouriteItems[m_selectedHotbox]->onLeftClick(m_selectedBlock);
            if(!m_favouriteItems[m_selectedHotbox]) {
                BlockAir* b = new BlockAir(m_selectedBlock->getPosition(), m_selectedBlock->getParentChunk());//createBlock((unsigned int)Categories::BlockIDs::WATER, m_selectedBlock->getPosition(), m_selectedBlock->getParentChunk());
                m_selectedBlock->getParentChunk()->setTile(b, m_selectedBlock->getLayer());
            }
            m_inventory->updateWeight();
        }
        if(m_input->isKeyPressed(SDL_BUTTON_RIGHT) && m_selectedBlock) {
            if(m_favouriteItems[m_selectedHotbox]) m_favouriteItems[m_selectedHotbox]->onRightClick(m_selectedBlock);
            m_inventory->updateWeight();
        }
        if(m_input->isKeyPressed(SDLK_r)) {
            Item* newItem = createItem((unsigned int)Categories::ItemIDs::BLOCK_DIRT, 1);
            m_inventory->addItem(newItem);
            //m_sq->activateScript(m_scriptID_makeHouse);
        }
        if(m_input->isKeyPressed(SDLK_t)) {
            Item* newItem = createItem((unsigned int)Categories::ItemIDs::BLOCK_TORCH, 1);
            m_inventory->addItem(newItem);
        }
        if(m_input->isKeyPressed(SDLK_i)) {
            m_inventoryOpen = !m_inventoryOpen;
        }
    }

    if(m_input->isKeyPressed(SDLK_1)) {
        m_selectedHotbox = 0;
        m_favouriteItems[0] = m_inventory->getItem(0);
    } else
    if(m_input->isKeyPressed(SDLK_2)) {
        m_selectedHotbox = 1;
        m_favouriteItems[1] = m_inventory->getItem(1);
    } else
    if(m_input->isKeyPressed(SDLK_3)) {
        m_selectedHotbox = 2;
    } else
    if(m_input->isKeyPressed(SDLK_4)) {
        m_selectedHotbox = 3;
    } else
    if(m_input->isKeyPressed(SDLK_5)) {
        m_selectedHotbox = 4;
    } else
    if(m_input->isKeyPressed(SDLK_6)) {
        m_selectedHotbox = 5;
    } else
    if(m_input->isKeyPressed(SDLK_7)) {
        m_selectedHotbox = 6;
    } else
    if(m_input->isKeyPressed(SDLK_8)) {
        m_selectedHotbox = 7;
    } else
    if(m_input->isKeyPressed(SDLK_9)) {
        m_selectedHotbox = 8;
    } else
    if(m_input->isKeyPressed(SDLK_0)) {
        m_selectedHotbox = 9;
    }
}

void Player::godMove() {
    //m_velocity.y -= 1.225f / 60.0f; // Earth gravity is far too harsh for games. We use about 1/8th
    //m_speed = 2.0f;
    m_position += m_velocity;
    m_stamina = 1.0f;
}
