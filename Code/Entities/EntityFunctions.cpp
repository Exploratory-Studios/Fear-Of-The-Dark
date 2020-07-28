#include "EntityFunctions.h"

#include "EntityNPC.h"

namespace EntityFunctions {

	void basic_straight(bool (&controls)[6], std::vector<glm::vec3>& targets, unsigned int& currentTarget, EntityNPC* entity) {
		// The targets are aligned to the bottom left corner of each tile (+0), **not** the middle (+size/2)

		// Controls:
		// +y/-y: 0/1 (up/down)
		// +x/-x: 3/2 (left/right)
		// +z/-z: 4/5 (away from cam/towards cam)

		/* Algorithm is as follows:
		 *  - Dumb, basically just move towards the next target in a straight line
		 *    - If Xt < Xe, move right (-x), else left (+x)
		 *    - If Yt < Ye, move down (-y), else up (+y)
		 *    - If Zt < Ze, move -z, else +z
		 *  - Naturally, if X/Y/Zt == X/Y/Ze, then don't activate those controls and move onto the next target.
		 *  - Take into account a small threshold (maybe 1/8th of a block?) that an entity can be within to move onto the next target
		 */

		// Make sure we aren't finished already lol
		if(currentTarget >= targets.size()) {
			controls[0] = false; // Do nothing
			controls[1] = false; // Do nothing
			controls[2] = false; // Do nothing
			controls[3] = false; // Do nothing
			controls[4] = false; // Do nothing
			controls[5] = false; // Do nothing
			return;
		}

		// Determine X, Y, and Z distances
		const glm::vec3 curTarget = targets[currentTarget];
		const float distX = entity->getPosition().x - curTarget.x;
		const float distY = entity->getPosition().y - curTarget.y;
		const float distZ = entity->getLayer() - curTarget.z;
		const float threshold = 1.0f/8.0f; // 1/8th of a block

		// Declare some variables to check if the entity is close enough to a target to move on to the next one
		// If all are true, move on.
		bool XAligned = false;
		bool YAligned = false;
		bool ZAligned = false;

		// Check X direction
		if(distX > threshold) {
			// Xentity > Xtarget, move left
			controls[3] = true;
			controls[2] = false;
		} else if(distX < -threshold) {
			// Other way round, move right
			controls[2] = true;
			controls[3] = false;
		} else {
			// Within threshold
			XAligned = true;
		}

		// Check Y direction
		if(distY > threshold) {
			// Yentity > Ytarget, move down
			controls[1] = true;
			controls[0] = false;
		} else if(distY < -threshold) {
			// Other way round, move up
			controls[0] = true;
			controls[1] = false;
		} else {
			// Within threshold
			YAligned = true;
		}

		// Check Y direction
		if(distX > threshold) {
			// Xentity > Xtarget, move toward cam
			controls[4] = true;
			controls[5] = false;
		} else if(distX < -threshold) {
			// Other way round, move away from cam
			controls[5] = true;
			controls[4] = false;
		} else {
			// Within threshold
			ZAligned = true;
		}

		if(XAligned && YAligned && ZAligned) {
			// All aligned, move onto next target
			currentTarget++;
		}

	}

}
