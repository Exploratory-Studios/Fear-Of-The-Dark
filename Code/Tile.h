#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <glm/glm.hpp>

#include "AudioManager.h"
#include "PresetValues.h"

class Chunk;

class Tile
{
    friend class Scripter;
    friend class WorldIOManager;

    public:
        Tile();
        Tile(glm::vec2 pos,
             GLuint textureId,
             GLuint backdropTextureId,
             GLEngine::ColourRGBA8 colour,
             bool isSolid,
             unsigned int id,
             Chunk* parent) :
                m_pos(pos),
                m_textureId(textureId),
                m_backdropTextureId(backdropTextureId),
                m_colour(colour),
                m_solid(isSolid),
                m_id(id),
                m_parentChunk(parent) { }
        virtual ~Tile() {}

        #ifdef DEV_CONTROLS
        std::string getPrintout() {
            std::string ret;
            ret =  "Position: X=" + std::to_string(m_pos.x) + ", Y=" + std::to_string(m_pos.y) + "\n";
            ret += "ID=" + std::to_string(m_id) + ", Solid=" + std::to_string(m_solid) + ", " + "LightLevel: Amb=" + std::to_string(m_ambientLight) + ", Sun=" + std::to_string(m_sunLight) + ", Emit=" + std::to_string(m_emittedLight) + "\n";
            ret += "Raw Temperature: " + std::to_string(m_temperature) + ", Real Temperature: " + std::to_string(getHeat()) + "\n";
            return ret;
        }
        #endif

        glm::vec2&      getPosition() { return m_pos;   }
        glm::vec2       getSize()     { return m_size;  }
        unsigned int    getID()       { return m_id;    }
        bool            isSolid()     { return m_solid; }
        bool            isNatural()   { return m_natural; }
        float           getLight();
        float           getAmbientLight() { return m_ambientLight; }
        float           getEmittedLight() { return m_emittedLight; }
        float           getSurroundingLight();
        float           getSurroundingHeat();
        unsigned int    getWalkedOnSoundEffectID() { return (unsigned int)m_walkEffect; }
        float           getSunLight()     { return m_sunLight;     }
        bool            isTransparent()   { return m_transparent;  }
        Chunk*          getParentChunk()  const { return m_parentChunk;  }
        float           getHeat(); // Used when gameplay mechanics are in play (modifiers in use, not used when tiles are inheriting temperatures)
        float           getRawHeat(); // Used whenever two tiles' temperatures are being compared, etc. (No modifiers excepting baseHeat)

        void            setAmbientLight(float light) { m_ambientLight = light; }
        void            setSunlight(float& tickTime)
        {
            if(m_needsSunCheck) { // check if this block is exposed to sunlight
                m_exposedToSun = exposedToSun();
                m_needsSunCheck = false;
            }
            if(m_exposedToSun) {
                m_sunLight = cos(tickTime / (DAY_LENGTH / 6.28318f)) / 2.0f + 0.5f;
            } else {
                m_sunLight = 0.0f;
            }
        }
        void setNeedsSunCheck() { m_needsSunCheck = true; }
        void setPosition(glm::vec2 pos) { m_pos = pos; }

        virtual void update(float time);
        virtual void tick(float tickTime);
        virtual void draw(GLEngine::SpriteBatch& sb, int xOffset);
        virtual void drawBackdrop(GLEngine::SpriteBatch& sb, int xOffset, int yOffset, float lightLevel);

        virtual void interact_WalkedOn() {}
        virtual void interact_LeftClicked() {}
        virtual void interact_RightClicked() {}
        // ... More interact functions, only used for special occasions :)

    protected:
        virtual void onUpdate(float& time) = 0;
        virtual void onTick(float& tickTime) = 0;

        bool exposedToSun();

        glm::vec2 m_pos = glm::vec2(0.0f, 0.0f);
        glm::vec2 m_size = glm::vec2(1, 1);

        GLuint m_textureId;
        GLuint m_backdropTextureId;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        float m_ambientLight = 0.0f; // Really should be inherited light or something, as this light is inherited from surrounding blocks
        float m_emittedLight = 0.0f;
        float m_sunLight = 0.0f;

        bool m_exposedToSun = false;
        bool m_needsSunCheck = true; // Was there a block just placed in this column?

        float m_emittedHeat = 0.0f;

        float m_temperature = 0.0f; // This should be affected by the biome, sunlight, and nearby heat sources.

        float m_lastLight; // This is used to make sure that we aren't giving other blocks light for no reason

        bool m_solid = true; // Don't collide if true: Air, water, etc.
        bool m_draw = false; // Don't draw if true: Air, etc.
        bool m_transparent = false; // 'Transmits' light?
        bool m_backdrop = false; // Does it draw a back wall?
        bool m_natural = false; // Is it a natural block? Does it get changed to stone when eras pass?

        unsigned int m_id;

        Chunk* m_parentChunk = nullptr;

        SoundEffectIDs m_walkEffect = SoundEffectIDs::WALK_DIRT;

};

#include "Chunk.h"
