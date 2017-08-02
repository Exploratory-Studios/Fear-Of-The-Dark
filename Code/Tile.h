#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <glm/glm.hpp>

#define TILE_SIZE 4 /// This sets how big each square on the tile grid is
#define TILE_SIZE 2 /// This sets how big each square on the tile grid is

class Tile
{
    public:
        Tile();
        Tile(glm::vec2 pos,
             GLEngine::GLTexture texture,
             GLEngine::ColourRGBA8 colour,
             unsigned int id) :
                m_pos(pos),
                m_texture(texture),
                m_colour(colour),
                m_id(id) { }

        glm::vec2       getPosition() { return m_pos; }
        unsigned int    getID()       { return m_id;  }

        virtual void update();
        virtual void draw(GLEngine::SpriteBatch& sb);

    protected:
        glm::vec2 m_pos;
        glm::vec2 m_size = glm::vec2(TILE_SIZE, TILE_SIZE);

        GLEngine::GLTexture m_texture;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        unsigned int m_id;

};
