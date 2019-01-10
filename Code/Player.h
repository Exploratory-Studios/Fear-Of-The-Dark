#pragma once

#include <InputManager.h>
#include <Camera2D.h>
#include <SDL2/SDL.h>
#include <SpriteBatch.h>

#include "Entities/TalkingNPC.h"

#include "Item.h"
#include "Block.h"
#include "Inventory.h"

#include "PresetValues.h"

/// TODO: GET RID OF
#include "Categories.h"

class Chunk;

class Player : public Entity {
    public:
        Player();
        Player(glm::vec2 position, GLEngine::InputManager* input);
        virtual ~Player();

        void draw(GLEngine::SpriteBatch& sb, float time) override;
        void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);
        void update(float timeStep, Chunk* worldChunks[WORLD_SIZE]) override;
        void updateMouse(GLEngine::Camera2D* worldCamera);

        TalkingNPC* getTalkingEntity() { return m_speakingEntity; }

        void setSpeakingEntity(TalkingNPC* entity) { m_speakingEntity = entity; }
        void setCanInteract(bool setting) { m_canInteract = setting; }

    protected:
        virtual void updateAI(Chunk* activeChunks[WORLD_SIZE]) {}
        virtual void updateMovement() {}

        GLEngine::InputManager* m_input = nullptr;

        glm::vec2 m_mousePos;
        Block* m_selectedBlock = nullptr;

        int m_selectedHotbox = 0;

        Inventory* m_inventory = nullptr;
        TalkingNPC* m_speakingEntity = nullptr;

        bool m_inventoryOpen = false;
        bool m_debuggingInfo = false; // FPS, selectedBlock, etc.
        bool m_canInteract = true; // Sets if the player can't interact with objects, people, etc. Used when player is in cutscenes, talking, etc.

        void updateInput();

};
