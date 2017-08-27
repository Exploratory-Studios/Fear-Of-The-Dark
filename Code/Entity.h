#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Block.h"
#include "Chunk.h"

#include "PresetValues.h"

class Entity
{
    public:
        Entity();
        Entity(glm::vec2 position, Categories::Entity_Type type, unsigned int id);
        virtual ~Entity();

        void init(glm::vec2 position, Categories::Entity_Type type, unsigned int id);

        virtual void update(Chunk* chunks[WORLD_SIZE]);
        virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::DebugRenderer& dr);
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
        void checkTilePosition(Tile tiles[WORLD_HEIGHT][CHUNK_SIZE], int chunkI, std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        void collideWithTile(glm::vec2 tilePos);

        GLEngine::GLTexture m_texture;

        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;



        glm::vec2 m_testPos;



        glm::vec2 m_velocity;

        Chunk* m_parentChunk = nullptr;

        bool m_onGround = false;
        bool m_wasOnGround = false;

};

