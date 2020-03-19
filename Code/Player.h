#pragma once

#include <InputManager.h>
#include <Camera2D.h>
#include <SDL2/SDL.h>
#include <SpriteBatch.h>
#include <GUI.h>

#include "Item.h"
#include "Items.h"
#include "Entity.h"
#include "Inventory.h"
#include "BuffData.h"

#include "PresetValues.h"

class Chunk;

class Player : public Entity {
    friend class GameplayScreen;
    friend class WorldIOManager;
    friend class Scripter;

    public:
        Player(glm::vec2 position, unsigned int layer, bool loadTexture = true);

        void onInteract(ScriptQueue* sq) {}
        void onDeath(ScriptQueue* sq) {}

        void initGUI(GLEngine::GUI* gui);

        virtual ~Player();

        void draw(GLEngine::SpriteBatch& sb, float time, int layerDifference, float xOffset) override;
        void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);
        void update(World* world, AudioManager* audio, float timeStep) override;
        void updateStats(World* world, float timeStep);
        void updateMouse(World* world, glm::vec2 mouseCoords);
        void updateInput(GLEngine::InputManager* input, World* world, ScriptQueue* sq);

        void showInventory(bool open) { m_inventoryOpen = open; }

        float getSanity() { return m_sanity; }

        void setSanity(float sanity) { m_sanity = sanity; } // I'm running low on this

        float getTemperature() { return 1; }

        Entity* getSelectedEntity() { return m_selectedEntity; }
        Tile* getSelectedBlock() { return m_selectedBlock; }

        void setCanInteract(bool setting) { m_canInteract = setting; }
        bool canInteract() const { return m_canInteract; }

        void setGodMode(bool mode) { m_godMode = mode; }

    protected:
        virtual void updateAI() {}
        virtual void updateMovement() {}

        void godMove();

        GLEngine::InputManager* m_input = nullptr;
        GLEngine::GUI* m_gui = nullptr;

        CEGUI::PopupMenu* m_statusBoxFrame = nullptr;
        CEGUI::PopupMenu* m_buffBoxFrame = nullptr;

        CEGUI::ProgressBar* m_sanityBar = nullptr;
        CEGUI::ProgressBar* m_healthBar = nullptr;
        CEGUI::ProgressBar* m_thirstBar = nullptr;
        CEGUI::ProgressBar* m_hungerBar = nullptr;
        CEGUI::ProgressBar* m_exhaustionBar = nullptr;
        CEGUI::ProgressBar* m_staminaBar = nullptr;

        CEGUI::DefaultWindow* m_statusBoxLabel = nullptr;

        Tile* m_selectedBlock = nullptr;
        Entity* m_selectedEntity = nullptr;

        int m_selectedHotbox = 0;

        bool m_inventoryOpen = false;
        bool m_questListOpen = false;
        bool m_skillTreeOpen = false;

        bool m_debuggingInfo = false; // FPS, selectedBlock, etc.
        bool m_canInteract = true; // Sets if the player can't interact with objects, people, etc. Used when player is in cutscenes, talking, etc.

        float m_sanity = 1.0f; // Fear level will be displayed as a heartbeat sound (fear is how fast your sanity drops)
        float m_health = 1.0f;
        float m_thirst = 1.0f;
        float m_hunger = 1.0f;
        float m_exhaustion = 1.0f;
        float m_stamina = 1.0f;

        unsigned int m_scriptID_dayTime = 0;
        unsigned int m_scriptID_makeHouse = 0;

        Item* m_favouriteItems[10] = { nullptr };
        std::vector<Buff*> m_buffs;

        bool m_godMode = false;

        bool m_loadedTexture = false;

};
