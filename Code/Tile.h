#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <glm/glm.hpp>

#include "PresetValues.h"

class Tile
{
    friend class Scripter;

    public:
        Tile();
        Tile(glm::vec2 pos,
             GLEngine::GLTexture texture,
             GLEngine::ColourRGBA8 colour,
             bool isSolid,
             unsigned int id) :
                m_pos(pos),
                m_texture(texture),
                m_colour(colour),
                m_solid(isSolid),
                m_id(id) { }

        glm::vec2       getPosition() { return m_pos;   }
        glm::vec2       getSize()     { return m_size;  }
        unsigned int    getID()       { return m_id;    }
        bool            isSolid()     { return m_solid; }

        virtual void update();
        virtual void draw(GLEngine::SpriteBatch& sb);

    protected:
        glm::vec2 m_pos;
        glm::vec2 m_size = glm::vec2(1, 1);

        GLEngine::GLTexture m_texture;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        unsigned int m_id;

        bool m_solid = false;
        bool m_transparent = false; // Don't draw if true: Air, etc.

};
