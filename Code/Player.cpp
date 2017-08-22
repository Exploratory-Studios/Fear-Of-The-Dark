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
#include <iostream>
void Player::update(Chunk* chunks[WORLD_SIZE]) {
    if(m_input->isKeyDown(SDLK_w)) m_position.y += 1.0f; /// Speed variable (how fast the entity is currently going & how fast one should accelerate)
    if(m_input->isKeyDown(SDLK_s)) m_position.y -= 1.0f; /// use speed for collision PREDICTION(!)
    if(m_input->isKeyDown(SDLK_d)) m_position.x += 1.0f;
    if(m_input->isKeyDown(SDLK_a)) m_position.x -= 1.0f;

    setParentChunk(chunks);
    std::cout << m_parentChunkIndex << std::endl;
}
