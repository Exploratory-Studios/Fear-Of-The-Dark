#include "../Entity.h"


class WalkingNPC : public Entity
{
    public:
        WalkingNPC(glm::vec2 position, unsigned int id);

    protected:
        virtual void updateAI(Chunk* activeChunks[WORLD_SIZE]);
        virtual void updateMovement();



        /*
        void setParentChunk(Chunk* worldChunks[WORLD_SIZE]);
        bool checkTilePosition(Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        void collideWithTile(glm::vec2 tilePos, bool ground = false);
        void updateLightLevel(Chunk* currentChunk);

        GLEngine::GLTexture m_texture;
        float m_light = 0.6f;

        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;

        glm::vec2 m_velocity = glm::vec2((float)NULL, (float)NULL);

        Chunk* m_parentChunk = nullptr;

        bool m_transparent = false; // Unimplemented

        bool m_onGround = false;

        AI_TYPE m_ai = AI_TYPE::WALK;
        DISABILITIES m_disabilities = DISABILITIES::NONE;
        ATTACK_TYPE m_attackType = ATTACK_TYPE::MELEE_ONLY;
        */
};
