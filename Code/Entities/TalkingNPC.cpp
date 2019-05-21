/*#include "TalkingNPC.h"

TalkingNPC::TalkingNPC(glm::vec2 position, unsigned int id, AudioManager* audioManager, ScriptQueue* sq) : Entity(position, audioManager, sq) {
    m_audioManager = audioManager;

    unsigned int mobId = Category_Data::dialogueMobData[id].mobId;
    m_questionId = Category_Data::dialogueMobData[id].dialogueId;

    m_dialogueStarted = false;

    //init(position, Categories::Entity_Type::MOB, mobId, sq);
}

TalkingNPC::~TalkingNPC()
{
    //dtor
}

void TalkingNPC::startDialogue() {
    m_dialogueStarted = true;
}

void TalkingNPC::draw(GLEngine::SpriteBatch& sb, float time, float xOffset) {
    glm::vec4 destRect = glm::vec4(m_position.x + xOffset * CHUNK_SIZE * TILE_SIZE, m_position.y, m_size.x * TILE_SIZE, m_size.y * TILE_SIZE);

    float x, y;
    if(m_velocity.x > m_speed) {
        x = (int)time*abs((int)m_velocity.x)+1 % 3;
        y = 1;
        m_flippedTexture = false;
    } else if(m_velocity.x < -m_speed) {
        x = (int)time*abs((int)m_velocity.x)+1 % 3;
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

    sb.draw(destRect, uvRect, m_texture.id, 0.0f, colour, glm::vec3(m_light));
}*/
