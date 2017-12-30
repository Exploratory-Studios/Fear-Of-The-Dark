#include "Player.h"

#include "PresetValues.h"

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

    GLEngine::ColourRGBA8 colour(255, 255, 255, 255);

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour);

    if(m_selectedBlock) { // Cursor box selection
        int cursorImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/Cursor.png").id;

        glm::vec4 cursorDestRect(m_selectedBlock->getPosition().x * TILE_SIZE, m_selectedBlock->getPosition().y * TILE_SIZE, m_selectedBlock->getSize().x * TILE_SIZE, m_selectedBlock->getSize().y * TILE_SIZE);
        sb.draw(cursorDestRect, uvRect, cursorImgId, 0.0f, colour);
    }
}

void Player::drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf) {

    glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

    {
        int hotbarImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/HotbarBox.png").id;

        sb.begin();

        for(int i = 0; i < HOTBAR_BOX_NUM; i++) {
            glm::vec4 destRect(HOTBAR_BOX_SIZE / 4 + (HOTBAR_BOX_SIZE + HOTBAR_BOX_PADDING) * i, HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE, HOTBAR_BOX_SIZE);
            sb.draw(destRect, fullUV, hotbarImgId, 0.0f, fullColour);
        }

        sb.end();
        sb.renderBatch();
    }

    {
        if(m_inventoryOpen) {

            m_inventory->draw(HOTBAR_BOX_SIZE / 4, HOTBAR_BOX_SIZE * 1.5, sb, sf);

        }
    }
}

void Player::update(Chunk* chunks[WORLD_SIZE]) {
    if(m_input->isKeyDown(SDLK_w)) {
        if(m_onGround) {
            m_velocity.y = 2.352f; // jumpHeight(blocks) = yVel(2) * timestep(1 avg.) / gravity(0.098) / TILE_SIZE(8)
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
    }
    if(m_input->isKeyDown(SDL_BUTTON_RIGHT) && m_selectedBlock) {
        if(m_handItem) m_handItem->onRightClick(m_selectedBlock);
    }
    if(m_input->isKeyDown(SDLK_l)) {
        BlockItem newItem(0, 0.5f, 1);
        m_inventory->addItem(newItem);
    }
    if(m_input->isKeyPressed(SDLK_i)) {
        m_inventoryOpen = !m_inventoryOpen;
    }


    if(m_input->isKeyPressed(SDLK_1)) {
        m_selectedHotbox = 1;
    } else
    if(m_input->isKeyPressed(SDLK_2)) {
        m_selectedHotbox = 2;
    } else
    if(m_input->isKeyPressed(SDLK_3)) {
        m_selectedHotbox = 3;
    } else
    if(m_input->isKeyPressed(SDLK_4)) {
        m_selectedHotbox = 4;
    } else
    if(m_input->isKeyPressed(SDLK_5)) {
        m_selectedHotbox = 5;
    } else
    if(m_input->isKeyPressed(SDLK_6)) {
        m_selectedHotbox = 6;
    } else
    if(m_input->isKeyPressed(SDLK_7)) {
        m_selectedHotbox = 7;
    } else
    if(m_input->isKeyPressed(SDLK_8)) {
        m_selectedHotbox = 8;
    } else
    if(m_input->isKeyPressed(SDLK_9)) {
        m_selectedHotbox = 9;
    } else
    if(m_input->isKeyPressed(SDLK_0)) {
        m_selectedHotbox = 0;
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
           m_selectedBlock = static_cast<Block*>(&chunks[chunkIndex]->tiles[(unsigned int)mousePos.y][(unsigned int)mousePos.x]);
}
