#pragma once

#include <string>

namespace Categories {

	enum class Faction { BENIGN, GOOD, NEUTRAL, BAD, EVIL };

	Faction		getFactionFromString(std::string str);
	std::string getStringFromFaction(Faction faction);

	enum class AI_Type { WALKING, FLYING, SWIMMING, NONE };

	enum class Disability_Type { NONE, BLIND, LAME, DEAF };

	enum class LootTableIds { ANIMAL, DEMON, HUMANOID, UNIQUE, NONE };

}; // namespace Categories
