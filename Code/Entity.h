#pragma once

#include <vector>
#include <DebugRenderer.h>

#include "Block.h"
#include "Inventory.h"

#include "PresetValues.h"
#include "Entities/EntityFunctions.h"

#include "QuestManager.h"
#include "AudioManager.h"

#include "Scripting/ScriptQueue.h"

class Chunk;

class Entity;

enum class LimbSection {
    HEAD, // High section (head(s), etc.)
    BODY, // Middle section (arms, torso, etc.)
    LEGS  // Low section (legs, feet, etc.)
};

class Limb { // Display purposes only (displaying proper weapon coords, armour, etc.)
    public:
        Limb(Entity* parentEnt, GLint textureId, glm::vec2 relPos, glm::vec2 size, float angle, glm::vec2 pointOfRotation = glm::vec2(0.0f)) :
             m_parentEntity(parentEnt),
             m_textureId(textureId),
             m_pos(relPos),
             m_size(size),
             m_angle(angle),
             m_por(pointOfRotation) { }

        void setPosition(glm::vec2 newPos) { m_pos = newPos; }
        void setAngle(float newAngle) { m_angle = newAngle; }
        void setSpeed(float speed) { m_speed = speed; }

        glm::vec2 getPosition() { return m_pos; }
        float getAngle() { return m_angle; }

        virtual void draw(GLEngine::SpriteBatch& sb);

        virtual void animate() {}

    protected:
        virtual void updateAngle() {}

        //LimbSection m_section; // Defense time
        //ArmourPiece m_armour;

        Entity* m_parentEntity = nullptr;
        GLint m_textureId = (GLuint)-1;
        glm::vec2 m_pos = glm::vec2(1.0f);
        glm::vec2 m_size = glm::vec2(1.0f);
        float m_angle = 0.0f;
        float m_speed = 0.0f;
        glm::vec2 m_por = glm::vec2(0.0f);
};
#include <iostream>
class Leg : public Limb { // Display purposes only (displaying proper weapon coords, armour, etc.)
    public:
        Leg(Entity* parentEnt, GLint textureId, glm::vec2 relPos, glm::vec2 size, float angle, float swingAmntRadians) : Limb(parentEnt, textureId, relPos, size, angle), m_swingAmnt(swingAmntRadians), m_centreAngle(angle) {}

        virtual void animate() {
            m_angle += std::cos(m_cosTracker) * m_swingAmnt;
            m_cosTracker += m_speed * 0.1f;
            m_cosTracker *= 0.95f;
        }

    private:
        float m_swingAmnt = 0.0f;
        float m_centreAngle = 0.0f;
        bool m_directionPos = true;

        float m_cosTracker = 0.0f;
};

class Player;

class Entity
{
    friend class Scripter;

    public:
        Entity(glm::vec2 position,
               AudioManager* audioManager,
               ScriptQueue* sq,
               float maxRunningSpeed,
               Categories::LootTableIds lootTable,
               unsigned int lootBeginLevel, unsigned int lootBeginIndex) :
                                                                           m_position(position),
                                                                           m_audioManager(audioManager),
                                                                           m_sq(sq),
                                                                           m_maxSpeed(maxRunningSpeed),
                                                                           m_inventory(new Inventory()) {
                                                                               if(lootTable != Categories::LootTableIds::NONE) m_lootTableStart = Category_Data::lootTables[(unsigned int)lootTable].getNode(lootBeginLevel, lootBeginIndex);
                                                                           }

        virtual ~Entity();

        //virtual void onInteract(ScriptQueue* sq) {} // Deprecated? Yes. Hotel? Trivago.
        virtual void onTalk(ScriptQueue* sq) {}
        virtual void onTrade(ScriptQueue* sq) {}
        virtual void onDeath(ScriptQueue* sq) {}

        virtual void update(float timeStep, Chunk* worldChunks[WORLD_SIZE]);
        virtual void tick(Player* p);
        virtual void draw(GLEngine::SpriteBatch& sb, float time, float xOffset);
        void move(float timeStepVariable);

        void collide();

        const Categories::Faction&     getFaction()      const { return m_faction; }
        const glm::vec2&               getPosition()     const { return m_position; }
        const glm::vec2&               getSize()         const { return m_size; }
              glm::vec2                getVelocity()     const { return m_velocity; }
              std::vector<glm::vec2>   getTargets()      const { return m_targets; }
              float                    getJumpHeight()   const { return m_jumpHeight; }
              float                    getLightLevel()   const { return m_light; }
              bool                     isDead()          const { return m_isDead; }

