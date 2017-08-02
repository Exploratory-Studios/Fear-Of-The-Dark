#pragma once

<<<<<<< HEAD
#include <vector>

#include "Block.h"
#include "Categories.h"

=======
>>>>>>> 98b8385241da9920f404ce8b5264336ac8b85c16
class Entity
{
    public:
        Entity();
        virtual ~Entity();

        virtual void update();
        virtual void draw();

<<<<<<< HEAD
        void collide(std::vector<Entity>& entities, std::vector<Block> blocks);

        const Categories::Faction&     getFaction()  const { return m_faction; }
        const glm::vec2&               getPosition() const { return m_position; }
        const glm::vec2&               getSize()     const { return m_size; }

        void                           setPosition(glm::vec2 pos)   { m_position = pos; }

    private:
        Categories::Faction m_faction;

        glm::vec2 m_position;
        glm::vec2 m_size;

=======
>>>>>>> 98b8385241da9920f404ce8b5264336ac8b85c16
};

