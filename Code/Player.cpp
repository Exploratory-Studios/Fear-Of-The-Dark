#include "Player.h"

#include "PresetValues.h"
#include "Item.h"

Player::Player() {
    m_inventory = new Inventory();
}

Player::Player(glm::vec2 position, GLEngine::InputManager* input) : m_input(input)
{
    init(position, Categories::Entity_Type::MOB, 1);
    m_inventory = new Inventory();
}

Player::~Player()
{
    //dtor
}

void Player::draw(GLEngine::SpriteBatch& sb) {
    glm::vec4 destRect = glm::vec4(m_position.x, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);
    glm::vec4 uvRect = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);

    GLEngine::ColourRGBA8 colour(255 * m_light, 255 * m_light, 255 * m_light, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour);

    if(m_selectedBlock) { // Cursor box selection
        int cursorImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/Cursor.png").id;

        glm::vec4 cursorDestRect(m_selectedBlock->getPosition().x * TILE_SIZE, m_selectedBlock->getPosition().y * TILE_SIZE, m_selectedBlock->getSize().x * TILE_SIZE, m_selectedBlock->getSize().y * TILE_SIZE);
        sb.draw(cursorDestRect, uvRect, cursorImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
    }
}

void Player::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {

    glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

    {
        int hotbarImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/Hotbar.png").id;
        int hotbarSelectImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/HotbarSelection.png").id;

        sb.begin();

        for(int i = 0; i < HOTBAR_BOX_NUM; i++) {

            glm::vec4 uv(i * (1.0 / HOTBAR_BOX_NUM), 0.0f, (1.0 / HOTBAR_BOX_NUM), 1.0f);
            glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
            sb.draw(destRect, uv, hotbarImgId, 0.0f, fullColour);

            {
                if(m_inventory->getItem(i, 0).getID() != -1) {
                    glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
                                    HOTBAR_BOX_SIZE / 4,
                                    HOTBAR_BOX_SIZE,
                                    HOTBAR_BOX_SIZE);
                    glm::vec4 itemUV(0, 0, 1, 1);
                    int itemImgId = GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_inventory->getItem(i, 0).getID()].texturePath).id;

                    sb.draw(destRect, itemUV, itemImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
                }
            }
        }

        sb.end();
        sb.renderBatch();

        for(int i = 0; i < HOTBAR_BOX_NUM; i++) {
            if(m_inventory->getItem(i, 0).getID() != -1) {
                glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i,
                                    HOTBAR_BOX_SIZE / 4,
                                    HOTBAR_BOX_SIZE,
                                    HOTBAR_BOX_SIZE);
                glm::vec4 itemUV(0, 0, 1, 1);
                int itemImgId = GLEngine::ResourceManager::getTexture(Category_Data::itemData[m_inventory->getItem(i, 0).getID()].texturePath).id;

                sb.begin();
                sb.draw(destRect, itemUV, itemImgId, 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255));
                sb.end();
                sb.renderBatch();

                sb.begin();
                sf.draw(sb, std::to_string(m_inventory->getItem(i, 0).getQuantity()).c_str(), glm::vec2(destRect.x + INVENTORY_BOX_SIZE * 9/10, destRect.y + INVENTORY_BOX_SIZE - 96.0f * 0.35f), glm::vec2(0.35f), 0.0f, GLEngine::ColourRGBA8(255, 255, 255, 255), GLEngine::Justification::RIGHT);
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

void Player::update(Chunk* chunks[WORLD_SIZE], float timeStep) {

    updateLightLevel(m_parentChunk);

    if(m_input->isKeyDown(SDLK_w)) {
        if(m_onGround) {
            m_velocity.y = 2.736f; // y=(jumpHeight+-0.098*60*s^2)  initial jump power is the absolute of the x at 0. jumpheight is in eights of tiles and you must add 4
            m_onGround = false;
        }
    }

    if(m_input->isKeyDown(SDLK_d)) {
        if(m_velocity.x < 0.0f) m_velocity.x /= 5.0f;
        m_velocity.x += 0.2f * m_inventory->getSpeedMultiplier();
    } else if(m_input->isKeyDown(SDLK_a)) {
        if(m_velocity.x > 0.0f) m_velocity.x /= 5.0f;
        m_velocity.x -= 0.2f * m_inventory->getSpeedMultiplier();
    } else {
        m_velocity.x *= 0.9f;
    }

    if(m_velocity.x > MAX_SPEED * m_inventory->getSpeedMultiplier()) {
        m_velocity.x = MAX_SPEED * m_inventory->getSpeedMultiplier();
    } else if(m_velocity.x < -MAX_SPEED * m_inventory->getSpeedMultiplier()) {
        m_velocity.x = -MAX_SPEED * m_inventory->getSpeedMultiplier();
    }

    if(m_input->isKeyDown(SDL_BUTTON_LEFT) && m_selectedBlock) {
        if(m_handItem) m_handItem->onLeftClick(m_selectedBlock);
        m_selectedBlock->setAmbientLight(1.0f);
    }
    if(m_input->isKeyDown(SDL_BUTTON_RIGHT) && m_selectedBlock) {
        if(m_handItem) m_handItem->onRightClick(m_selectedBlock);
    }
    if(m_input->isKeyPressed(SDLK_l)) {
        Item newItem(0, 0.5, 1);
        m_inventory->addItem(newItem);
    }
    if(m_input->isKeyPressed(SDLK_i)) {
        m_inventoryOpen = !m_inventoryOpen;
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


    setParentChunk(chunks);
}

void Player::updateMouse(Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera) {
    glm::vec2 mousePos = worldCamera->convertScreenToWorld(m_input->getMouseCoords());

    mousePos = glm::vec2(floor(mousePos.x / TILE_SIZE), floor(mousePos.y / TILE_SIZE)); // Changes it to grid-space

    unsigned int chunkIndex = floor(mousePos.x / CHUNK_SIZE);

    mousePos.x -= chunkIndex * CHUNK_SIZE; // Make sure that coords are staying inside of the selected chunk.

    if(mousePos.x >= 0 &&
       mousePos.x + (chunkIndex * CHUNK_SIZE) < WORLD_SIZE * CHUNK_SIZE &&
       mousePos.y >= 0 &&
       mousePos.y < WORLD_HEIGHT)
           m_selectedBlock = static_cast<Block*>(chunks[chunkIndex]->tiles[(unsigned int)mousePos.y][(unsigned int)mousePos.x]);
}
