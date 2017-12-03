#include "Player.h"

#include "PresetValues.h"

Player::Player() {

}

Player::Player(glm::vec2 position, GLEngine::InputManager* input) : m_input(input)
{
    init(position, Categories::Entity_Type::MOB, 1);
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

void Player::drawGUI(GLEngine::SpriteBatch& sb) {

    glm::vec4 fullUV = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    GLEngine::ColourRGBA8 fullColour(255, 255, 255, 255);

    {
        int hotbarImgId = GLEngine::ResourceManager::getTexture("../Assets/GUI/Player/Hotbar.png").id;

        glm::vec4 destRect(HOTBAR_X, HOTBAR_Y, HOTBAR_BOX_SIZE * HOTBAR_BOX_NUM, HOTBAR_BOX_SIZE);

        sb.draw(destRect, fullUV, hotbarImgId, 0.0f, fullColour);
    }
}

void Player::update(Chunk* chunks[WORLD_SIZE]) {
    if(m_input->isKeyDown(SDLK_w)) {
        if(m_onGround) {
            m_velocity.y = 2.0f; // jumpHeight(blocks) = yVel * timestep / gravity(0.07) / TILE_SIZE(8)
            m_onGround = false;
        }
    }

    if(m_input->isKeyDown(SDLK_d)) {
        m_velocity.x += 0.1f * m_inventory->getSpeedMultiplier();
    } else if(m_input->isKeyDown(SDLK_a)) {
        m_velocity.x -= 0.1f * m_inventory->getSpeedMultiplier();
    } else {
        m_velocity.x *= 0.8f;
    }

    if(m_input->isKeyDown(SDL_BUTTON_LEFT) && m_selectedBlock) {
        m_selectedBlock->switchID((unsigned int)Categories::BlockIDs::AIR);
    }
    if(m_input->isKeyDown(SDL_BUTTON_RIGHT) && m_selectedBlock) {
        m_selectedBlock->switchID((unsigned int)Categories::BlockIDs::DIRT);
    }
    if(m_input->isKeyDown(SDLK_a)) {
        Item newItem;
        newItem.
        m_inventory->addItem(newItem);
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
