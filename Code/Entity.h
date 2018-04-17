#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Block.h"
#include "Chunk.h"

#include "PresetValues.h"

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

        void                           setPosition(glm::vec2 pos)   { m_position = pos; }

        int m_parentChunkIndex = -1;

    protected:
        void setParentChunk(Chunk* worldChunks[WORLD_SIZE]);
        bool checkTilePosition(Tile* tiles[WORLD_HEIGHT][CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        void collideWithTile(glm::vec2 tilePos, bool ground = false);
        void updateLightLevel(Chunk* currentChunk);

        GLEngine::GLTexture m_texture;
        float m_light = 0.6f;

        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;

        glm::vec2 m_velocity;

        Chunk* m_parentChunk = nullptr;

        bool m_transparent = false; // Unimplemented

        bool m_onGround = false;

};

