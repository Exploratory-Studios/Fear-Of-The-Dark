#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <glm/glm.hpp>

#include "PresetValues.h"

class Chunk;

class Tile
{
    friend class Scripter;

    public:
        Tile();
        Tile(glm::vec2 pos,
             GLEngine::GLTexture texture,
             GLEngine::ColourRGBA8 colour,
             bool isSolid,
             unsigned int id,
             Chunk* parent) :
                m_pos(pos),
                m_texture(texture),
                m_colour(colour),
                m_solid(isSolid),
                m_id(id),
                m_parentChunk(parent) { }

        glm::vec2       getPosition() { return m_pos;   }
        glm::vec2       getSize()     { return m_size;  }
        unsigned int    getID()       { return m_id;    }
        bool            isSolid()     { return m_solid; }
        float           getLight();
        float           getAmbientLight() { return m_ambientLight; }

        void            setAmbientLight(float light) { m_ambientLight = light; }

        virtual void update(float time);
        virtual void tick(int tickTime);
        virtual void draw(GLEngine::SpriteBatch& sb);

    protected:
        bool exposedToSun();

        glm::vec2 m_pos;
        glm::vec2 m_size = glm::vec2(1, 1);

        GLEngine::GLTexture m_texture;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);
        float m_ambientLight = 0.0f;
        float m_emittedLight = 0.0f;
        float m_sunLight = 0.0f;

        unsigned int m_id;

        bool m_solid = true; // Don't collide if true: Air, water, etc.
        bool m_transparent = false; // Don't draw if true: Air, etc.

        Chunk* m_parentChunk = nullptr;

};

#include "Chunk.h"
