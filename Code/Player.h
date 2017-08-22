#pragma once

#include <InputManager.h>
#include <SDL/SDL.h>

#include "Entity.h"
#include "Chunk.h"

#include "PresetValues.h"

class Player : public Entity
{
    public:
        Player();
        Player(glm::vec2 position, GLEngine::InputManager* input);
        virtual ~Player();

        void update(Chunk* chunks[WORLD_SIZE]) override;

    protected:
        GLEngine::InputManager* m_input;

};
