#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../PresetValues.h"

namespace EntityFunctions {

    void WalkingAI(bool (&controls)[6], std::vector<glm::vec3>& targets, unsigned int& currentTarget, glm::vec2& velocity, glm::vec2& size, glm::vec3 position);

}
