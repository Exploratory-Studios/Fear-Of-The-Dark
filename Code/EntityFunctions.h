#pragma once

#include <glm/glm.hpp>
#include <vector>

class EntityNPC;

namespace EntityFunctions {

	// This one works for both flying and walking dummies because they just activate up/down controls if the target is above/below
	void basic_straight(bool (&controls)[6],
						std::vector<glm::vec3>& targets,
						unsigned int&			currentTarget,
						EntityNPC*				entity);

} // namespace EntityFunctions
