#include "WalkingNPC.h"

WalkingNPC::WalkingNPC(glm::vec2 position, unsigned int id)
{
    init(position, Categories::Entity_Type::MOB, id);
}
#include <iostream>
void WalkingNPC::updateAI(Chunk* activeChunks[WORLD_SIZE]) {
    m_controls[0] = false;
    m_controls[1] = false;
    m_controls[2] = false;
    m_controls[3] = false;

    if(m_targets.size() > m_curTarget) {
        float modifier = 0.0f;
        if(m_curTarget == m_targets.size()-1) modifier = abs(m_velocity.x) * 1.25f;
        if(m_targets[m_curTarget].x + m_size.x * TILE_SIZE / 2.0f > m_position.x + modifier) {
            m_controls[3] = true; // RIGHT
            m_controls[2] = false;
        }
        if(m_targets[m_curTarget].x + m_size.x * TILE_SIZE / 2.0f < m_position.x - modifier) {
            m_controls[2] = true; // LEFT
            m_controls[3] = false;
        }
        if((int)m_targets[m_curTarget].y - (int)m_position.y > 1.0f) {
            m_controls[0] = true;
        }
        if(abs(m_position.x + m_size.x * TILE_SIZE / 2.0f - m_targets[m_curTarget].x) <= 1.0f && abs(m_position.y - m_targets[m_curTarget].y) <= 1.0f) {
            m_curTarget++;
            if(m_curTarget >= m_targets.size()) {
                m_targets.clear();
                m_curTarget = 0;
            }
        }
    }
}

void WalkingNPC::updateMovement() {
    if(m_controls[0]) { // UP
        if(m_onGround) {
            m_velocity.y = m_jumpHeight; // y=(jumpHeight+-0.098*60*s^2)  initial jump power is the absolute of the x at 0. jumpheight is in eights of tiles and you must add 4
            m_onGround = false;
        }
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
