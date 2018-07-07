#pragma once

#include <InputManager.h>
#include <Camera2D.h>
#include <SDL2/SDL.h>
#include <SpriteBatch.h>

#include "Entity.h"
#include "Chunk.h"
#include "Block.h"
#include "Inventory.h"
#include "Item.h"

#include "PresetValues.h"





/// GET RID OF
#include "Categories.h"

class Player : public Entity
{
    public:
        Player();
        Player(glm::vec2 position, GLEngine::InputManager* input);
        virtual ~Player();

        void draw(GLEngine::SpriteBatch& sb) override;
        void drawGUI(GLEngine::SpriteBatch& sb, GLEngine::SpriteFont& sf);
        void update(Chunk* chunks[WORLD_SIZE], float timeStep) override;
        void updateMouse(Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera);

    protected:
        virtual void updateAI(Chunk* activeChunks[WORLD_SIZE]) {}
        virtual void updateMovement() {}

        GLEngine::InputManager* m_input = nullptr;

        Block* m_selectedBlock = nullptr;
        Item* m_handItem;

        int m_selectedHotbox = 0;

        Inventory* m_inventory = nullptr;

        bool m_inventoryOpen = false;

        void updateInput();

};
