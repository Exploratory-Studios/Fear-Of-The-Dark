#include "Player.h"

#include "PresetValues.h"
#include "ItemBlock.h"



#include <iostream>

Player::Player() {
    m_inventory = new Inventory();
}

Player::Player(glm::vec2 position, GLEngine::InputManager* input) : m_input(input)
{
    init(position, Categories::Entity_Type::MOB, 0);
    m_inventory = new Inventory();
    m_speed = 0.2f;
    m_jumpHeight = 2.736f;
}

Player::~Player()
{
    //dtor
}

void Player::draw(GLEngine::SpriteBatch& sb, float time) {

    glm::vec4 destRect = glm::vec4(m_position.x, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);

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

    sb.draw(destRect, uvRect, m_texture.id, 1.0f, colour, glm::vec3(m_light));

    if(m_selectedBlock) { // Cursor box selection
        glm::vec4 fullUV(0.0f, 0.0f, 1.0f, 1.0f);
        int cursorImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Cursor.png").id;

        int chunkIndex = (int)floor(m_mousePos.x / CHUNK_SIZE);
        int x = (int)(m_mousePos.x + CHUNK_SIZE * TILE_SIZE) % CHUNK_SIZE * TILE_SIZE;

        glm::vec4 cursorDestRect(x + chunkIndex * CHUNK_SIZE * TILE_SIZE, m_selectedBlock->getPosition().y * TILE_SIZE, m_selectedBlock->getSize().x * TILE_SIZE, m_selectedBlock->getSize().y * TILE_SIZE);
        sb.draw(cursorDestRect, fullUV, cursorImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
    }
}

void Player::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {

    glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

    {
        int hotbarImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/Hotbar.png").id;
        int hotbarSelectImgId = GLEngine::ResourceManager::getTexture(ASSETS_FOLDER_PATH + "GUI/Player/HotbarSelection.png").id;

        sb.begin();

        for(int i = 0; i < HOTBAR_BOX_NUM; i++) {

            glm::vec4 uv(i * (1.0 / HOTBAR_BOX_NUM), 0.0f, (1.0 / HOTBAR_BOX_NUM), 1.0f);
            glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
            sb.draw(destRect, uv, hotbarImgId, 0.0f, fullColour);

            {
                if(m_inventory->getItem(i, 0)) {
                    glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
                                    HOTBAR_BOX_SIZE / 4,
                                    HOTBAR_BOX_SIZE,
                                    HOTBAR_BOX_SIZE);
                    glm::vec4 itemUV(0, 0, 1, 1);
                    int itemImgId = GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_inventory->getItem(i, 0)->getID()].texturePath).id;

                    sb.draw(destRect, itemUV, itemImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
                }
            }
        }

        sb.end();
        sb.renderBatch();

        for(int i = 0; i < HOTBAR_BOX_NUM; i++) {
            if(m_inventory->getItem(i, 0)) {
                glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
                                    HOTBAR_BOX_SIZE / 4,
                                    HOTBAR_BOX_SIZE,
                                    HOTBAR_BOX_SIZE);
                glm::vec4 itemUV(0, 0, 1, 1);
                int itemImgId = GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_inventory->getItem(i, 0)->getID()].texturePath).id;

                sb.begin();
                sb.draw(destRect, itemUV, itemImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
                sb.end();
                sb.renderBatch();

                sb.begin();
                sf.draw(sb, std::to_string(m_inventory->getItem(i, 0)->getQuantity()).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE * 9/10, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);
                sb.end();
                sb.renderBatch();
            }
        }

        sb.begin();
        glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * m_selectedHotbox, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
        sb.draw(destRect, fullUV, hotbarSelectImgId, 0.0f, fullColour);

        sb.end();
        sb.renderBatch();
    }

    {
        if(m_inventoryOpen) {

            m_inventory->draw(HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE * 1.5, sb, sf);

        }
    }
}

void Player::update(float timeStep) {

    setParentChunk(m_parentChunk->getSurroundingChunks());
    updateLightLevel();

    updateInput();
    m_inventory->update();

    if(m_velocity.x > MAX_SPEED * m_inventory->getSpeedMultiplier()) {
        m_velocity.x = MAX_SPEED * m_inventory->getSpeedMultiplier();
    } else if(m_velocity.x < -MAX_SPEED * m_inventory->getSpeedMultiplier()) {
        m_velocity.x = -MAX_SPEED * m_inventory->getSpeedMultiplier();
    }
}

