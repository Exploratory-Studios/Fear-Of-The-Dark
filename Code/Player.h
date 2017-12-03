#pragma once

#include <InputManager.h>
#include <Camera2D.h>
#include <SDL/SDL.h>

#include "Entity.h"
#include "Chunk.h"
#include "Block.h"
#include "Inventory.h"

#include "PresetValues.h"

class Player : public Entity
{
    public:
        Player();
        Player(glm::vec2 position, GLEngine::InputManager* input);
        virtual ~Player();

        void draw(GLEngine::SpriteBatch& sb) override;
        void drawGUI(GLEngine::SpriteBatch& sb);
        void update(Chunk* chunks[WORLD_SIZE]) override;
        void updateMouse(Chunk* chunks[WORLD_SIZE], GLEngine::Camera2D* worldCamera);

    protected:
        GLEngine::InputManager* m_input;

        Block* m_selectedBlock = nullptr;

        Inventory* m_inventory;

};
