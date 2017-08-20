#pragma once

#include "Entity.h"

class Player : public Entity
{
    public:
        Player();
        Player(glm::vec2 position);
        virtual ~Player();

    protected:

};
