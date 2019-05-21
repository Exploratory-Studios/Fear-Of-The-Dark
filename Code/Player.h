#pragma once

#include <InputManager.h>
#include <Camera2D.h>
#include <SDL2/SDL.h>
#include <SpriteBatch.h>
#include <GUI.h>

#include "Item.h"
#include "Items.h"
#include "Entity.h"
#include "Block.h"
#include "Inventory.h"

#include "PresetValues.h"

class Chunk;

class Player : public Entity {
    friend class GameplayScreen;
    friend class WorldIOManager;

    public:
        Player(glm::vec2 position, GLEngine::InputManager* input, ScriptQueue* sq);

        void onInteract(ScriptQueue* sq) {}
        void onDeath(ScriptQueue* sq) {}

        void initGUI(GLEngine::GUI* gui);

        virtual ~Player();

        void draw(GLEngine::SpriteBatch& sb, float time, float xOffset) override;
        void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);
        void update(float timeStep, Chunk* worldChunks[WORLD_SIZE]) override;
        void updateMouse(GLEngine::Camera2D* worldCamera);

        void setCanInteract(bool setting) { m_canInteract = setting; }

    protected:
        virtual void updateAI() {}
        virtual void updateMovement() {}

        GLEngine::InputManager* m_input = nullptr;
        GLEngine::GUI* m_gui = nullptr;

        CEGUI::PopupMenu* m_statusBoxFrame = nullptr;

        CEGUI::ProgressBar* m_sanityBar = nullptr;
        CEGUI::ProgressBar* m_healthBar = nullptr;
        CEGUI::ProgressBar* m_thirstBar = nullptr;
        CEGUI::ProgressBar* m_hungerBar = nullptr;
        CEGUI::ProgressBar* m_exhaustionBar = nullptr;
        CEGUI::ProgressBar* m_staminaBar = nullptr;

        CEGUI::DefaultWindow* m_statusBoxLabel = nullptr;

        glm::vec2 m_mousePos;
        Block* m_selectedBlock = nullptr;
        Entity* m_selectedEntity = nullptr;

        int m_selectedHotbox = 0;

        Inventory* m_inventory = nullptr;

        bool m_inventoryOpen = false;
        bool m_debuggingInfo = false; // FPS, selectedBlock, etc.
        bool m_canInteract = true; // Sets if the player can't interact with objects, people, etc. Used when player is in cutscenes, talking, etc.

        void updateInput();

        float m_sanity = 1.0f; // Fear level will be displayed as a heartbeat sound
        float m_health = 1.0f;
        float m_thirst = 1.0f;
        float m_hunger = 1.0f;
        float m_exhaustion = 1.0f;
        float m_stamina = 1.0f;

        unsigned int m_scriptID_dayTime = 0;
        unsigned int m_scriptID_makeHouse = 0;

        Item* m_favouriteItems[10] = { nullptr };

};
