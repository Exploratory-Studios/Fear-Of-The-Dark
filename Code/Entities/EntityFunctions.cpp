#include "EntityFunctions.h"

namespace EntityFunctions {

        void WalkingAI(bool (&controls)[4], std::vector<glm::vec2>& targets, unsigned int& currentTarget, glm::vec2& velocity, glm::vec2& size, glm::vec2& position) {

            if(targets.size() > currentTarget) {
                float targetXPos = targets[currentTarget].x;
                float xPos = position.x + size.x / 2.0f;

                unsigned int dist1 = std::abs(xPos - targetXPos);
                unsigned int dist2 = (xPos < targetXPos ? xPos : targetXPos) + WORLD_SIZE * CHUNK_SIZE - (xPos >= targetXPos ? xPos : targetXPos);

                unsigned int leftDist = xPos > targetXPos ? dist1 : dist2; // Distance if we go left
                unsigned int rightDist = xPos <= targetXPos ? dist1 : dist2; // Distance if we go right

                /// TODO: 'Tis but a dream, really
                //float distToDecelerate = velocity.x / std::pow(5.0f, std::log(0.1f * velocity.x) / std::log(5));

                if(leftDist >= rightDist) {
                    controls[3] = true; // RIGHT
                    controls[2] = false;
                }
                if(leftDist < rightDist) {
                    controls[2] = true; // LEFT
                    controls[3] = false;
                }
                if((int)targets[currentTarget].y - (int)position.y >= 1.0f/2.0f) {
                    controls[0] = true;
                }

                if(std::abs(xPos + 0.5f - targetXPos) <= 0.5f) {
                    currentTarget++;
                    if(currentTarget >= targets.size()) {
                        targets.clear();
                        currentTarget = 0;
                    }
                }
            }
        }

}
