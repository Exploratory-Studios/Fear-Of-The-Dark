#pragma once

class Physics {
    public:
        /// returns an array of the sides that the rect is colliding with
        /// 0 = TOP, 1 = BOTTOM, 2 = LEFT, 3 = RIGHT
        static bool[4] intersectingRects(glm::vec2 pos1, glm::vec2 dims1, glm::vec2 pos2, glm::vec2 pos2, float &diff);
};
