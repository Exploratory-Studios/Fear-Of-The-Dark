#pragma once

#include "Entity.h"
#include "Entities/TalkingNPC.h"
#include "Categories.h"

#include <GLEngineErrors.h>
#include <string>

/// This file holds and handles all new Entities and TalkingNPCs

class EntityCompanionCube : public Entity {
    public:
        EntityCompanionCube(glm::vec2 position, unsigned int id, AudioManager* audioManager, ScriptQueue* sq);

};

class EntityTalkingCivilian : public TalkingNPC {
    public:
        EntityTalkingCivilian(glm::vec2 position, unsigned int id, AudioManager* audioManager, ScriptQueue* sq);
};
