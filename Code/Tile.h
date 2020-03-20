#pragma once

#include <Vertex.h>
#include <GLTexture.h>
#include <SpriteBatch.h>
#include <SpriteFont.h>
#include <ParticleEngine2D.h>
#include <glm/glm.hpp>

#include "AudioManager.h"
#include "PresetValues.h"
#include "Scripting/ScriptQueue.h"

#include "SaveDataTypes.h"

class World;

class Tile
{
    friend class Scripter;
    friend class WorldIOManager;

    public:
        Tile();
        Tile(glm::vec2 pos, unsigned int layer, MetaData data) : m_pos(pos), m_layer(layer), m_metaData(data) { handleMetaDataInit(data); }
        Tile(glm::vec2 pos,
             unsigned int layer,
             GLuint textureId,
             GLuint bumpMapId,
             GLEngine::ColourRGBA8 colour,
             bool isSolid,
             unsigned int id,
             MetaData data) :
                m_pos(pos),
                m_layer(layer),
                m_textureId(textureId),
                m_bumpMapId(bumpMapId),
                m_colour(colour),
                m_solid(isSolid),
                m_id(id),
                m_metaData(data) { handleMetaDataInit(data); }
        virtual ~Tile() {}

        virtual void initParticles(GLEngine::ParticleEngine2D* engine) = 0;

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

        void operator=(const Tile& other) {
            m_pos = other.getPosition();
            m_size = other.getSize();
            m_textureId = other.getTextureID();
            m_bumpMapId = other.getBumpMapID();
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
            m_walkEffect = (SoundEffectIDs)other.getWalkedOnSoundEffectID();
        }

        glm::vec2       getPosition()                   const { return m_pos;                       }
        glm::vec2       getSize()                       const { return m_size;                      }
        GLuint          getTextureID()                  const { return m_textureId;                 }
        GLuint          getBumpMapID()                  const { return m_bumpMapId;         }
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
        unsigned int    getLayer()                      const { return m_layer;                     }
        float           getLightAtPoint(glm::vec2 posFromBL);

        void light_update(float light, Tile* source);
        void light_transferToNeighbours(float light, Tile* source);
        void light_removeSource(Tile* source, std::vector<Tile*>& newSources);
        void light_addEffected(Tile* effected);
        void light_reset();

        bool needsSunCheck() { return m_needsSunCheck; }

        // Don't fuck with my formatting

        virtual TileData getSaveData();

        void setAmbientLight(float light) { m_ambientLight = light; }
        void addAmbientLight(float light) { m_ambientLight += light; if(m_ambientLight < 0.0f) m_ambientLight = 0.0f; }
        void setToDraw(bool draw) { m_draw = draw; }

        void addCornerLight(float TL, float TR, float BR, float BL) {
            if(TL > 0) m_cornerLight.x += TL;
            if(TR > 0) m_cornerLight.y += TR;
            if(BR > 0) m_cornerLight.z += BR;
            if(BL > 0) m_cornerLight.w += BL;

            m_ambientLight = m_cornerLight.z; // Bottom right corner.
        }
        void subtractCornerLight(float TL, float TR, float BR, float BL) {
            if(TL > 0) m_cornerLight.x -= TL;
            if(TR > 0) m_cornerLight.y -= TR;
            if(BR > 0) m_cornerLight.z -= BR;
            if(BL > 0) m_cornerLight.w -= BL;

            m_ambientLight = m_cornerLight.z; // Bottom right corner.
        }

        void setNeedsSunCheck() { m_needsSunCheck = true; }
        void setPosition(glm::vec2 pos) { m_pos = pos; }
        void setToUpdate_heat() { m_updateHeat = true; } // Sends the signal to update heat
        void setSunLight(float light, bool left, bool right, bool top, bool bottom) {
            m_sunLight = light;

            if(top) {
                m_cornerSunlight.x = m_sunLight;
                m_cornerSunlight.y = m_sunLight;
            }
            if(right) {
                if(!top) {
                    m_cornerSunlight.y = m_sunLight;
                }
                if(bottom) {
                    m_cornerSunlight.z = m_sunLight;
                }
            }
            if(left) {
                if(!top) {
                    m_cornerSunlight.x = m_sunLight;
                }
                if(bottom) {
                    m_cornerSunlight.w = m_sunLight;
                }
            }

        }
        void resetSunlightCorners() {
            m_cornerSunlight = glm::vec4(0.0f);
        }

        virtual void update(World* world, float time, bool updateLighting, const float& sunlight);
        virtual void tick(World* world, float tickTime, const float& sunlight);
        virtual void draw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, int xOffset, int depthDifference);
        virtual void drawBackdrop(GLEngine::SpriteBatch& sb, int xOffset, int yOffset, float lightLevel);

        void destroy(World* world);

        virtual void interact_WalkedOn(ScriptQueue* sq) {}
        virtual void interact_LeftClicked(ScriptQueue* sq) {}
        virtual void interact_RightClicked(ScriptQueue* sq) {}
        // ... More interact functions

        void resetNeighboursLight(World* world);
        void setNeighboursLight(World* world);

        void calculateSunlight(World* world, float sunlight); // Determines and sets the sunlight corner values for this tile and surrounding tiles.

    protected:
        virtual void handleMetaDataInit(MetaData& data) { };
        virtual MetaData* getMetaData() { return new MetaData(); }

        virtual void onUpdate(World* world, float& time) { };
        virtual void onDraw(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf, glm::vec4& pos, float& depth) { }
        virtual void onTick(World* world, float& tickTime) { };
        virtual void onInteract(ScriptQueue* sq) {}
        virtual void onDestruction() { }

        virtual void loadTexture() = 0; // Fill this out with your own texture and bumpmap

        bool exposedToSun(World* world);

        glm::vec2 m_pos = glm::vec2(69.420f, 69.420f);
        unsigned int m_layer = 0; // Higher numbers are farther behind
        glm::vec2 m_size = glm::vec2(1, 1);

        GLuint m_textureId = (GLuint)-1;
        GLuint m_bumpMapId = (GLuint)-1;
        GLEngine::ColourRGBA8 m_colour = GLEngine::ColourRGBA8(255.0f, 255.0f, 255.0f, 255.0f);

        float m_ambientLight = 0.0f; // Really should be inherited light or something, as this light is inherited from surrounding blocks
        float m_emittedLight = 0.0f;
        float m_sunLight;
        glm::vec4 m_cornerSunlight;
        glm::vec4 m_cornerLight; // Clockwise, 0 is top left

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

        MetaData m_metaData;

};
