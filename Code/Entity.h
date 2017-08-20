#pragma once

#include <vector>

#include "Block.h"

#include "PresetValues.h"

class Entity
{
    public:
        Entity();
        Entity(glm::vec2 position, Categories::Entity_Type type, unsigned int id);
        virtual ~Entity();

        void init(glm::vec2 position, Categories::Entity_Type type, unsigned int id);

        virtual void update();
        virtual void draw(GLEngine::SpriteBatch& sb);

        void collide(std::vector<Entity*>& entities, Tile tiles[WORLD_HEIGHT][CHUNK_SIZE]);
        void checkTilePosition(Tile tiles[WORLD_HEIGHT][CHUNK_SIZE] , std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        void collideWithTile(glm::vec2 tilePos);

        const Categories::Faction&     getFaction()  const { return m_faction; }
        const glm::vec2&               getPosition() const { return m_position; }
        const glm::vec2&               getSize()     const { return m_size; }

        void                           setPosition(glm::vec2 pos)   { m_position = pos; }

    private:
        GLEngine::GLTexture m_texture;

        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;

};