void Player::updateMouse(GLEngine::Camera2D* worldCamera) {
    glm::vec2 mousePos = worldCamera->convertScreenToWorld(m_input->getMouseCoords());

    mousePos = glm::vec2(floor(mousePos.x / TILE_SIZE), floor(mousePos.y / TILE_SIZE)); // Changes it to grid-space

    m_mousePos = mousePos;

    unsigned int chunkIndex = ((int)abs(floor(mousePos.x / CHUNK_SIZE) + WORLD_SIZE) % WORLD_SIZE) - m_parentChunkIndex;

    mousePos.x = (int)(mousePos.x + CHUNK_SIZE * WORLD_SIZE) % CHUNK_SIZE;

    if(/*mousePos.x >= 0 &&
       mousePos.x + (chunkIndex * CHUNK_SIZE) < WORLD_SIZE * CHUNK_SIZE &&*/
       mousePos.y >= 0 &&
       mousePos.y < WORLD_HEIGHT) {
			Chunk* chunk = nullptr;

			if(chunkIndex < 0) chunk = m_parentChunk->getSurroundingChunks()[0];
			if(chunkIndex > 0) chunk = m_parentChunk->getSurroundingChunks()[1];
			if(chunkIndex == 0) chunk = m_parentChunk;

			m_selectedBlock = static_cast<Block*>(chunk->tiles[(unsigned int)mousePos.y][(unsigned int)mousePos.x]);

	}

}

void Player::updateInput() {
    if(m_input->isKeyDown(SDLK_w)) {
        if(m_onGround) {
            m_velocity.y = m_jumpHeight; // y=(jumpHeight*TILE_SIZE+3/4*TILE_SIZE+-5.88*x^2)  initial jump power is the absolute of the x when y=0. jumpheight is in eights of tiles and you must add 4
            m_onGround = false;
        }
    }

    if(m_input->isKeyDown(SDLK_d)) {
        if(m_velocity.x < 0.0f) m_velocity.x /= 5.0f;
        m_velocity.x += m_speed * m_inventory->getSpeedMultiplier();
    } else if(m_input->isKeyDown(SDLK_a)) {
        if(m_velocity.x > 0.0f) m_velocity.x /= 5.0f;
        m_velocity.x -= m_speed * m_inventory->getSpeedMultiplier();
    } else {
        m_velocity.x *= 0.9f;
    }

    if(m_canInteract) {
        if(m_input->isKeyPressed(SDLK_e)) {
            if(m_speakingEntity) {
                m_speakingEntity->startDialogue();
            }
        }

        if(m_input->isKeyDown(SDL_BUTTON_LEFT) && m_selectedBlock) {
            if(m_inventory->getItem(m_selectedHotbox, 0)) m_inventory->getItem(m_selectedHotbox, 0)->onLeftClick(m_selectedBlock);
            m_selectedBlock->switchID((int)Categories::BlockIDs::AIR);
            m_inventory->updateWeight();
        }
        if(m_input->isKeyDown(SDL_BUTTON_RIGHT) && m_selectedBlock) {
            if(m_inventory->getItem(m_selectedHotbox, 0)) m_inventory->getItem(m_selectedHotbox, 0)->onRightClick(m_selectedBlock);
            m_inventory->updateWeight();
        }
        if(m_input->isKeyPressed(SDLK_r)) {
            ItemBlock* newItem = new ItemBlock(1, 0.0f, 1, (int)Categories::BlockIDs::DIRT);
            m_inventory->addItem(newItem);
        }
        if(m_input->isKeyPressed(SDLK_t)) {
            ItemBlock* newItem = new ItemBlock(2, 0.0f, 1, (int)Categories::BlockIDs::TORCH);
            m_inventory->addItem(newItem);
        }
        if(m_input->isKeyPressed(SDLK_i)) {
            m_inventoryOpen = !m_inventoryOpen;
        }
    }

    if(m_input->isKeyPressed(SDLK_1)) {
        m_selectedHotbox = 0;
    } else
    if(m_input->isKeyPressed(SDLK_2)) {
        m_selectedHotbox = 1;
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
