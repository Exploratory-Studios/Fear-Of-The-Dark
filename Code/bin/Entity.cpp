#include "Entity.h"

Entity::Entity()
{
    //ctor
}

Entity::~Entity()
{
    //dtor
}

void Entity::update() {

}

void Entity::draw() {

}

<<<<<<< HEAD
void Entity::collide(std::vector<Entity>& entities, std::vector<Block> blocks) {
    for(auto e : entities) {
        if(&e != this) {
            if((e.getFaction() > Categories::Faction::NEUTRAL && m_faction < Categories::Faction::NEUTRAL) ||
               (e.getFaction() < Categories::Faction::NEUTRAL && m_faction > Categories::Faction::NEUTRAL)) {
                /// They are different (opposing) factions. Make them collide with each other

                if(e.getPosition().x < m_position.x + m_size.x && e.getPosition().x + e.getSize().x > m_position.x) {
                    if(e.getPosition().y < m_position.y + m_size.y && e.getPosition().y + e.getSize().y > m_position.y) {
                        e.setPosition(e.getPosition() + m_position / glm::vec2(2.0f));
                        m_position = e.getPosition() + m_position / glm::vec2(-2.0f);
                    }
                }

            }
        }

        for(auto b : blocks) {

        }

    }
}

=======
>>>>>>> 98b8385241da9920f404ce8b5264336ac8b85c16
