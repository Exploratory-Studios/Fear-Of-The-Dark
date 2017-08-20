#include "Player.h"

Player::Player() {

}

Player::Player(glm::vec2 position)
{
    m_size = glm::vec2(1.0f, 2.0f);
    m_faction = Categories::Faction::GOOD;
    m_texture = GLEngine::ResourceManager::getTexture("../Assets/Textures/Mobs/Mob0.png");
    m_position = position;
}

Player::~Player()
{
    //dtor
}
