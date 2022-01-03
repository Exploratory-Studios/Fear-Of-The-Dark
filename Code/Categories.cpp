#include "Categories.h"

#include <BAREErrors.hpp>

namespace Categories {
	Faction getFactionFromString(std::string str) {
		if(str == "benign") {
			return Faction::BENIGN;
		} else if(str == "good") {
			return Faction::GOOD;
		} else if(str == "neutral") {
			return Faction::NEUTRAL;
		} else if(str == "bad") {
			return Faction::BAD;
		} else if(str == "evil") {
			return Faction::EVIL;
		}
		BARE2D::throwFatalError(BARE2D::BAREError::OTHER_ERROR, "Could not decipher faction from string: " + str);
	}

	std::string getStringFromFaction(Faction faction) {
		if(faction == Faction::BENIGN) {
			return "benign";
		} else if(faction == Faction::GOOD) {
			return "good";
		} else if(faction == Faction::NEUTRAL) {
			return "neutral";
		} else if(faction == Faction::BAD) {
			return "bad";
		} else if(faction == Faction::EVIL) {
			return "evil";
		}
	}
} // namespace Categories
