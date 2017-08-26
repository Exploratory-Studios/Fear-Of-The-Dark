#include "Player.h"

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

void Player::update(Chunk* chunks[WORLD_SIZE]) {
    if(m_input->isKeyDown(SDLK_w)) m_velocity.y += 0.1f; /// Speed variable (how fast the entity is currently going & how fast one should accelerate)
    if(m_input->isKeyDown(SDLK_s)) m_velocity.y -= 0.1f; /// use speed for collision PREDICTION(!)
    if(m_input->isKeyDown(SDLK_d)) m_velocity.x += 0.1f;
    if(m_input->isKeyDown(SDLK_a)) m_velocity.x -= 0.1f;

    setParentChunk(chunks);
}
