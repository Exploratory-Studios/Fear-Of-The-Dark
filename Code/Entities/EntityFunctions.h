#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "../PresetValues.h"

namespace EntityFunctions {

    void WalkingAI(bool (&controls)[4], std::vector<glm::vec2>& targets, unsigned int& currentTarget, glm::vec2& velocity, glm::vec2& size, glm::vec2& position);

}
