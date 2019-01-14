#include "EntityFunctions.h"

#include <iostream>

namespace EntityFunctions {

    void WalkingAI(bool (&controls)[4], std::vector<glm::vec2>& targets, unsigned int& currentTarget, glm::vec2& velocity, glm::vec2& size, glm::vec2& position) {
        controls[0] = false;
        controls[1] = false;
        controls[2] = false;
        controls[3] = false;

        if(targets.size() > currentTarget) {
            float modifier = 0.0f;
            if(currentTarget == targets.size()-1) modifier = abs(velocity.x) * 1.25f;
            if(targets[currentTarget].x - position.x > 0.5f * TILE_SIZE) {
                controls[3] = true; // RIGHT
                controls[2] = false;
            }
            if(targets[currentTarget].x - position.x < 0.5f * TILE_SIZE) {
                controls[2] = true; // LEFT
                controls[3] = false;
            }
            if((int)targets[currentTarget].y < (int)position.y - 0.5f * TILE_SIZE) {
                controls[0] = true;
            }
            if(abs(position.x - targets[currentTarget].x) <= 1.0f) {
                currentTarget++;
                if(currentTarget >= targets.size()) {
                    targets.clear();
                    currentTarget = 0;
                }
            }
        }
    }

}
