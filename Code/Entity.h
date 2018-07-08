#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Block.h"
#include "Chunk.h"

#include "PresetValues.h"

enum class AI_TYPE {
    FLY,
    WALK,
    SWIM
};

enum class DISABILITIES {
    NONE,
    BLIND,
    LAME,
    DEAF
};

enum class ATTACK_TYPE {
    RANGED,
    MELEE_ONLY,
    MAGIC
};

class Entity
{
    friend class Scripter;

    public:
        Entity();
        Entity(glm::vec2 position, Categories::Entity_Type type, unsigned int id);
        virtual ~Entity();

        void init(glm::vec2 position, Categories::Entity_Type type, unsigned int id);

        virtual void update(Chunk* chunks[WORLD_SIZE], float timeStep);
        virtual void draw(GLEngine::SpriteBatch& sb);
        void move(float timeStepVariable);

        void collide(std::vector<Entity*> entities, int chunkI);

        const Categories::Faction&     getFaction()      const { return m_faction; }
        const glm::vec2&               getPosition()     const { return m_position; }
        const glm::vec2&               getSize()         const { return m_size; }
        const int&                     getChunkIndex()         { return m_parentChunkIndex; }
              glm::vec2                getVelocity()     const { return m_velocity; }
              std::vector<glm::vec2>   getTargets()      const { return m_targets; }

        void                           setPosition(glm::vec2 pos)   { m_position = pos; }
        void                           setTargets(std::vector<glm::vec2> targets)  { m_targets = targets; m_curTarget = 0; }

        int m_parentChunkIndex = -1;

    protected:
        void setParentChunk(Chunk* worldChunks[WORLD_SIZE]);
        bool checkTilePosition(Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        void collideWithTile(glm::vec2 tilePos, bool ground = false);
        void updateLightLevel(Chunk* currentChunk);
        virtual void updateAI(Chunk* activeChunks[WORLD_SIZE]) {}
        virtual void updateMovement() {}

        bool m_controls[4]; // Up, down (crouching while on ground), left, right
        float m_speed = 0.02;
        float m_jumpHeight = 2.736f;
        std::vector<glm::vec2> m_targets;
        unsigned int m_curTarget = 0;

        GLEngine::GLTexture m_texture;
        float m_light = 0.6f;

        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;

        glm::vec2 m_velocity;

        Chunk* m_parentChunk = nullptr;

        bool m_transparent = false; // Unimplemented

        bool m_onGround = false;

        AI_TYPE m_ai = AI_TYPE::WALK;
        DISABILITIES m_disabilities = DISABILITIES::NONE;
        ATTACK_TYPE m_attackType = ATTACK_TYPE::MELEE_ONLY;

};

