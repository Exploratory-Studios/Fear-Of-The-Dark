#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <glm/glm.hpp>

#include "AudioManager.h"
#include "PresetValues.h"

#include "SaveDataTypes.h"

class World;

class Tile
{
    friend class Scripter;
    friend class WorldIOManager;

    public:
        Tile();
        Tile(glm::vec2 pos,
             unsigned int layer,
             GLuint textureId,
             GLuint backdropTextureId,
             GLEngine::ColourRGBA8 colour,
             bool isSolid,
             unsigned int id,
             MetaData data) :
                m_pos(pos),
                m_layer(layer),
                m_textureId(textureId),
                m_backdropTextureId(backdropTextureId),
                m_colour(colour),
                m_solid(isSolid),
                m_id(id),
                m_metaData(data) { handleMetaDataInit(data); }
        virtual ~Tile() {}

        #ifdef DEV_CONTROLS
        std::string getPrintout(World* world) {
            std::string ret;
            ret =  "Position: X=" + std::to_string(m_pos.x) + ", Y=" + std::to_string(m_pos.y) + "\n";
            ret += "ID=" + std::to_string(m_id) + ", Solid=" + std::to_string(m_solid) + ", " + "LightLevel: Amb=" + std::to_string(m_ambientLight) + ", Sun=" + std::to_string(m_sunLight) + ", Emit=" + std::to_string(m_emittedLight) + "\n";
            ret += "ExposedToSun: " + std::to_string(m_exposedToSun) + "\n";
            ret += "Raw Temperature: " + std::to_string(m_temperature) + ", Real Temperature: " + std::to_string(getHeat(world)) + "\n";
            return ret;
        }
        #endif

        glm::vec2       getPosition()                   const { return m_pos;                       }
        glm::vec2       getSize()                       const { return m_size;                      }
        GLuint          getTextureID()                  const { return m_textureId;                 }
        GLuint          getBackdropTextureID()          const { return m_backdropTextureId;         }
        unsigned int    getID()                         const { return m_id;                        }
        bool            isSolid()                       const { return m_solid;                     }
        bool            isNatural()                     const { return m_natural;                   }
        float           getLight();
        float           getAmbientLight()               const { return m_ambientLight;              }
        float           getEmittedLight()               const { return m_emittedLight;              }
        //float           getSurroundingLight();
        float           getSurroundingHeat(World* world);
        unsigned int    getWalkedOnSoundEffectID()      const { return (unsigned int)m_walkEffect;  }
        float           getSunLight()                   const { return m_sunLight;                  }
        bool            isExposedToSunlight()           const { return m_exposedToSun;              }
        bool            isTransparent()                 const { return m_transparent;               }
        float           getHeat(World* world);                      // Used when gameplay mechanics are in play (modifiers in use, not used when tiles are inheriting temperatures)
        float           getRawHeat();                   // Used whenever two tiles' temperatures are being compared, etc. (No modifiers excepting baseHeat)
        float           getEmittedHeat()                const { return m_emittedHeat;               }
        bool            doDraw()                        const { return m_draw;                      }
        bool            doDrawBackdrop()                const { return m_backdrop;                  }
        ParticleIDs     getWalkedOnParticleID()         const { return m_walkParticle;              }
        unsigned int    getLayer()                      const { return m_layer;                     }

        void light_update(float light, Tile* source);
        void light_transferToNeighbours(float light, Tile* source);
        void light_removeSource(Tile* source, std::vector<Tile*>& newSources);
        void light_addEffected(Tile* effected);
        void light_reset();

        bool needsSunCheck() { return m_needsSunCheck; }

        /// TODO: Don't fuck with my formatting

        virtual TileData getSaveData();

        void setAmbientLight(float light) { m_ambientLight = light; }
        void addAmbientLight(float light) { m_ambientLight += light; if(m_ambientLight < 0.0f) m_ambientLight = 0.0f; }
        void setNeedsSunCheck() { m_needsSunCheck = true; }
        void setPosition(glm::vec2 pos) { m_pos = pos; }
        void setToUpdate_heat() { m_updateHeat = true; } // Sends the signal to update heat
        void setSunLight(float sunlight) { m_sunLight = sunlight; }

        virtual void update(World* world, float time, bool updateLighting);
        virtual void tick(World* world, float tickTime, const float& sunlight);
        virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, int depthDifference);
        virtual void drawBackdrop(GLEngine::SpriteBatch& sb, int xOffset, int yOffset, float lightLevel);

        void destroy(World* world);

        virtual void interact_WalkedOn() {}
        virtual void interact_LeftClicked() {}
        virtual void interact_RightClicked() {}
        // ... More interact functions

        void resetNeighboursLight(World* world);
        void setNeighboursLight(World* world);

    protected:
        virtual void handleMetaDataInit(MetaData& data) = 0;
        virtual MetaData* getMetaData() { return new MetaData(); }

        virtual void onUpdate(World* world, float& time) = 0;
        virtual void onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth) {}
        virtual void onTick(World* world, float& tickTime) = 0;

        virtual void loadTexture() = 0; // Fill this out with your own texture

        bool exposedToSun(World* world);

        glm::vec2 m_pos = glm::vec2(69.420f, 69.420f);
        unsigned int m_layer = 0; // Higher numbers are farther behind
        glm::vec2 m_size = glm::vec2(1, 1);

        GLuint m_textureId = (GLuint)-1;
        GLuint m_backdropTextureId;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        float m_ambientLight = 0.0f; // Really should be inherited light or something, as this light is inherited from surrounding blocks
        float m_emittedLight = 0.0f;
        float m_sunLight = 0.0f;

        bool m_exposedToSun = false;
        bool m_needsSunCheck = true; // Was there a block just placed in this column?

        float m_emittedHeat = 0.0f;

        float m_temperature = 0.0f; // This should be affected by the biome, sunlight, and nearby heat sources.
        float m_lastTemperature = 0.0f;

        bool m_updateHeat = true;

        bool m_solid = true; // Don't collide if true: Air, water, pillars, etc. -> This is the 'passable' aspect of blocks
        bool m_draw = false; // Don't draw if true: Air, etc.
        bool m_transparent = false; // 'Transmits' light?
        bool m_backdrop = false; // Does it draw a back wall?
        bool m_natural = false; // Is it a natural block? Does it get changed to stone when eras pass?

        unsigned int m_id;

        SoundEffectIDs m_walkEffect = SoundEffectIDs::WALK_DIRT;
        ParticleIDs m_walkParticle = ParticleIDs::DIRT_PARTICLE;

        MetaData m_metaData;

};
