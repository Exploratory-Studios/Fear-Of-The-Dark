#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Tile.h"
#include "Inventory.h"

#include "PresetValues.h"
#include "Entities/EntityFunctions.h"

#include "QuestManager.h"
#include "AudioManager.h"

#include "Scripting/ScriptQueue.h"

class Chunk;

enum class EntityIDs {
    NPC_PLAYER,
    NPC_NEUTRAL_QUESTGIVER_A,
    NPC_NEUTRAL_COMPANIONCUBE,
    ITEM
};

enum class EntityTypes {
    NPC,
    ITEM,
    PROJECTILE
};

class Entity
{
    friend class Scripter;

    public:
        Entity(glm::vec2 pos, unsigned int layer, MetaData data) : m_position(pos), m_layer(layer), m_metaData(data) {

        } // This is more of an abstract class
        virtual ~Entity();

        void update(World* world, float timeStep, unsigned int selfIndex);
        void tick(World* world);
        void draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset);
        void debugDraw(GLEngine::DebugRenderer& dr, float xOffset);
        void move(float timeStepVariable);

        virtual void collide(World* world, unsigned int entityIndex) {}

        // Getters
        const glm::vec2&               getPosition()     const { return m_position; }
        const glm::vec2&               getSize()         const { return m_size; }
              glm::vec2                getVelocity()     const { return m_velocity; }
              glm::vec4                getLightLevel()   const { return m_cornerLight; }
              float                    getAverageLightLevel() const { return (m_cornerLight.x + m_cornerLight.y + m_cornerLight.z + m_cornerLight.w) / 4.0f; }
              void                     setPosition(glm::vec2 pos)   { m_position = pos; }
        unsigned int                   getLayer()        const { return m_layer; }
        std::string                    getUUID()         const { return m_UUID; }
        EntityTypes                    getType()         const { return m_type; }

        void setToDraw(bool draw) { m_draw = draw; }

        void generateUUID(World* world);

        virtual std::string generateLuaValues() {
            std::string valScript = "selfX,selfY,selfXVel,selfYVel,selfID=";
            valScript += std::to_string(m_position.x) + ",";
            valScript += std::to_string(m_position.y) + ",";
            valScript += std::to_string(m_velocity.x) + ",";
            valScript += std::to_string(m_velocity.y) + ",";
            valScript += std::to_string(m_id);

            return valScript;
        }

    protected:
        // Collision
        bool checkTilePosition(World* world, std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        virtual void collideWithTile(glm::vec2 tilePos, bool ground = false);

        void updateLightLevel(World* world);
        virtual void interact(World* world);

        void moveUpLayer(World* world);
        void moveDownLayer(World* world);

        void die(World* world);

        virtual void onUpdate(World* world, float timeStep, unsigned int selfIndex) {}
        virtual void onTick(World* world) {}
        virtual void onDraw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) {}

        virtual void animate(int& x, int& y, bool& flip, float time) {}


        // Internal
        // Rendering
        void loadTexture();
        int m_animationFramesX = 1, m_animationFramesY = 1;

        MetaData m_metaData;

        bool m_controls[6]; // Up, down (crouching while on ground), left, right, backwards (layer++), forwards (layer--)

        bool m_exposedToSun = false;
        glm::vec4 m_cornerLight; // light values at each of the 4 corners. (clockwise, component 0 is at top left
        bool m_flippedTexture = false;

        glm::vec2 m_position = glm::vec2(0.0f);
        unsigned int m_layer = 0;
        glm::vec2 m_velocity = glm::vec2(0.0f);
        bool m_onGround = false;
        bool m_draw = true;

        EntityTypes m_type;


        // XML Attributes
        unsigned int m_id;
        GLuint m_textureId = (GLuint)-1;
        GLuint m_bumpMapId = (GLuint)-1;

        std::string m_texturePath;
        std::string m_bumpMapPath;

        glm::vec2 m_size = glm::vec2(1.0f);

        int m_updateScriptId = -1;
        int m_tickScriptId = -1;


        // UUID
        std::string m_UUID = "NO_UUID"; // This is a universally unique identifier, and is generated by concatenating the current gametime, a few random numbers, and the position of this entity on creation
};

Entity* createEntity(glm::vec2 pos, unsigned int layer, unsigned int id, MetaData data, bool loadTex);
Entity* createEntity(glm::vec2 pos, unsigned int layer, EntityIDs id, MetaData data, bool loadTex);







