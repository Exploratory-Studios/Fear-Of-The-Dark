#pragma once

#include <glm/glm.hpp>
#include <Vertex.h>
#include <ResourceManager.h>
#include <GLTexture.h>

#include "Tile.h"
#include "Categories.h"
#include "Scripting/ScriptQueue.h"

class Block : public Tile
{
    public:
        Block(glm::vec2 pos, unsigned int layer, Chunk* parent, MetaData metaData);
        virtual ~Block() { }

        virtual void onInteract(ScriptQueue* sq) { }; // Class already has parent chunk

        void operator=(const Block& other) {
            m_pos = other.getPosition();
            m_size = other.getSize();
            m_textureId = other.getTextureID();
            m_backdropTextureId = other.getBackdropTextureID();
            m_colour = GLEngine::ColourRGBA8(255, 255, 255, 255);
            m_emittedLight = other.getEmittedLight();
            m_emittedHeat = other.getEmittedHeat();
            m_needsSunCheck = true;
            m_solid = other.isSolid();
            m_draw = other.doDraw();
            m_transparent = other.isTransparent();
            m_backdrop = other.doDrawBackdrop();
            m_natural = other.isNatural();
            m_id = other.getID();
            m_parentChunk = other.getParentChunk();
            m_walkEffect = (SoundEffectIDs)other.getWalkedOnSoundEffectID();
            m_walkParticle = (ParticleIDs)other.getWalkedOnParticleID();
        }

    protected:
        virtual void onTick(float& tickTime) { }
        virtual void onUpdate(float& time) { }

        /// Protected, taken from "Tile" class
        /*
        glm::vec2 m_pos = glm::vec2(69.420f, 69.420f);
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

        bool m_solid = true; // Don't collide if true: Air, water, background pillars, etc. -> This is the 'passable' aspect of blocks
        bool m_draw = false; // Don't draw if true: Air, etc.
        bool m_transparent = false; // 'Transmits' light?
        bool m_backdrop = false; // Does it draw a back wall?
        bool m_natural = false; // Is it a natural block? Does it get changed to stone when eras pass?

        unsigned int m_id;

        Chunk* m_parentChunk = nullptr;

        SoundEffectIDs m_walkEffect = SoundEffectIDs::WALK_DIRT;
        ParticleIDs m_walkParticle = ParticleIDs::DIRT_PARTICLE;
        */
};

#include "Blocks.h"
