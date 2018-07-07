#include "WalkingNPC.h"

WalkingNPC::WalkingNPC(glm::vec2 position, unsigned int id)
{
    init(position, Categories::Entity_Type::MOB, id);
}

void WalkingNPC::updateAI(Chunk* activeChunks[WORLD_SIZE]) {
    if(m_target) {
        m_controls[0] = false;
        m_controls[1] = false;
        m_controls[2] = false;
        m_controls[3] = false;

        if(m_targetPos.x > m_position.x + abs(m_velocity.x) * 1.2f) {
            m_controls[3] = true; // RIGHT
            m_controls[2] = false;
        }
        if(m_targetPos.x < m_position.x - abs(m_velocity.x) * 1.2f) {
            m_controls[2] = true; // LEFT
            m_controls[3] = false;
        }
    }
}

void WalkingNPC::updateMovement() {
    if(m_controls[0]) { // UP
        m_velocity.y = m_jumpHeight;
    }
    if(m_controls[1]) { // DOWN
        /// TODO: implement crouching
    }
    if(m_controls[2]) { // LEFT
        m_velocity.x -= m_speed;
    } else if(m_controls[3]) { // RIGHT
        m_velocity.x += m_speed;
    } else {
        m_velocity.x /= 5.0f;//= 5.0f;
    }

    if(m_velocity.x > MAX_SPEED) {
        m_velocity.x = MAX_SPEED;
    } else if(m_velocity.x < -MAX_SPEED) {
        m_velocity.x = -MAX_SPEED;
    }
}
