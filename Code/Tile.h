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
    friend class WorldIOManager;

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

        #ifdef DEV_CONTROLS
        std::string getPrintout() {
            std::string ret;
            ret =  "Position: X=" + std::to_string(m_pos.x) + ", Y=" + std::to_string(m_pos.y) + "\n";
            ret += "ID=" + std::to_string(m_id) + ", Solid=" + std::to_string(m_solid) + ", " + "LightLevel: Amb=" + std::to_string(m_ambientLight) + ", Sun=" + std::to_string(m_sunLight) + ", Emit=" + std::to_string(m_emittedLight) + "\n";
            return ret;
        }
        #endif

        glm::vec2       getPosition() { return m_pos;   }
        glm::vec2       getSize()     { return m_size;  }
        unsigned int    getID()       { return m_id;    }
        bool            isSolid()     { return m_solid; }
        float           getLight();
        float           getAmbientLight() { return m_ambientLight; }
        float           getEmittedLight() { return m_emittedLight; }
        float           getSurroundingLight();
        float           getSunLight()     { return m_sunLight;     }
        bool            isTransparent()   { return m_transparent;  }
        Chunk*          getParentChunk()  const { return m_parentChunk;  }

        void            setAmbientLight(float light) { m_ambientLight = light; }
        void            setSunlight(float& tickTime)
        {
            if(exposedToSun()) { // check if this block is exposed to sunlight
                m_sunLight = cos(tickTime / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
            } else {
                m_sunLight = 0.0f;
            }
        }

        virtual void update(float time);
        virtual void tick(float tickTime);
        virtual void draw(GLEngine::SpriteBatch& sb, int xOffset);

    protected:
        virtual void onUpdate(float& time) = 0;
        virtual void onTick(float& tickTime) = 0;

        bool exposedToSun();

        glm::vec2 m_pos;
        glm::vec2 m_size = glm::vec2(1, 1);

        GLEngine::GLTexture m_texture;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        float m_ambientLight = 0.0f; // Really should be inherited light or something, as this light is inherited from surrounding blocks
        float m_emittedLight = 0.0f;
        float m_sunLight = 0.0f;

        float m_lastLight; // This is used to make sure that we aren't giving other blocks light for no reason

        bool m_solid = true; // Don't collide if true: Air, water, etc.
        bool m_draw = false; // Don't draw if true: Air, etc.
        bool m_transparent = false; // 'Transmits' light?

        unsigned int m_id;

        Chunk* m_parentChunk = nullptr;

};

#include "Chunk.h"