        void                           setPosition(glm::vec2 pos)   { m_position = pos; }
        void                           setTargets(std::vector<glm::vec2> targets)  { m_targets = targets; }
        Categories::Entity_Type        getType()         const { return m_type; }

        void setParentChunk(Chunk* chunk);
        void setAudioManager(AudioManager* audio) { m_audioManager = audio; }
        int setParentChunk(Chunk* worldChunks[WORLD_SIZE]);
        unsigned int getChunkIndex();

        void giveItem(Item* item) { if(m_inventory) { m_inventory->addItem(item); } else { Logger::getInstance()->log("ERROR: Entity inventory not initialized, could not give item", true); } }
        Inventory* getInventory() { return m_inventory; }

        virtual bool canTalk() const { return false; }
        virtual bool canTrade() const { return true; }

        virtual void attack(Entity* enemy); /// TODO: make swinging swords etc. by making a new Entity (SwordBlade), and attaching it to this Entity, with a customized collide function
        virtual void defend(Entity* attacker, float damage, LimbSection section, Item* weapon);

    protected:
        bool checkTilePosition(std::vector<glm::vec2>& collideTilePositions, float xPos, float yPos);
        void collideWithTile(glm::vec2 tilePos, bool ground = false);
        void updateLightLevel();
        virtual void updateAI() {};
        virtual void updateLimbs() {
            for(unsigned int i = 0; i < m_limbs.size(); i++) {
                m_limbs[i]->setSpeed(std::abs(m_velocity.x * 2.0f));
                m_limbs[i]->animate();
            }
        }
        void updateMovement();
        virtual void updateSounds();

        virtual void die();

        Inventory* m_inventory = nullptr;
        std::vector<unsigned int> m_equippedItems; // For armour, weapons, etc.

        DropDatum m_lootTableStart = DropDatum(0, (Categories::ItemIDs)(0), 0, 0, -1.0f);
        unsigned int m_lootRolls = 0; // How many times the table is rolled on death

        bool m_exposedToSun = false;

        float m_fallenDistance = 0.0f;
        bool m_takesFallDamage = true;

        bool m_isDead = false;
        bool m_canDie = true;

        unsigned int m_animation_jumpFrames = 3;
        unsigned int m_animation_runFrames = 3;

        bool m_controls[4]; // Up, down (crouching while on ground), left, right
        float m_speed = 0.02;
        float m_jumpHeight = 0.767f * 10.0f; // Jumpheight == y = (2.736t+(-0.098t^2))/8
                                            /*
                                                Calculate:
                                                y = (-.1(x-5j)^2 + 2.5j^2) / 4
                                                Where y is the jumpheight (in blocks), x is the time (in frames), and j is the jumpheight

                                                or

                                                2.5j^2 = 8y
                                                Where y is the highest height in the jump, in blocks

                                                and

                                                y = (h * 4) / x + .1x
                                                Where h is the desired height (in blocks), y is the jumpheight, and x is time (in frames)

                                            */
        std::vector<glm::vec2> m_targets;
        unsigned int m_curTarget = 0;

        GLEngine::GLTexture m_texture;
        float m_light = 0.6f;

        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;
        unsigned int m_layer = 0;

        glm::vec2 m_velocity;

        bool m_transparent = false; // Unimplemented

        bool m_onGround = false;

        bool m_flippedTexture = false;

        Categories::AI_Type m_ai = Categories::AI_Type::WALKING;
        Categories::Disability_Type m_disabilities = Categories::Disability_Type::NONE;
        Categories::Attack_Type m_attackType = Categories::Attack_Type::MELEE_ONLY;
        Categories::Entity_Type m_type = Categories::Entity_Type::MOB;

        Chunk* m_parentChunk = nullptr;
        AudioManager* m_audioManager = nullptr;
        ScriptQueue* m_sq = nullptr;

        float m_maxSpeed;

        std::vector<Limb*> m_limbs;

        unsigned int m_soundTimer = 0;
        bool m_makesNoise = false; // Does this mob make ambient noise? (Mooing, etc.)
        unsigned int m_noiseFrequency = 0; // Determines the chance of this mob making a noise
        int m_currentNoise = -1; // The id of the noise the mob is making
        std::vector<SoundEffectIDs> m_ambientNoiseSound; // What ambient noise does this mob make?

        float m_maxHealth = 100.0f;
        float m_health = m_maxHealth;

        /* Entities.h attributes:
        - Texture
        - Faction
        - JumpHeight
        - Position
        - Size
        - Transparency
        - AI Type
        - Disability Types
        - Attack Types
        - Max Health
        */

};
