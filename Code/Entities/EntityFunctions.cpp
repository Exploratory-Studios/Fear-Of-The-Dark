#include "EntityFunctions.h"

namespace EntityFunctions {

        void WalkingAI(bool (&controls)[6], std::vector<glm::vec3>& targets, unsigned int& currentTarget, glm::vec2& velocity, glm::vec2& size, glm::vec3 position) {

            if(targets.size() > currentTarget) {
                float targetXPos = targets[currentTarget].x;
                float xPos = position.x;

                float xDist;

                if(xPos < targetXPos) {
                    float xDist0 = targetXPos - xPos; // Regular dist not accounting for crossover.
                    float xDist1 = (xPos) + (WORLD_SIZE - targetXPos); // Distance accounting for crossover.

                    if(xDist0 < xDist1) {
                        // Go right
                        controls[3] = true;
                        controls[2] = false;

                        xDist = xDist0;
                    } else {
                        // Go left
                        controls[2] = true;
                        controls[3] = false;

                        xDist = xDist1;
                    }
                } else if(xPos > targetXPos) {
                    float xDist0 = xPos - targetXPos; // Regular dist not accounting for crossover.
                    float xDist1 = (targetXPos) + (WORLD_SIZE - xPos); // Distance accounting for crossover.

                    if(xDist0 < xDist1) {
                        // Go left
                        controls[2] = true;
                        controls[3] = false;

                        xDist = xDist0;
                    } else {
                        // Go right
                        controls[3] = true;
                        controls[2] = false;

                        xDist = xDist1;
                    }
                }

                float yDist = std::abs(position.y - targets[currentTarget].y);
                float zDist = std::abs(position.z - targets[currentTarget].z);

                float totalDist = std::sqrt(xDist*xDist + yDist*yDist + zDist*zDist);

                if((int)targets[currentTarget].y - (int)position.y >= 1.0f/2.0f) {
                    controls[0] = true; // UP
                }
                if(position.z > targets[currentTarget].z) {
                    controls[5] = true; // FORWARDS
                } else if(position.z < targets[currentTarget].z) {
                    controls[4] = true; // BACKWARDS
                }

                if(totalDist <= 0.75f) {
                    currentTarget++;

                    if(currentTarget >= targets.size()) {
                        targets.clear();
                        currentTarget = 0;
                    }
                }
            }
        }

}
